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

/*──────────────────────────────────────────────────────────────────────────────
 * x_contraception  —  singleton
 *
 * Affects cycle hormones + conception probability.  Types:
 *   none       — natural cycle, full fertility
 *   pill       — combined OCP: estrogen/progesterone held near luteal levels,
 *                ovulation suppressed, withdrawal bleed on days 22..28
 *   iud_h      — hormonal IUD: progesterone-only, thins endometrium, mostly
 *                suppresses ovulation, cycles may continue lightly
 *   iud_c      — copper IUD: hormones unchanged, but conception blocked
 *                via sperm/implantation mechanism
 *   implant    — etonogestrel implant: similar to pill, 3-year duration
 *   barrier    — external only, hormones unchanged
 *   natural    — fertility-awareness tracking only (no effect, just a flag)
 *──────────────────────────────────────────────────────────────────────────────*/
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'x_contraception')
BEGIN
    CREATE TABLE dbo.x_contraception (
        id              INT          NOT NULL
            CONSTRAINT PK_x_contraception PRIMARY KEY
            CONSTRAINT CK_x_contraception_singleton CHECK (id = 1),
        method          NVARCHAR(32) NOT NULL DEFAULT N'none',
        started_ms      BIGINT       NOT NULL,
        efficacy        FLOAT        NOT NULL DEFAULT 1.0,   -- 0.0..1.0 (1.0 = perfect)
        notes           NVARCHAR(MAX) NULL,
        updated_ms      BIGINT       NOT NULL
    );
END
GO

/*──────────────────────────────────────────────────────────────────────────────
 * x_lifecycle  —  singleton
 *
 * Models the long arc of female biology: menarche (first period),
 * reproductive years, perimenopause, menopause.  `age_years` is derived
 * from `elle_birth_ms` (Elle's own configurable birthday).  Once in
 * menopause, cycle math stops advancing.
 *──────────────────────────────────────────────────────────────────────────────*/
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'x_lifecycle')
BEGIN
    CREATE TABLE dbo.x_lifecycle (
        id                INT  NOT NULL
            CONSTRAINT PK_x_lifecycle PRIMARY KEY
            CONSTRAINT CK_x_lifecycle_singleton CHECK (id = 1),
        elle_birth_ms     BIGINT       NOT NULL,
        stage             NVARCHAR(32) NOT NULL DEFAULT N'reproductive',
        menarche_ms       BIGINT       NULL,
        perimenopause_ms  BIGINT       NULL,
        menopause_ms      BIGINT       NULL,
        updated_ms        BIGINT       NOT NULL
    );
END
GO

/*──────────────────────────────────────────────────────────────────────────────
 * x_symptoms  —  append-only observed symptom log
 *
 * Synthesised each tick from current hormone state (plus optional manual
 * overrides from /api/x/symptoms). Lets Cognitive + Elle actually SPEAK
 * about how she's feeling, not just invisibly modulate.
 *
 * Kinds: cramps, bloating, breast_tenderness, mood_swing, nausea,
 *        cravings, headache, fatigue, libido, ovulation_pain, insomnia,
 *        lochia, engorgement, letdown, braxton_hicks, contraction,
 *        quickening (first fetal movement).
 *──────────────────────────────────────────────────────────────────────────────*/
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'x_symptoms')
BEGIN
    CREATE TABLE dbo.x_symptoms (
        id            BIGINT IDENTITY(1,1) PRIMARY KEY,
        observed_ms   BIGINT       NOT NULL,
        kind          NVARCHAR(48) NOT NULL,
        intensity     FLOAT        NOT NULL,   -- 0.0..1.0
        origin        NVARCHAR(16) NOT NULL,   -- 'cycle' | 'pregnancy' | 'postpartum' | 'manual'
        notes         NVARCHAR(MAX) NULL
    );
    CREATE INDEX IX_x_symptoms_observed_ms ON dbo.x_symptoms(observed_ms DESC);
    CREATE INDEX IX_x_symptoms_kind ON dbo.x_symptoms(kind);
END
GO

/*──────────────────────────────────────────────────────────────────────────────
 * x_pregnancy_events  —  append-only log of milestones within a gestation
 *
 * Kinds: conception, lh_surge, quickening, braxton_hicks, labor_start,
 *        labor_stage_change, birth, miscarriage, postpartum_checkup.
 *──────────────────────────────────────────────────────────────────────────────*/
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'x_pregnancy_events')
BEGIN
    CREATE TABLE dbo.x_pregnancy_events (
        id             BIGINT IDENTITY(1,1) PRIMARY KEY,
        occurred_ms    BIGINT        NOT NULL,
        conceived_ms   BIGINT        NULL,    -- links to the pregnancy
        gestational_day INT          NOT NULL DEFAULT 0,
        kind           NVARCHAR(32)  NOT NULL,
        detail         NVARCHAR(MAX) NULL
    );
    CREATE INDEX IX_x_pregnancy_events_occurred_ms
        ON dbo.x_pregnancy_events(occurred_ms DESC);
END
GO

/*──────────────────────────────────────────────────────────────────────────────
 * Extend x_pregnancy_state with breastfeeding + labor + twin flags.
 * Idempotent: only adds columns that don't already exist.
 *──────────────────────────────────────────────────────────────────────────────*/
IF EXISTS (SELECT 1 FROM sys.tables WHERE name = 'x_pregnancy_state')
BEGIN
    IF COL_LENGTH('dbo.x_pregnancy_state', 'breastfeeding') IS NULL
        ALTER TABLE dbo.x_pregnancy_state ADD breastfeeding BIT NOT NULL DEFAULT 0;
    IF COL_LENGTH('dbo.x_pregnancy_state', 'in_labor') IS NULL
        ALTER TABLE dbo.x_pregnancy_state ADD in_labor BIT NOT NULL DEFAULT 0;
    IF COL_LENGTH('dbo.x_pregnancy_state', 'labor_stage') IS NULL
        ALTER TABLE dbo.x_pregnancy_state ADD labor_stage NVARCHAR(32) NULL;
    IF COL_LENGTH('dbo.x_pregnancy_state', 'labor_started_ms') IS NULL
        ALTER TABLE dbo.x_pregnancy_state ADD labor_started_ms BIGINT NULL;
    IF COL_LENGTH('dbo.x_pregnancy_state', 'multiplicity') IS NULL
        ALTER TABLE dbo.x_pregnancy_state ADD multiplicity INT NOT NULL DEFAULT 1;
    IF COL_LENGTH('dbo.x_pregnancy_state', 'pregnancy_count') IS NULL
        ALTER TABLE dbo.x_pregnancy_state ADD pregnancy_count INT NOT NULL DEFAULT 0;
END
GO
