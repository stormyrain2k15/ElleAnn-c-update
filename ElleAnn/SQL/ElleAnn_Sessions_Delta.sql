/*══════════════════════════════════════════════════════════════════════════════
 * ElleAnn_Sessions_Delta.sql — opaque bearer-token sessions (Feb 2026 pivot).
 *
 *   Replaces the HS256-JWT auth that preceded it.  Per user directive
 *   Feb 2026: "username and password login, no expire, enough of the
 *   unnecessary security."  Sessions live in ElleSystem.dbo.Sessions
 *   and have NO expires_ms column — a session is only invalidated by
 *   explicit DELETE (logout) or by flipping tUser.bIsBlock server-side
 *   (checked at every auth-gate lookup).
 *
 *   Design:
 *     - Token is 64 hex chars (32 cryptographically-random bytes).
 *     - nAuthID is cached from tUser at session-creation time so we
 *       don't re-hit the Account DB on every request; on privilege
 *       change the user logs out and back in.
 *     - LastSeenMs is best-effort (written on every auth'd request)
 *       so the dev panel can show "idle devices" without a separate
 *       heartbeat.  No index on it by design — ordering is by Token
 *       PK for the common case (lookup) and that's already covered.
 *
 *   Run order:
 *     1. ElleAnn_SchemaSync_FebPivot.sql
 *     2. ElleAnn_Schema.sql
 *     3. ElleAnn_System_Schema.sql
 *     4. ElleAnn_Sessions_Delta.sql            ← this file
 *══════════════════════════════════════════════════════════════════════════════*/
SET ANSI_NULLS ON;
SET QUOTED_IDENTIFIER ON;
GO

USE ElleSystem;
GO

IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'Sessions')
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
END
GO

PRINT 'ElleAnn_Sessions_Delta.sql applied OK';
GO
