/*******************************************************************************
 * ElleAnn_QueueReaperDelta.sql — Queue Timeout-Reaper Correctness Delta
 *
 * Adds a ProcessingMs column to ElleCore.dbo.IntentQueue. Previously the
 * timeout reaper measured stale intents as `(now - CreatedMs) > TimeoutMs`,
 * which was wrong on two counts:
 *   1. A row that legitimately sat PENDING for longer than TimeoutMs would
 *      be reaped the instant it transitioned to PROCESSING, even though the
 *      consumer never had a chance to work on it.
 *   2. A row that completed legitimately right at the deadline could be
 *      reaped concurrently and dispatched twice.
 *
 * With ProcessingMs in place, the reaper measures
 * `(now - ISNULL(ProcessingMs, CreatedMs)) > TimeoutMs`, which is the
 * actual consumer deadline.
 *
 * ActionQueue already has StartedMs serving the same purpose.
 *
 * Safe to run multiple times — uses IF NOT EXISTS guards.
 ******************************************************************************/
USE ElleCore;
GO

IF NOT EXISTS (
    SELECT 1 FROM sys.columns
    WHERE object_id = OBJECT_ID(N'dbo.IntentQueue')
      AND name = 'ProcessingMs'
)
BEGIN
    ALTER TABLE dbo.IntentQueue ADD ProcessingMs BIGINT NULL;
END
GO

/* Helper index for the reaper's range scan on stale PROCESSING rows. */
IF NOT EXISTS (
    SELECT 1 FROM sys.indexes
    WHERE name = 'IX_IntentQueue_Processing'
      AND object_id = OBJECT_ID(N'dbo.IntentQueue')
)
BEGIN
    CREATE INDEX IX_IntentQueue_Processing
        ON dbo.IntentQueue (Status, ProcessingMs);
END
GO
