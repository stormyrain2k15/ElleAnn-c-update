import { Page, ErrorBanner, Spinner, Card, Stat, useApi } from "@/components/page-shell";

export default function Dashboard() {
  const me     = useApi("/api/me");
  const status = useApi("/api/status");
  const llm    = useApi("/api/ai/status");
  const queues = useApi("/api/diag/queues");

  return (
    <Page
      title="Dashboard"
      subtitle="Live read of who's authenticated, services up, and queue depth."
    >
      <ErrorBanner error={me.error}     onRetry={me.reload} />
      <ErrorBanner error={status.error} onRetry={status.reload} />

      {/* Identity */}
      <Card title="Authenticated user" className="mb-4">
        {me.loading ? (
          <Spinner />
        ) : me.data ? (
          <div className="grid grid-cols-2 gap-4 text-sm" data-testid="me-block">
            <div><span className="text-zinc-500">user_id</span> <span className="ml-2 font-mono">{me.data.user_id}</span></div>
            <div><span className="text-zinc-500">username</span> <span className="ml-2 font-mono">{me.data.username || "—"}</span></div>
            <div className="col-span-2"><span className="text-zinc-500">device</span> <span className="ml-2 font-mono text-xs break-all">{me.data.device_id}</span></div>
            <div className="col-span-2"><span className="text-zinc-500">source</span> <span className="ml-2 font-mono text-xs">{me.data.authoritative_source}</span></div>
          </div>
        ) : (
          <div className="text-xs text-zinc-500">no identity bound to this session</div>
        )}
      </Card>

      {/* Status grid */}
      <div className="grid grid-cols-4 gap-3 mb-4" data-testid="status-grid">
        <Stat
          label="HTTP Service"
          value={status.data ? "Up" : (status.loading ? "…" : "Down")}
          tone={status.data ? "good" : (status.loading ? "default" : "bad")}
        />
        <Stat
          label="LLM Provider"
          value={llm.data?.provider || (llm.loading ? "…" : "—")}
          hint={llm.data?.model || ""}
          tone={llm.data?.healthy ? "good" : (llm.loading ? "default" : "warn")}
        />
        <Stat
          label="Intent Queue"
          value={queues.data?.intent?.pending ?? (queues.loading ? "…" : "?")}
          hint={`processing: ${queues.data?.intent?.processing ?? "?"}`}
          tone="default"
        />
        <Stat
          label="Action Queue"
          value={queues.data?.action?.pending ?? (queues.loading ? "…" : "?")}
          hint={`processing: ${queues.data?.action?.processing ?? "?"}`}
          tone="default"
        />
      </div>

      {/* Raw status JSON for transparency */}
      <Card title="Raw /api/status">
        {status.loading ? <Spinner /> : (
          <pre className="text-xs font-mono text-zinc-400 overflow-x-auto" data-testid="status-raw">
{JSON.stringify(status.data, null, 2)}
          </pre>
        )}
      </Card>
    </Page>
  );
}
