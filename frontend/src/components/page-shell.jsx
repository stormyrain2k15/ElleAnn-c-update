/*══════════════════════════════════════════════════════════════════════════════
 * page-shell.jsx — common page chrome: title, subtitle, error banner,
 * empty-state, simple `useApi(path)` hook so every page handles 500s
 * uniformly instead of breaking on a missing field.
 *══════════════════════════════════════════════════════════════════════════════*/
import { useEffect, useState, useCallback } from "react";
import { AlertCircle, RefreshCw, Loader2 } from "lucide-react";
import { api } from "@/lib/api";

export function Page({ title, subtitle, children, actions }) {
  return (
    <div className="px-8 py-6 max-w-7xl">
      <header className="flex items-start justify-between mb-6 pb-4 border-b border-zinc-800">
        <div>
          <h1 className="text-2xl font-semibold tracking-tight text-zinc-100">
            {title}
          </h1>
          {subtitle && (
            <p className="text-sm text-zinc-500 mt-1">{subtitle}</p>
          )}
        </div>
        {actions && <div className="flex gap-2">{actions}</div>}
      </header>
      <div>{children}</div>
    </div>
  );
}

export function ErrorBanner({ error, onRetry }) {
  if (!error) return null;
  return (
    <div
      className="rounded-md border border-rose-500/30 bg-rose-500/5 px-4 py-3 mb-4 flex items-start gap-3"
      data-testid="error-banner"
    >
      <AlertCircle className="h-4 w-4 text-rose-400 mt-0.5 shrink-0" />
      <div className="flex-1 text-sm text-rose-200">
        <div className="font-medium text-rose-100">Backend error</div>
        <div className="text-rose-300/80 font-mono text-xs mt-1 break-all">
          {error}
        </div>
      </div>
      {onRetry && (
        <button
          onClick={onRetry}
          data-testid="error-retry-button"
          className="text-xs px-2.5 py-1 rounded border border-rose-500/30 hover:bg-rose-500/10 text-rose-200 flex items-center gap-1"
        >
          <RefreshCw className="h-3 w-3" /> Retry
        </button>
      )}
    </div>
  );
}

export function EmptyState({ title, hint }) {
  return (
    <div
      className="rounded-md border border-zinc-800 bg-zinc-900/40 px-6 py-10 text-center"
      data-testid="empty-state"
    >
      <div className="text-sm font-medium text-zinc-300">{title}</div>
      {hint && <div className="text-xs text-zinc-500 mt-1.5">{hint}</div>}
    </div>
  );
}

export function Spinner() {
  return (
    <div className="flex items-center gap-2 text-sm text-zinc-500" data-testid="spinner">
      <Loader2 className="h-4 w-4 animate-spin" /> Loading…
    </div>
  );
}

/* useApi — fetches a path on mount, exposes { data, error, loading, reload }. */
export function useApi(path, deps = []) {
  const [state, set] = useState({ data: null, error: null, loading: true });
  const reload = useCallback(async () => {
    set((s) => ({ ...s, loading: true }));
    const r = await api.get(path);
    set({ data: r.ok ? r.data : null, error: r.ok ? null : r.error, loading: false });
  // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [path, ...deps]);

  useEffect(() => { reload(); }, [reload]);
  return { ...state, reload };
}

/* Card — small panel with optional title. */
export function Card({ title, children, className = "" }) {
  return (
    <div className={`rounded-md border border-zinc-800 bg-zinc-900/40 ${className}`}>
      {title && (
        <div className="px-4 py-2.5 border-b border-zinc-800 text-xs font-medium uppercase tracking-wider text-zinc-400">
          {title}
        </div>
      )}
      <div className="p-4">{children}</div>
    </div>
  );
}

/* Stat — numeric display block. */
export function Stat({ label, value, hint, tone = "default" }) {
  const toneCls = {
    default: "text-zinc-100",
    good: "text-emerald-300",
    warn: "text-amber-300",
    bad: "text-rose-300",
  }[tone] || "text-zinc-100";
  return (
    <div className="rounded-md border border-zinc-800 bg-zinc-900/40 px-4 py-3">
      <div className="text-[11px] uppercase tracking-wider text-zinc-500">{label}</div>
      <div className={`text-2xl font-semibold mt-1 ${toneCls}`}>{value}</div>
      {hint && <div className="text-xs text-zinc-500 mt-1">{hint}</div>}
    </div>
  );
}
