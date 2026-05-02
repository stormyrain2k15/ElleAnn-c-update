/*══════════════════════════════════════════════════════════════════════════════
 * ElleAnn_Sessions_Delta.sql — opaque bearer-token sessions.
 *
 *   Feb-2026 pivot v2: moved from ElleSystem to ElleCore.
 *
 *   Why the move?  The C++ SQL pool connects to ElleCore by default and
 *   writing cross-database to ElleSystem requires the pool's login to
 *   also have INSERT/SELECT/DELETE rights on ElleSystem.dbo.Sessions.
 *   That tripped "failed to create session" for operators who granted
 *   rights only on ElleCore.  Keeping Sessions local to ElleCore means
 *   the same login that wrote Memory rows writes Sessions rows, and
 *   login Just Works.
 *
 *   Token is 64 hex chars (32 cryptographically-random bytes).
 *   nAuthID is cached from tUser at session-creation time so the gate
 *   doesn't re-hit Account on every request; privilege changes take
 *   effect on the next logout/login.
 *   No expires_ms by design — invalidation is DELETE-only.
 *
 *   Run order (idempotent; safe to re-apply):
 *     1. ElleAnn_SchemaSync_FebPivot.sql
 *     2. ElleAnn_Schema.sql
 *     3. ElleAnn_System_Schema.sql
 *     4. ElleAnn_Sessions_Delta.sql            ← this file
 *
 *   Migration from v1 (if you already ran the ElleSystem version):
 *     - v2 will create ElleCore.dbo.Sessions (empty).
 *     - Any old tokens stored in ElleSystem.dbo.Sessions become
 *       orphaned; users will need to log in once. Low cost.
 *     - You can DROP ElleSystem.dbo.Sessions afterwards if you want a
 *       clean state; the code no longer reads from it.
 *══════════════════════════════════════════════════════════════════════════════*/
SET ANSI_NULLS ON;
SET QUOTED_IDENTIFIER ON;
GO

USE ElleCore;
GO

IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'Sessions' AND schema_id = SCHEMA_ID('dbo'))
BEGIN
    CREATE TABLE dbo.Sessions (
        Token       NVARCHAR(64)   NOT NULL PRIMARY KEY,
        nUserNo     BIGINT         NOT NULL,
        sUserID     NVARCHAR(30)   NOT NULL,
        sUserName   NVARCHAR(60)   NULL,
        nAuthID     INT            NOT NULL DEFAULT 0,
        CreatedMs   BIGINT         NOT NULL,
        LastSeenMs  BIGINT         NOT NULL,
        DeviceName  NVARCHAR(128)  NULL,
        PeerAddr    NVARCHAR(64)   NULL
    );
    CREATE INDEX IX_Sessions_nUserNo ON dbo.Sessions(nUserNo);
    PRINT 'ElleCore.dbo.Sessions created';
END
ELSE
BEGIN
    PRINT 'ElleCore.dbo.Sessions already exists';
END
GO

PRINT 'ElleAnn_Sessions_Delta.sql applied OK';
GO
