"""
elle_video_worker.py — External Wav2Lip / ffmpeg worker for Elle-Ann.

Polls the C++ HTTP service for queued video_jobs, synthesizes lip-synced
video from (text, avatar_image), posts back progress + final output.

Design (mirrors legacy Python video_generator.py, now decoupled from the
C++ core so this runs on a GPU box while the core runs anywhere):

   [Android/User] → POST /api/video/generate
            ↓
   [video_jobs table] — queued
            ↓
   [this worker] polls /api/video/worker/claim
            ↓
   1. TTS(text)           → temp WAV
   2. Wav2Lip(avatar+wav) → lip-synced MP4
   3. (optional) GFPGAN   → face-enhance upscale
   4. ffmpeg mux          → final MP4
   5. POST /worker/progress/{id}  ×N
   6. POST /worker/complete/{id}  output_path
            ↓
   [Android] polls /api/video/status/{id} → sees done

Environment:
  ELLE_API_BASE       (default http://localhost:8000)
  WAV2LIP_CHECKPOINT  path to Wav2Lip .pth  (required)
  WAV2LIP_INFER       python script path    (required — Wav2Lip's inference.py)
  GFPGAN_SCRIPT       optional GFPGAN inference.py path
  FFMPEG_BIN          default 'ffmpeg'
  TTS_ENGINE          'edge-tts' (default) | 'pyttsx3'
  VOICE_NAME          default 'en-US-JennyNeural'
  WORK_DIR            scratch dir for intermediate files (default ./work)
  OUTPUT_DIR          final mp4 output dir (default ./output)
  POLL_INTERVAL_SEC   default 3
"""

from __future__ import annotations

import os
import sys
import time
import uuid
import json
import shutil
import logging
import tempfile
import subprocess
from pathlib import Path
from typing import Optional

import requests

# ─── configuration ──────────────────────────────────────────────────────────
API_BASE         = os.environ.get("ELLE_API_BASE", "http://localhost:8000")
WAV2LIP_CKPT     = os.environ.get("WAV2LIP_CHECKPOINT", "")
WAV2LIP_INFER    = os.environ.get("WAV2LIP_INFER", "")
GFPGAN_SCRIPT    = os.environ.get("GFPGAN_SCRIPT", "")
FFMPEG_BIN       = os.environ.get("FFMPEG_BIN", "ffmpeg")
TTS_ENGINE       = os.environ.get("TTS_ENGINE", "edge-tts")
VOICE_NAME       = os.environ.get("VOICE_NAME", "en-US-JennyNeural")
WORK_DIR         = Path(os.environ.get("WORK_DIR", "./work")).resolve()
OUTPUT_DIR       = Path(os.environ.get("OUTPUT_DIR", "./output")).resolve()
POLL_INTERVAL    = float(os.environ.get("POLL_INTERVAL_SEC", "3"))

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)s %(message)s",
    datefmt="%H:%M:%S",
)
log = logging.getLogger("elle.video.worker")


# ─── HTTP helpers against the C++ worker API ───────────────────────────────
def claim_job() -> Optional[dict]:
    r = requests.post(f"{API_BASE}/api/video/worker/claim", timeout=10)
    r.raise_for_status()
    body = r.json()
    return body if body.get("claimed") else None


def post_progress(job_id: str, pct: int) -> None:
    try:
        requests.post(
            f"{API_BASE}/api/video/worker/progress/{job_id}",
            json={"progress": int(pct)},
            timeout=5,
        )
    except requests.RequestException as ex:
        log.warning("progress POST failed: %s", ex)


def post_complete(job_id: str, output_path: str) -> None:
    """Notify the server of successful completion.

    Previously this swallowed all errors — a transient 5xx from the API
    would leave the local worker thinking the job was finished while the
    server never recorded it. Now we raise so the caller can move the
    job to post_fail() and keep the system consistent.
    """
    resp = requests.post(
        f"{API_BASE}/api/video/worker/complete/{job_id}",
        json={"output_path": output_path},
        timeout=10,
    )
    resp.raise_for_status()


def post_fail(job_id: str, err: str) -> None:
    try:
        requests.post(
            f"{API_BASE}/api/video/worker/fail/{job_id}",
            json={"error": err[:2000]},
            timeout=10,
        )
    except requests.RequestException:
        pass


# ─── pipeline steps ─────────────────────────────────────────────────────────
def synth_tts(text: str, out_wav: Path) -> None:
    """Text → WAV. Falls back to pyttsx3 if edge-tts isn't installed."""
    if TTS_ENGINE == "edge-tts":
        # edge-tts produces mp3 by default — ask for wav via --output then convert.
        mp3 = out_wav.with_suffix(".mp3")
        subprocess.run(
            ["edge-tts", "--voice", VOICE_NAME, "--text", text, "--write-media", str(mp3)],
            check=True,
        )
        subprocess.run(
            [FFMPEG_BIN, "-y", "-i", str(mp3), "-ar", "16000", "-ac", "1", str(out_wav)],
            check=True,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        mp3.unlink(missing_ok=True)
    else:
        import pyttsx3

        engine = pyttsx3.init()
        engine.save_to_file(text, str(out_wav))
        engine.runAndWait()


def run_wav2lip(avatar: Path, wav: Path, out_mp4: Path) -> None:
    """Invoke Wav2Lip's inference.py as a subprocess.
    The python binary is the one running THIS script, which must have the
    Wav2Lip dependencies (torch, numpy, opencv-python, librosa, etc.) installed.
    """
    if not WAV2LIP_CKPT or not Path(WAV2LIP_CKPT).exists():
        raise RuntimeError(f"WAV2LIP_CHECKPOINT not set or missing: {WAV2LIP_CKPT}")
    if not WAV2LIP_INFER or not Path(WAV2LIP_INFER).exists():
        raise RuntimeError(f"WAV2LIP_INFER not set or missing: {WAV2LIP_INFER}")

    cmd = [
        sys.executable, WAV2LIP_INFER,
        "--checkpoint_path", WAV2LIP_CKPT,
        "--face",            str(avatar),
        "--audio",           str(wav),
        "--outfile",         str(out_mp4),
        "--nosmooth",
    ]
    log.info("wav2lip: %s", " ".join(cmd))
    subprocess.run(cmd, check=True)


def run_gfpgan(in_mp4: Path, out_mp4: Path) -> None:
    """Optional face-enhancement pass. Skipped cleanly if GFPGAN_SCRIPT unset."""
    if not GFPGAN_SCRIPT:
        shutil.move(str(in_mp4), str(out_mp4))
        return

    # Extract frames, enhance, reassemble.
    frames_dir = in_mp4.parent / (in_mp4.stem + "_frames")
    enhanced_dir = in_mp4.parent / (in_mp4.stem + "_enhanced")
    frames_dir.mkdir(exist_ok=True)
    enhanced_dir.mkdir(exist_ok=True)

    subprocess.run(
        [FFMPEG_BIN, "-y", "-i", str(in_mp4), str(frames_dir / "frame_%05d.png")],
        check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    )
    subprocess.run(
        [sys.executable, GFPGAN_SCRIPT,
         "-i", str(frames_dir), "-o", str(enhanced_dir), "-s", "2"],
        check=True,
    )
    # Reassemble (assume 25 fps — Wav2Lip's default).
    subprocess.run(
        [FFMPEG_BIN, "-y",
         "-framerate", "25",
         "-i", str(enhanced_dir / "restored_imgs" / "frame_%05d.png"),
         "-i", str(in_mp4),
         "-c:v", "libx264", "-pix_fmt", "yuv420p",
         "-c:a", "aac", "-map", "0:v:0", "-map", "1:a:0",
         str(out_mp4)],
        check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    )
    shutil.rmtree(frames_dir, ignore_errors=True)
    shutil.rmtree(enhanced_dir, ignore_errors=True)


def process_job(job: dict) -> None:
    job_id     = job["job_id"]
    text       = job["text"]
    avatar     = Path(job["avatar_path"]).resolve()
    log.info("claim job_id=%s text=%.60s avatar=%s", job_id, text, avatar)

    if not avatar.exists():
        raise RuntimeError(f"avatar path missing on this worker: {avatar}")

    WORK_DIR.mkdir(parents=True, exist_ok=True)
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    scratch = WORK_DIR / f"job_{job_id}_{uuid.uuid4().hex[:6]}"
    scratch.mkdir()

    success = False
    try:
        post_progress(job_id, 5)

        wav      = scratch / "voice.wav"
        raw_mp4  = scratch / "raw.mp4"
        final_mp4 = OUTPUT_DIR / f"{job_id}.mp4"

        log.info("step 1/4: tts")
        synth_tts(text, wav)
        post_progress(job_id, 25)

        log.info("step 2/4: wav2lip")
        run_wav2lip(avatar, wav, raw_mp4)
        post_progress(job_id, 70)

        log.info("step 3/4: gfpgan+mux (%s)",
                 "enabled" if GFPGAN_SCRIPT else "mux only")
        run_gfpgan(raw_mp4, final_mp4)
        post_progress(job_id, 95)

        log.info("step 4/4: commit (%s)", final_mp4)
        post_complete(job_id, str(final_mp4))
        log.info("job %s DONE (%s)", job_id, final_mp4)
        success = True

    finally:
        # Honour the comment: scratch dir stays on failure for debugging,
        # gets wiped on success so tmp artifacts don't accumulate forever.
        # Previously this block was `pass` — every successful job leaked
        # the entire scratch tree. After ~1000 jobs you're out of disk.
        if success:
            shutil.rmtree(scratch, ignore_errors=True)
        else:
            log.warning("keeping scratch for post-mortem: %s", scratch)


def main() -> int:
    log.info("Elle-Ann video worker starting. api_base=%s poll=%.1fs",
             API_BASE, POLL_INTERVAL)

    # Sanity-check required tooling so we fail early, not mid-job.
    if not shutil.which(FFMPEG_BIN):
        log.error("ffmpeg not found on PATH (FFMPEG_BIN=%s)", FFMPEG_BIN)
        return 2
    if not WAV2LIP_CKPT or not Path(WAV2LIP_CKPT).exists():
        log.error("WAV2LIP_CHECKPOINT missing (%s) — set the env var before running",
                  WAV2LIP_CKPT)
        return 2
    if not WAV2LIP_INFER or not Path(WAV2LIP_INFER).exists():
        log.error("WAV2LIP_INFER missing (%s)", WAV2LIP_INFER)
        return 2

    while True:
        try:
            job = claim_job()
        except requests.RequestException as ex:
            log.warning("claim failed: %s (retrying in %.0fs)", ex, POLL_INTERVAL)
            time.sleep(POLL_INTERVAL)
            continue

        if not job:
            time.sleep(POLL_INTERVAL)
            continue

        try:
            process_job(job)
        except subprocess.CalledProcessError as ex:
            log.exception("subprocess failed on job %s: %s", job.get("job_id"), ex)
            post_fail(job["job_id"], f"subprocess rc={ex.returncode}: {ex}")
        except Exception as ex:
            log.exception("job failed")
            post_fail(job.get("job_id", "?"), str(ex))


if __name__ == "__main__":
    sys.exit(main() or 0)
