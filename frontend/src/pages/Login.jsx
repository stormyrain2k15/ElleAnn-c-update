import { useState } from "react";
import { useNavigate } from "react-router-dom";
import { auth, BACKEND } from "@/lib/api";
import { LogIn, AlertCircle } from "lucide-react";

export default function Login() {
  const [u, setU] = useState("");
  const [p, setP] = useState("");
  const [err, setErr] = useState("");
  const [busy, setBusy] = useState(false);
  const nav = useNavigate();

  const submit = async (e) => {
    e.preventDefault();
    setErr("");
    setBusy(true);
    const r = await auth.login(u, p);
    setBusy(false);
    if (r.ok) { nav("/"); return; }
    setErr(r.error || "login failed");
  };

  return (
    <div className="min-h-screen bg-zinc-950 text-zinc-100 grid place-items-center px-6">
      <form
        onSubmit={submit}
        className="w-full max-w-sm rounded-lg border border-zinc-800 bg-zinc-900/60 backdrop-blur p-6 space-y-4"
        data-testid="login-form"
      >
        <div>
          <div className="text-xs uppercase tracking-[0.25em] text-zinc-500">Elle-Ann</div>
          <h1 className="text-xl font-semibold mt-1">Sign in</h1>
          <p className="text-xs text-zinc-500 mt-1">
            Uses your game account credentials (Account.dbo.tUser).
          </p>
        </div>

        <div className="space-y-1.5">
          <label className="text-xs text-zinc-400">Username</label>
          <input
            data-testid="login-username-input"
            value={u}
            onChange={(e) => setU(e.target.value)}
            autoFocus
            className="w-full rounded-md bg-zinc-950 border border-zinc-800 focus:border-emerald-500/40 focus:outline-none px-3 py-2 text-sm"
          />
        </div>

        <div className="space-y-1.5">
          <label className="text-xs text-zinc-400">Password</label>
          <input
            data-testid="login-password-input"
            type="password"
            value={p}
            onChange={(e) => setP(e.target.value)}
            className="w-full rounded-md bg-zinc-950 border border-zinc-800 focus:border-emerald-500/40 focus:outline-none px-3 py-2 text-sm"
          />
        </div>

        {err && (
          <div className="rounded-md border border-rose-500/30 bg-rose-500/5 px-3 py-2 text-xs text-rose-200 flex items-start gap-2" data-testid="login-error">
            <AlertCircle className="h-3.5 w-3.5 mt-0.5 shrink-0" />
            <span className="break-all">{err}</span>
          </div>
        )}

        <button
          type="submit"
          disabled={busy || !u || !p}
          data-testid="login-submit-button"
          className="w-full rounded-md bg-emerald-500/90 hover:bg-emerald-400 disabled:opacity-50 disabled:cursor-not-allowed text-zinc-950 font-medium px-3 py-2 text-sm flex items-center justify-center gap-2"
        >
          <LogIn className="h-4 w-4" />
          {busy ? "Signing in…" : "Sign in"}
        </button>

        <div className="text-[10px] text-zinc-600 text-center" data-testid="backend-target">
          backend: {BACKEND || "(same origin)"}
        </div>
      </form>
    </div>
  );
}
