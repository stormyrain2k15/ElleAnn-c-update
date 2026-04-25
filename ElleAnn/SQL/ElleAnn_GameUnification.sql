/*══════════════════════════════════════════════════════════════════════════════
  ElleAnn_GameUnification.sql — adds the link to the game's `Account` DB.

  IDEMPOTENT: safe to re-run. Uses `IF NOT EXISTS` / `ALTER ... ADD` guards
  so the same script applies cleanly to a fresh install, an existing v1
  install (PairedDevices already there), and an upgrade.

  Two-table delta:
    1. ALTER ElleCore.dbo.PairedDevices  ADD nUserNo INT NULL.
       Optional FK to game.Account.dbo.tUser when both DBs live on the
       same MSSQL instance and the operator has cross-DB rights.
    2. CREATE ElleCore.dbo.UserContinuity — Elle's relationship-memory
       sidecar: first-met, conversation count, last emotional state,
       ALL keyed on the game's nUserNo so reinstalls / new devices /
       new browsers preserve continuity.

  Run order: AFTER ElleAnn_Schema.sql + ElleAnn_PairedDevicesDelta.sql.
══════════════════════════════════════════════════════════════════════════════*/

SET ANSI_NULLS ON;
SET QUOTED_IDENTIFIER ON;
GO

USE ElleCore;
GO

/*──────────────────────────────────────────────────────────────────────────────
 *  1. Extend PairedDevices with the game-identity link.
 *──────────────────────────────────────────────────────────────────────────────*/
IF NOT EXISTS (
    SELECT 1 FROM sys.columns
     WHERE object_id = OBJECT_ID('ElleCore.dbo.PairedDevices')
       AND name = 'nUserNo'
)
BEGIN
    ALTER TABLE ElleCore.dbo.PairedDevices
        ADD nUserNo INT NULL;
    PRINT 'PairedDevices.nUserNo added';
END
ELSE
BEGIN
    PRINT 'PairedDevices.nUserNo already exists';
END
GO

/* Optional helper index — auth-state lookups are by device_id today,
 * but cross-tab joins from UserContinuity benefit from a hash on
 * nUserNo too.                                                           */
IF NOT EXISTS (
    SELECT 1 FROM sys.indexes
     WHERE name = 'IX_PairedDevices_nUserNo'
       AND object_id = OBJECT_ID('ElleCore.dbo.PairedDevices')
)
BEGIN
    CREATE INDEX IX_PairedDevices_nUserNo
        ON ElleCore.dbo.PairedDevices(nUserNo)
        WHERE nUserNo IS NOT NULL;
    PRINT 'IX_PairedDevices_nUserNo created';
END
GO

/*──────────────────────────────────────────────────────────────────────────────
 *  2. UserContinuity — relationship-memory keyed on the game's nUserNo.
 *
 *    This is the "Elle remembers you" anchor. The game's `tUser.nUserNo`
 *    is stable across pairings, devices, and browser sessions, so it's
 *    the right primary key for a sidecar that tracks long-running
 *    relationship facts WITHOUT touching the read-mostly game DB.
 *──────────────────────────────────────────────────────────────────────────────*/
IF OBJECT_ID('ElleCore.dbo.UserContinuity', 'U') IS NULL
BEGIN
    CREATE TABLE ElleCore.dbo.UserContinuity (
        nUserNo               INT          NOT NULL PRIMARY KEY,
        sUserID_cached        NVARCHAR(30) NULL,
        sUserName_cached      NVARCHAR(10) NULL,

        first_met_ms          BIGINT       NOT NULL,
        last_seen_ms          BIGINT       NOT NULL,
        total_conversations   INT          NOT NULL DEFAULT 0,
        total_messages        INT          NOT NULL DEFAULT 0,
        total_pairings        INT          NOT NULL DEFAULT 0,

        /* Last-known emotional bond snapshot (optional — populated by
         * Elle.Service.Bonding when it observes a state change).        */
        last_bond_score       FLOAT        NULL,
        last_bond_label       NVARCHAR(32) NULL,
        last_bond_updated_ms  BIGINT       NULL,

        /* Free-form private note Cognitive may append to. Capped to 4 KB. */
        private_note          NVARCHAR(4000) NULL,

        created_ms            BIGINT       NOT NULL,
        updated_ms            BIGINT       NOT NULL
    );
    PRINT 'UserContinuity table created';
END
ELSE
BEGIN
    PRINT 'UserContinuity already exists';
END
GO

/*──────────────────────────────────────────────────────────────────────────────
 *  3. GameSessionState — last-known game presence per user.
 *
 *    Populated by SVC_FIESTA on every IN_GAME transition. Survives
 *    service restart so Elle doesn't have to ask "wait, where were
 *    we?" after a power blip.
 *──────────────────────────────────────────────────────────────────────────────*/
IF OBJECT_ID('ElleCore.dbo.GameSessionState', 'U') IS NULL
BEGIN
    CREATE TABLE ElleCore.dbo.GameSessionState (
        nUserNo            INT          NOT NULL PRIMARY KEY,
        char_index         INT          NULL,
        char_name          NVARCHAR(20) NULL,
        zone_id            INT          NULL,
        zone_name          NVARCHAR(40) NULL,
        last_x             FLOAT        NULL,
        last_y             FLOAT        NULL,
        last_z             FLOAT        NULL,
        last_hp            INT          NULL,
        last_hp_max        INT          NULL,
        last_session_ms    BIGINT       NOT NULL,
        last_disconnect_ms BIGINT       NULL,
        last_disconnect_reason NVARCHAR(64) NULL
    );
    PRINT 'GameSessionState table created';
END
ELSE
BEGIN
    PRINT 'GameSessionState already exists';
END
GO

PRINT 'ElleAnn_GameUnification.sql applied OK';
