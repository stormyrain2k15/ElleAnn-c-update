/*══════════════════════════════════════════════════════════════════════════════
 * ElleAnn_System_Schema.sql — operational tables (Feb 2026 pivot).
 *
 *   Pre-pivot these tables were referenced by the C++ code (Workers,
 *   Logs, Analytics, TrustState, TrustAudit) but never declared in any
 *   schema file. The first write fired and either silently auto-built
 *   them (if the SQL login had DDL rights) or returned a missing-table
 *   error that was logged and swallowed — which is why
 *   /api/diag/heartbeats came up empty on every fresh install and
 *   `Logs` never had anything actionable when you opened it.
 *
 *   This file declares them as the canonical source of truth.  Every
 *   table is keyed sensibly (ServiceId for Workers, TimestampMs DESC
 *   index for Logs) so the dev-panel diag endpoints stay snappy as the
 *   tables grow.
 *
 *   Run order (slot in AFTER ElleAnn_Schema.sql, BEFORE the deltas):
 *     1. ElleAnn_SchemaSync_FebPivot.sql
 *     2. ElleAnn_Schema.sql
 *     3. ElleAnn_System_Schema.sql              ← this file
 *     4. ElleAnn_PairedDevicesDelta.sql ...etc
 *══════════════════════════════════════════════════════════════════════════════*/
SET ANSI_NULLS ON;
SET QUOTED_IDENTIFIER ON;
GO

IF NOT EXISTS (SELECT name FROM sys.databases WHERE name = 'ElleSystem')
    CREATE DATABASE ElleSystem;
GO

USE ElleSystem;
GO

/*════════════════════════════════════════════════════════════════════════════
 *  Workers — per-service heartbeat / health.
 *
 *  Written by ElleServiceBase's heartbeat thread (ElleDB_Domain.cpp
 *  UpdateWorkerHeartbeat). Read by GET /api/diag/heartbeats.
 *
 *  ServiceId is the ELLE_SERVICE_ID enum value (see Shared/ElleTypes.h),
 *  not a foreign key — services are static and the enum is canonical.
 *══════════════════════════════════════════════════════════════════════════*/
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'Workers')
BEGIN
    CREATE TABLE dbo.Workers (
        ServiceId           INT     NOT NULL PRIMARY KEY,
        ServiceName         NVARCHAR(64) NULL,
        LastHeartbeatMs     BIGINT  NOT NULL DEFAULT 0,
        Healthy             BIT     NOT NULL DEFAULT 0,
        StartedMs           BIGINT  NOT NULL DEFAULT 0,
        Pid                 INT     NULL,
        HostName            NVARCHAR(128) NULL
    );
    CREATE INDEX IX_Workers_LastHeartbeat
        ON dbo.Workers(LastHeartbeatMs DESC);
END
GO

/*════════════════════════════════════════════════════════════════════════════
 *  Logs — structured service log.
 *══════════════════════════════════════════════════════════════════════════*/
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'Logs')
BEGIN
    CREATE TABLE dbo.Logs (
        Id                  BIGINT  IDENTITY(1,1) PRIMARY KEY,
        Level               INT     NOT NULL,           -- 0=DEBUG..4=ERROR
        ServiceId           INT     NOT NULL,
        Message             NVARCHAR(MAX) NOT NULL,
        TimestampMs         BIGINT  NOT NULL,
        CorrelationId       NVARCHAR(64) NULL
    );
    CREATE INDEX IX_Logs_TimestampMs ON dbo.Logs(TimestampMs DESC);
    CREATE INDEX IX_Logs_Service     ON dbo.Logs(ServiceId, TimestampMs DESC);
    CREATE INDEX IX_Logs_Level       ON dbo.Logs(Level, TimestampMs DESC);
END
GO

/*════════════════════════════════════════════════════════════════════════════
 *  Analytics — operational metrics (request counts, p95 latencies).
 *
 *  Written by various services on a 60s aggregation tick.  Bucket is
 *  e.g. 'http.request.count', 'llm.latency_ms.p95', 'memory.recall.count'.
 *══════════════════════════════════════════════════════════════════════════*/
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'Analytics')
BEGIN
    CREATE TABLE dbo.Analytics (
        Id                  BIGINT  IDENTITY(1,1) PRIMARY KEY,
        Bucket              NVARCHAR(128) NOT NULL,
        Value               FLOAT   NOT NULL,
        Tags                NVARCHAR(512) NULL,         -- "service=cognitive,user=12345"
        TimestampMs         BIGINT  NOT NULL
    );
    CREATE INDEX IX_Analytics_Bucket_Time
        ON dbo.Analytics(Bucket, TimestampMs DESC);
END
GO

/*════════════════════════════════════════════════════════════════════════════
 *  TrustState / TrustAudit — runtime trust scoring per user.
 *
 *  Written by Cognitive when an action's trust threshold gates execution.
 *  TrustState is the live row; TrustAudit is the immutable history.
 *══════════════════════════════════════════════════════════════════════════*/
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'TrustState')
BEGIN
    CREATE TABLE dbo.TrustState (
        UserId              INT     NOT NULL PRIMARY KEY,
        TrustScore          FLOAT   NOT NULL DEFAULT 0.5,
        LastUpdatedMs       BIGINT  NOT NULL,
        ActionCount         BIGINT  NOT NULL DEFAULT 0
    );
END
GO

IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'TrustAudit')
BEGIN
    CREATE TABLE dbo.TrustAudit (
        Id                  BIGINT  IDENTITY(1,1) PRIMARY KEY,
        UserId              INT     NOT NULL,
        Delta               FLOAT   NOT NULL,
        Reason              NVARCHAR(256) NULL,
        ActionType          NVARCHAR(64)  NULL,
        TimestampMs         BIGINT  NOT NULL
    );
    CREATE INDEX IX_TrustAudit_User_Time
        ON dbo.TrustAudit(UserId, TimestampMs DESC);
END
GO

PRINT 'ElleAnn_System_Schema.sql applied OK';
GO
