"""
elle_video_worker.py — External Wav2Lip / ffmpeg worker for Elle-Ann.

Polls the C++ HTTP service for queued video_jobs, synthesizes lip-synced
video from (text, avatar_image), posts back progress + final output.

Design (mirrors legacy Python video_generator.py, now decoupled from the
C++ core so this runs on a GPU box while the core runs anywhere):

   [Android/User] -> POST /api/video/generate
            v
   [video_jobs table] -- queued
            v
   [this worker] polls /api/video/worker/claim
            v
   1. TTS(text)           -> temp WAV
   2. Wav2Lip(avatar+wav) -> lip-synced MP4
   3. (optional) GFPGAN   -> face-enhance upscale
   4. ffmpeg mux          -> final MP4
   5. POST /worker/progress/{id}  xN
   6. POST /worker/complete/{id}  output_path
            v
   [Android] polls /api/video/status/{id} -> sees done

Strictness contract (OpSec audit):
  - The claim response is VALIDATED against a strict schema before use.
    Any missing / wrong-type / empty field fails the job immediately
    with a descriptive `post_fail`, the worker does NOT attempt to
    synthesize on malformed input.
  - Every artifact (WAV, raw MP4, final MP4) is verified on disk AFTER
    its producing step: must exist, must be a regular file, must be
    non-empty. A silent ffmpeg/Wav2Lip failure that leaves a 0-byte
    file on disk is caught here, not by the Android user.
  - SIGINT / SIGTERM installed a graceful shutdown: the worker finishes
    polling, refuses new jobs, and if a job is mid-flight fails it
    explicitly (the C++ side requeues) so the process can exit cleanly
    without orphaning an in-flight row.

Environment:
  ELLE_API_BASE       (default http://localhost:8000)
  WAV2LIP_CHECKPOINT  path to Wav2Lip .pth  (required)
  WAV2LIP_INFER       python script path    (required -- Wav2Lip's inference.py)
  GFPGAN_SCRIPT       optional GFPGAN inference.py path
  FFMPEG_BIN          default 'ffmpeg'
  TTS_ENGINE          'edge-tts' (default) | 'pyttsx3'
  VOICE_NAME          default 'en-US-JennyNeural'
  WORK_DIR            scratch dir for intermediate files (default ./work)
  OUTPUT_DIR          final mp4 output dir (default ./output)
  POLL_INTERVAL_SEC   default 3
  MIN_ARTIFACT_BYTES  default 512 (floor for "non-empty" artifact check)
"""

from __future__ import annotations

import os
import sys
import time
import uuid
import signal
import shutil
import logging
import threading
import subprocess
from pathlib import Path
from typing import Optional

import requests

# --- configuration ---------------------------------------------------------
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
MIN_ARTIFACT_BYTES = int(os.environ.get("MIN_ARTIFACT_BYTES", "512"))

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)s %(message)s",
    datefmt="%H:%M:%S",
)
log = logging.getLogger("elle.video.worker")


# --- graceful shutdown -----------------------------------------------------
# The event is set by the SIGINT / SIGTERM handler. The main loop checks it
# at every safe point. Any subprocess we launched will still be terminated
# by the OS when this process exits, and we explicitly `post_fail` the
# in-flight job so the server requeues it instead of stranding it in
# `processing` forever.
_shutdown = threading.Event()


def _install_signal_handlers() -> None:
    def _handler(signum, _frame):
        name = {signal.SIGINT: "SIGINT", signal.SIGTERM: "SIGTERM"}.get(
            signum, f"signal {signum}")
        if _shutdown.is_set():
            log.warning("received %s again -- exiting hard", name)
            os._exit(130)
        log.info("received %s -- entering graceful shutdown", name)
        _shutdown.set()

    # SIGTERM is not defined on Windows for non-console processes, but
    # Python exposes it as an alias for SIGINT there. Register both;
    # signal.signal() no-ops the duplicate.
    for sig in (signal.SIGINT, signal.SIGTERM):
        try:
            signal.signal(sig, _handler)
        except (ValueError, OSError):
            # Not running in main thread, or signal not supported on this
            # platform. Fail closed and log -- the operator must run this
            # as the main thread.
            log.warning("could not install handler for %s", sig)


# --- strict response schema validation -------------------------------------
class ClaimValidationError(ValueError):
    """Raised when the server's claim payload is malformed."""


def _require_nonempty_str(body: dict, key: str) -> str:
    if key not in body:
        raise ClaimValidationError(f"claim response missing required field '{key}'")
    v = body[key]
    if not isinstance(v, str):
        raise ClaimValidationError(
            f"claim response field '{key}' has type {type(v).__name__}, expected str")
    if not v.strip():
        raise ClaimValidationError(f"claim response field '{key}' is empty")
    return v


def _validate_claim(body: dict) -> dict:
    """Return the validated job dict, or raise ClaimValidationError.

    Contract with the C++ HTTP service:
        {"claimed": true, "job_id": "<uuid>", "text": "...", "avatar_path": "..."}
      | {"claimed": false}
    Anything else is a protocol violation.
    """
    if not isinstance(body, dict):
        raise ClaimValidationError(
            f"claim response is {type(body).__name__}, expected object")
    if "claimed" not in body or not isinstance(body["claimed"], bool):
        raise ClaimValidationError("claim response missing/typed bool 'claimed'")
    if not body["claimed"]:
        return {}
    job_id = _require_nonempty_str(body, "job_id")
    text   = _require_nonempty_str(body, "text")
    avatar = _require_nonempty_str(body, "avatar_path")
    return {"job_id": job_id, "text": text, "avatar_path": avatar}


# --- HTTP helpers against the C++ worker API -------------------------------
def claim_job() -> Optional[dict]:
    r = requests.post(f"{API_BASE}/api/video/worker/claim", timeout=10)
    r.raise_for_status()
    try:
        body = r.json()
    except ValueError as ex:
        raise ClaimValidationError(f"claim response was not valid JSON: {ex}") from ex
    validated = _validate_claim(body)
    return validated or None


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

    Previously this swallowed all errors -- a transient 5xx from the API
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


# --- artifact verification -------------------------------------------------
def _verify_artifact(p: Path, label: str) -> None:
    """Assert `p` is a regular file >= MIN_ARTIFACT_BYTES bytes.

    ffmpeg / Wav2Lip occasionally exit 0 yet produce a 0-byte file (out
    of disk, silent codec failure). Without this check that corruption
    would propagate to the user as a "successful" job.
    """
    if not p.exists():
        raise RuntimeError(f"{label} artifact missing: {p}")
    if not p.is_file():
        raise RuntimeError(f"{label} artifact is not a regular file: {p}")
    sz = p.stat().st_size
    if sz < MIN_ARTIFACT_BYTES:
        raise RuntimeError(
            f"{label} artifact too small ({sz} bytes < {MIN_ARTIFACT_BYTES}): {p}")


# --- pipeline steps --------------------------------------------------------
def synth_tts(text: str, out_wav: Path) -> None:
    """Text -> WAV. Falls back to pyttsx3 if edge-tts isn't installed."""
    if TTS_ENGINE == "edge-tts":
        # edge-tts produces mp3 by default -- ask for wav via --output then convert.
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
    # Reassemble (assume 25 fps -- Wav2Lip's default).
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


def _check_shutdown(job_id: str) -> None:
    """Bail out of the pipeline if a shutdown was requested between steps."""
    if _shutdown.is_set():
        raise RuntimeError(
            f"worker shutdown requested mid-job {job_id} -- failing for requeue")


def process_job(job: dict) -> None:
    job_id     = job["job_id"]
    text       = job["text"]
    avatar     = Path(job["avatar_path"]).resolve()
    log.info("claim job_id=%s text=%.60s avatar=%s", job_id, text, avatar)

    if not avatar.exists():
        raise RuntimeError(f"avatar path missing on this worker: {avatar}")
    if not avatar.is_file():
        raise RuntimeError(f"avatar path is not a regular file: {avatar}")
    if avatar.stat().st_size < MIN_ARTIFACT_BYTES:
        raise RuntimeError(f"avatar file suspiciously small: {avatar}")

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

        _check_shutdown(job_id)
        log.info("step 1/4: tts")
        synth_tts(text, wav)
        _verify_artifact(wav, "tts wav")
        post_progress(job_id, 25)

        _check_shutdown(job_id)
        log.info("step 2/4: wav2lip")
        run_wav2lip(avatar, wav, raw_mp4)
        _verify_artifact(raw_mp4, "wav2lip raw mp4")
        post_progress(job_id, 70)

        _check_shutdown(job_id)
        log.info("step 3/4: gfpgan+mux (%s)",
                 "enabled" if GFPGAN_SCRIPT else "mux only")
        run_gfpgan(raw_mp4, final_mp4)
        _verify_artifact(final_mp4, "final mp4")
        post_progress(job_id, 95)

        _check_shutdown(job_id)
        log.info("step 4/4: commit (%s)", final_mp4)
        post_complete(job_id, str(final_mp4))
        log.info("job %s DONE (%s)", job_id, final_mp4)
        success = True

    finally:
        # Honour the comment: scratch dir stays on failure for debugging,
        # gets wiped on success so tmp artifacts don't accumulate forever.
        # Previously this block was `pass` -- every successful job leaked
        # the entire scratch tree. After ~1000 jobs you're out of disk.
        if success:
            shutil.rmtree(scratch, ignore_errors=True)
        else:
            log.warning("keeping scratch for post-mortem: %s", scratch)


def main() -> int:
    _install_signal_handlers()
    log.info("Elle-Ann video worker starting. api_base=%s poll=%.1fs",
             API_BASE, POLL_INTERVAL)

    # Sanity-check required tooling so we fail early, not mid-job.
    if not shutil.which(FFMPEG_BIN):
        log.error("ffmpeg not found on PATH (FFMPEG_BIN=%s)", FFMPEG_BIN)
        return 2
    if not WAV2LIP_CKPT or not Path(WAV2LIP_CKPT).exists():
        log.error("WAV2LIP_CHECKPOINT missing (%s) -- set the env var before running",
                  WAV2LIP_CKPT)
        return 2
    if not WAV2LIP_INFER or not Path(WAV2LIP_INFER).exists():
        log.error("WAV2LIP_INFER missing (%s)", WAV2LIP_INFER)
        return 2

    while not _shutdown.is_set():
        try:
            job = claim_job()
        except ClaimValidationError as ex:
            # Server is speaking the wrong protocol. Don't busy-loop --
            # back off aggressively so an operator can notice.
            log.error("protocol violation on claim: %s", ex)
            if _shutdown.wait(POLL_INTERVAL * 4):
                break
            continue
        except requests.RequestException as ex:
            log.warning("claim failed: %s (retrying in %.0fs)", ex, POLL_INTERVAL)
            if _shutdown.wait(POLL_INTERVAL):
                break
            continue

        if not job:
            if _shutdown.wait(POLL_INTERVAL):
                break
            continue

        try:
            process_job(job)
        except subprocess.CalledProcessError as ex:
            log.exception("subprocess failed on job %s: %s", job.get("job_id"), ex)
            post_fail(job["job_id"], f"subprocess rc={ex.returncode}: {ex}")
        except Exception as ex:  # noqa: BLE001 -- defensive boundary
            log.exception("job failed")
            post_fail(job.get("job_id", "?"), str(ex))

    log.info("graceful shutdown complete")
    return 0


if __name__ == "__main__":
    sys.exit(main() or 0)
