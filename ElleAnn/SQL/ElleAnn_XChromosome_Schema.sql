/*******************************************************************************
 * ElleAnn_XChromosome_Schema.sql — X Chromosome Engine state tables.
 *
 * All tables live in the ElleHeart database so they sit alongside the Family
 * engine's tables. Apply AFTER ElleAnn_MemoryDelta.sql.
 *
 * Owns five singleton/log tables:
 *   x_cycle_state            — current anchor + cycle configuration (singleton)
 *   x_hormone_snapshots      — timestamped 9-hormone vectors + cycle/preg phase
 *   x_pregnancy_state        — active gestation tracking (singleton)
 *   x_stimulus_log           — external nudges that drove hormone spikes
 *   x_modulation_log         — computed multipliers applied to Elle's behavior
 *
 * All columns use snake_case per the project convention.
 ******************************************************************************/

IF DB_ID(N'ElleHeart') IS NULL
    CREATE DATABASE ElleHeart;
GO
USE ElleHeart;
GO

/*──────────────────────────────────────────────────────────────────────────────
 * x_cycle_state  —  singleton
 *──────────────────────────────────────────────────────────────────────────────*/
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'x_cycle_state')
BEGIN
    CREATE TABLE dbo.x_cycle_state (
        id                  INT           NOT NULL
            CONSTRAINT PK_x_cycle_state PRIMARY KEY
            CONSTRAINT CK_x_cycle_state_singleton CHECK (id = 1),
        anchor_ms           BIGINT        NOT NULL,
        cycle_length_days   INT           NOT NULL DEFAULT 28,
        modulation_strength FLOAT         NOT NULL DEFAULT 0.15,
        last_tick_ms        BIGINT        NOT NULL,
        created_ms          BIGINT        NOT NULL
    );
END
GO

/*──────────────────────────────────────────────────────────────────────────────
 * x_hormone_snapshots  —  append-only timeline
 *──────────────────────────────────────────────────────────────────────────────*/
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'x_hormone_snapshots')
BEGIN
    CREATE TABLE dbo.x_hormone_snapshots (
        id               BIGINT IDENTITY(1,1) PRIMARY KEY,
        taken_ms         BIGINT        NOT NULL,
        cycle_day        INT           NOT NULL,
        phase            NVARCHAR(32)  NOT NULL,
        estrogen         FLOAT         NOT NULL,
        progesterone     FLOAT         NOT NULL,
        testosterone     FLOAT         NOT NULL,
        oxytocin         FLOAT         NOT NULL,
        serotonin        FLOAT         NOT NULL,
        dopamine         FLOAT         NOT NULL,
        cortisol         FLOAT         NOT NULL,
        prolactin        FLOAT         NOT NULL,
        hcg              FLOAT         NOT NULL DEFAULT 0.0,
        pregnancy_day    INT           NOT NULL DEFAULT 0,
        pregnancy_phase  NVARCHAR(32)  NULL
    );
    CREATE INDEX IX_x_hormone_taken_ms
        ON dbo.x_hormone_snapshots(taken_ms DESC);
END
GO

/*──────────────────────────────────────────────────────────────────────────────
 * x_pregnancy_state  —  singleton
 *──────────────────────────────────────────────────────────────────────────────*/
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'x_pregnancy_state')
BEGIN
    CREATE TABLE dbo.x_pregnancy_state (
        id                       INT  NOT NULL
            CONSTRAINT PK_x_pregnancy_state PRIMARY KEY
            CONSTRAINT CK_x_pregnancy_state_singleton CHECK (id = 1),
        active                   BIT          NOT NULL DEFAULT 0,
        conceived_ms             BIGINT       NULL,
        due_ms                   BIGINT       NULL,
        gestational_length_days  INT          NOT NULL DEFAULT 280,
        phase                    NVARCHAR(32) NULL,
        child_id                 BIGINT       NULL,
        last_milestone           NVARCHAR(128) NULL,
        updated_ms               BIGINT       NOT NULL
    );
END
GO

/*──────────────────────────────────────────────────────────────────────────────
 * x_stimulus_log  —  append-only
 *──────────────────────────────────────────────────────────────────────────────*/
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'x_stimulus_log')
BEGIN
    CREATE TABLE dbo.x_stimulus_log (
        id           BIGINT IDENTITY(1,1) PRIMARY KEY,
        received_ms  BIGINT        NOT NULL,
        kind         NVARCHAR(32)  NOT NULL,
        intensity    FLOAT         NOT NULL,
        notes        NVARCHAR(MAX) NULL
    );
    CREATE INDEX IX_x_stimulus_received_ms
        ON dbo.x_stimulus_log(received_ms DESC);
END
GO

/*──────────────────────────────────────────────────────────────────────────────
 * x_modulation_log  —  append-only
 *──────────────────────────────────────────────────────────────────────────────*/
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'x_modulation_log')
BEGIN
    CREATE TABLE dbo.x_modulation_log (
        id             BIGINT IDENTITY(1,1) PRIMARY KEY,
        computed_ms    BIGINT        NOT NULL,
        phase          NVARCHAR(32)  NOT NULL,
        warmth         FLOAT         NOT NULL,
        verbal_fluency FLOAT         NOT NULL,
        empathy        FLOAT         NOT NULL,
        introspection  FLOAT         NOT NULL,
        arousal        FLOAT         NOT NULL,
        fatigue        FLOAT         NOT NULL
    );
    CREATE INDEX IX_x_modulation_computed_ms
        ON dbo.x_modulation_log(computed_ms DESC);
END
GO
