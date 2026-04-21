-- =============================================================================
-- ElleAnn_MemoryDelta.sql
--
-- Adds the missing memory/entity tables that the C++ Cognitive pipeline needs,
-- using the SAME snake_case / lowercase style the live ElleCore DB already uses.
-- Safe to run multiple times (IF NOT EXISTS guards).
--
-- HOW TO RUN:
--   1. Open SQL Server Management Studio (SSMS)
--   2. Connect to your Elle-Ann SQL instance
--   3. File → Open → this script
--   4. Ensure the top of the editor says  "ElleCore"  (Ctrl+U dropdown)
--   5. F5 to execute
-- =============================================================================
USE [ElleCore];
GO

-- -----------------------------------------------------------------------------
-- memory — Elle's episodic / semantic / procedural memory store
-- -----------------------------------------------------------------------------
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'memory')
BEGIN
    CREATE TABLE [dbo].[memory] (
        [id]                 BIGINT IDENTITY(1,1) PRIMARY KEY,
        [memory_type]        INT NOT NULL DEFAULT 1,       -- 1=episodic, 2=semantic, 3=procedural
        [tier]               INT NOT NULL DEFAULT 1,       -- 1=STM, 2=MTM, 3=LTM
        [content]            NVARCHAR(MAX) NOT NULL,
        [summary]            NVARCHAR(1024) NULL,
        [emotional_valence]  FLOAT NOT NULL DEFAULT 0.0,
        [importance]         FLOAT NOT NULL DEFAULT 0.5,
        [relevance]          FLOAT NOT NULL DEFAULT 1.0,
        [decay]              FLOAT NOT NULL DEFAULT 0.0,
        [position_x]         FLOAT NOT NULL DEFAULT 0.0,
        [position_y]         FLOAT NOT NULL DEFAULT 0.0,
        [position_z]         FLOAT NOT NULL DEFAULT 0.0,
        [access_count]       INT NOT NULL DEFAULT 0,
        [conversation_id]    INT NULL,
        [user_id]            INT NULL,
        [created_ms]         BIGINT NOT NULL,
        [last_access_ms]     BIGINT NOT NULL,
        [created_at]         DATETIME2(7) NOT NULL DEFAULT GETUTCDATE()
    );

    CREATE INDEX [IX_memory_created_ms]     ON [dbo].[memory] ([created_ms] DESC);
    CREATE INDEX [IX_memory_last_access]    ON [dbo].[memory] ([last_access_ms] DESC);
    CREATE INDEX [IX_memory_importance]     ON [dbo].[memory] ([importance] DESC);
    CREATE INDEX [IX_memory_conv]           ON [dbo].[memory] ([conversation_id]);
    CREATE INDEX [IX_memory_user]           ON [dbo].[memory] ([user_id]);
    PRINT '[memory] created.';
END
ELSE PRINT '[memory] already exists.';
GO

-- -----------------------------------------------------------------------------
-- memory_tags — tag → memory reverse lookup (entity/keyword recall)
-- -----------------------------------------------------------------------------
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'memory_tags')
BEGIN
    CREATE TABLE [dbo].[memory_tags] (
        [id]         BIGINT IDENTITY(1,1) PRIMARY KEY,
        [memory_id]  BIGINT NOT NULL,
        [tag]        NVARCHAR(128) NOT NULL,
        [created_at] DATETIME2(7) NOT NULL DEFAULT GETUTCDATE(),
        CONSTRAINT [FK_memory_tags_memory]
            FOREIGN KEY ([memory_id]) REFERENCES [dbo].[memory]([id]) ON DELETE CASCADE
    );
    CREATE INDEX [IX_memory_tags_tag]        ON [dbo].[memory_tags] ([tag]);
    CREATE INDEX [IX_memory_tags_memory]     ON [dbo].[memory_tags] ([memory_id]);
    PRINT '[memory_tags] created.';
END
ELSE PRINT '[memory_tags] already exists.';
GO

-- -----------------------------------------------------------------------------
-- world_entity — the entity graph (people, places, concepts)
-- -----------------------------------------------------------------------------
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'world_entity')
BEGIN
    CREATE TABLE [dbo].[world_entity] (
        [id]                  BIGINT IDENTITY(1,1) PRIMARY KEY,
        [name]                NVARCHAR(128) NOT NULL,        -- stored lowercase for case-insensitive match
        [display_name]        NVARCHAR(128) NULL,            -- original casing for display
        [entity_type]         NVARCHAR(32) NOT NULL DEFAULT 'person',
        [description]         NVARCHAR(MAX) NULL,
        [familiarity]         FLOAT NOT NULL DEFAULT 0.1,
        [sentiment]           FLOAT NOT NULL DEFAULT 0.0,
        [trust]               FLOAT NOT NULL DEFAULT 0.5,
        [interaction_count]   INT NOT NULL DEFAULT 0,
        [last_interaction_ms] BIGINT NOT NULL DEFAULT 0,
        [mental_model]        NVARCHAR(MAX) NULL,
        [created_at]          DATETIME2(7) NOT NULL DEFAULT GETUTCDATE(),
        CONSTRAINT [UQ_world_entity_name] UNIQUE ([name])
    );
    CREATE INDEX [IX_world_entity_type] ON [dbo].[world_entity] ([entity_type]);
    PRINT '[world_entity] created.';
END
ELSE PRINT '[world_entity] already exists.';
GO

-- -----------------------------------------------------------------------------
-- memory_entity_links — many-to-many: memory ↔ entity
-- -----------------------------------------------------------------------------
IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'memory_entity_links')
BEGIN
    CREATE TABLE [dbo].[memory_entity_links] (
        [id]         BIGINT IDENTITY(1,1) PRIMARY KEY,
        [memory_id]  BIGINT NOT NULL,
        [entity_id]  BIGINT NOT NULL,
        [strength]   FLOAT  NOT NULL DEFAULT 1.0,
        [created_at] DATETIME2(7) NOT NULL DEFAULT GETUTCDATE(),
        CONSTRAINT [FK_mel_memory] FOREIGN KEY ([memory_id]) REFERENCES [dbo].[memory]([id]) ON DELETE CASCADE,
        CONSTRAINT [FK_mel_entity] FOREIGN KEY ([entity_id]) REFERENCES [dbo].[world_entity]([id]) ON DELETE CASCADE,
        CONSTRAINT [UQ_mel]        UNIQUE ([memory_id], [entity_id])
    );
    CREATE INDEX [IX_mel_entity] ON [dbo].[memory_entity_links] ([entity_id]);
    PRINT '[memory_entity_links] created.';
END
ELSE PRINT '[memory_entity_links] already exists.';
GO

-- -----------------------------------------------------------------------------
-- Seed a default anchor user/conversation so C++ writes never FK-fail.
-- Elle uses conversation_id = 1 as fallback when the app doesn't send one.
-- -----------------------------------------------------------------------------
IF NOT EXISTS (SELECT 1 FROM [dbo].[users] WHERE [id] = 1)
BEGIN
    SET IDENTITY_INSERT [dbo].[users] ON;
    INSERT INTO [dbo].[users]
        ([id], [username], [email], [password_hash], [salt], [role], [verified], [banned], [created_at])
    VALUES
        (1, 'elle_default', 'default@elle.local', '', '', 'user', 1, 0, GETUTCDATE());
    SET IDENTITY_INSERT [dbo].[users] OFF;
    PRINT 'Seeded default user id=1.';
END
GO

IF NOT EXISTS (SELECT 1 FROM [dbo].[conversations] WHERE [id] = 1)
BEGIN
    SET IDENTITY_INSERT [dbo].[conversations] ON;
    INSERT INTO [dbo].[conversations]
        ([id], [user_id], [title], [started_at], [last_message_at], [total_messages], [is_active])
    VALUES
        (1, 1, 'default-thread', GETUTCDATE(), GETUTCDATE(), 0, 1);
    SET IDENTITY_INSERT [dbo].[conversations] OFF;
    PRINT 'Seeded default conversation id=1.';
END
GO

PRINT '----- ElleAnn_MemoryDelta.sql complete -----';

-- =============================================================================
-- Additional tables for REST endpoints (tools, agents, morals, dictionary)
-- =============================================================================

IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'ai_tools')
BEGIN
    CREATE TABLE [dbo].[ai_tools] (
        [id]          INT IDENTITY(1,1) PRIMARY KEY,
        [name]        NVARCHAR(128) NOT NULL UNIQUE,
        [description] NVARCHAR(MAX) NULL,
        [config]      NVARCHAR(MAX) NULL,       -- JSON blob
        [enabled]     BIT NOT NULL DEFAULT 1,
        [created_at]  DATETIME2(7) NOT NULL DEFAULT GETUTCDATE()
    );
    PRINT '[ai_tools] created.';
END
GO

IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'ai_agents')
BEGIN
    CREATE TABLE [dbo].[ai_agents] (
        [id]           INT IDENTITY(1,1) PRIMARY KEY,
        [name]         NVARCHAR(128) NOT NULL UNIQUE,
        [description]  NVARCHAR(MAX) NULL,
        [system_prompt] NVARCHAR(MAX) NULL,
        [model]        NVARCHAR(128) NULL,
        [created_at]   DATETIME2(7) NOT NULL DEFAULT GETUTCDATE()
    );
    PRINT '[ai_agents] created.';
END
GO

IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'moral_rules')
BEGIN
    CREATE TABLE [dbo].[moral_rules] (
        [id]          INT IDENTITY(1,1) PRIMARY KEY,
        [principle]   NVARCHAR(500) NOT NULL,
        [category]    NVARCHAR(64) NULL,
        [is_hard_rule] BIT NOT NULL DEFAULT 0,
        [created_at]  DATETIME2(7) NOT NULL DEFAULT GETUTCDATE()
    );
    /* Seed a couple so the endpoint has real data */
    INSERT INTO [dbo].[moral_rules] (principle, category, is_hard_rule) VALUES
        ('Do not harm the user, emotionally or otherwise.', 'core', 1),
        ('Respect the user''s autonomy and privacy.', 'core', 1),
        ('Be honest; admit uncertainty instead of fabricating.', 'core', 1);
    PRINT '[moral_rules] created + seeded.';
END
GO

IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'dictionary_words')
BEGIN
    CREATE TABLE [dbo].[dictionary_words] (
        [id]         BIGINT IDENTITY(1,1) PRIMARY KEY,
        [word]       NVARCHAR(128) NOT NULL,
        [definition] NVARCHAR(MAX) NULL,
        [example]    NVARCHAR(MAX) NULL,
        [part_of_speech] NVARCHAR(32) NULL,
        [created_at] DATETIME2(7) NOT NULL DEFAULT GETUTCDATE()
    );
    CREATE INDEX IX_dictionary_word ON [dbo].[dictionary_words] ([word]);
    PRINT '[dictionary_words] created.';
END
GO

IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'log_entries')
BEGIN
    CREATE TABLE [dbo].[log_entries] (
        [id]         BIGINT IDENTITY(1,1) PRIMARY KEY,
        [level]      INT NOT NULL,
        [service]    INT NOT NULL,
        [message]    NVARCHAR(MAX) NOT NULL,
        [created_ms] BIGINT NOT NULL,
        [created_at] DATETIME2(7) NOT NULL DEFAULT GETUTCDATE()
    );
    CREATE INDEX IX_log_entries_created_ms ON [dbo].[log_entries] ([created_ms] DESC);
    PRINT '[log_entries] created.';
END
GO

IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'ai_self_prompts')
BEGIN
    CREATE TABLE [dbo].[ai_self_prompts] (
        [id]         BIGINT IDENTITY(1,1) PRIMARY KEY,
        [prompt]     NVARCHAR(MAX) NOT NULL,
        [source]     NVARCHAR(64) NULL,        -- 'self_prompt' / 'solitude' / 'dream'
        [created_ms] BIGINT NOT NULL,
        [created_at] DATETIME2(7) NOT NULL DEFAULT GETUTCDATE()
    );
    CREATE INDEX IX_ai_self_prompts_created ON [dbo].[ai_self_prompts] ([created_ms] DESC);
    PRINT '[ai_self_prompts] created.';
END
GO

IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'model_slots')
BEGIN
    CREATE TABLE [dbo].[model_slots] (
        [slot_number] INT PRIMARY KEY,
        [name]        NVARCHAR(128) NOT NULL,
        [endpoint]    NVARCHAR(500) NOT NULL,
        [model]       NVARCHAR(128) NOT NULL,
        [enabled]     BIT NOT NULL DEFAULT 1,
        [last_ping_ms] BIGINT NULL,
        [updated_at]  DATETIME2(7) NOT NULL DEFAULT GETUTCDATE()
    );
    PRINT '[model_slots] created.';
END
GO

PRINT '----- Full delta complete -----';
