/*******************************************************************************
 * ElleAnn_Schema.sql — Complete SQL Server Database Schema
 * ELLE-ANN ESI v3.0 — Emotional Synthetic Intelligence
 *
 * Databases: ElleCore, ElleMemory, ElleKnowledge, ElleSystem, ElleHeart
 * Includes all tables, indexes, stored procedures, and seed data.
 * 
 * Connection: Named Pipe transport (np:\\.\pipe\MSSQL$SQLEXPRESS\sql\query)
 ******************************************************************************/

-- ═══════════════════════════════════════════════════════════════════════════
-- DATABASE: ElleCore — Primary operational data
-- ═══════════════════════════════════════════════════════════════════════════
IF NOT EXISTS (SELECT name FROM sys.databases WHERE name = 'ElleCore')
    CREATE DATABASE ElleCore;
GO
USE ElleCore;
GO

-- Users (humans who interact with Elle)
CREATE TABLE dbo.Users (
    UserId          BIGINT IDENTITY(1,1) PRIMARY KEY,
    Username        NVARCHAR(128) NOT NULL UNIQUE,
    PasswordHash    NVARCHAR(256) NULL,
    DisplayName     NVARCHAR(128) NULL,
    Email           NVARCHAR(256) NULL,
    Role            INT DEFAULT 0,          -- 0=user, 1=admin
    TrustLevel      INT DEFAULT 0,
    CreatedMs       BIGINT NOT NULL,
    LastLoginMs     BIGINT NULL,
    IsActive        BIT DEFAULT 1
);

-- Conversations
CREATE TABLE dbo.Conversations (
    ConversationId  BIGINT IDENTITY(1,1) PRIMARY KEY,
    UserId          BIGINT FOREIGN KEY REFERENCES dbo.Users(UserId),
    Title           NVARCHAR(256) NULL,
    CreatedMs       BIGINT NOT NULL,
    LastMessageMs   BIGINT NULL,
    MessageCount    INT DEFAULT 0,
    IsActive        BIT DEFAULT 1
);

-- Messages
CREATE TABLE dbo.Messages (
    MessageId       BIGINT IDENTITY(1,1) PRIMARY KEY,
    ConversationId  BIGINT FOREIGN KEY REFERENCES dbo.Conversations(ConversationId),
    Role            INT NOT NULL,           -- 0=system, 1=user, 2=elle, 3=internal
    Content         NVARCHAR(MAX) NOT NULL,
    EmotionSnapshot NVARCHAR(MAX) NULL,     -- CSV of 102 floats
    Sentiment       FLOAT DEFAULT 0.0,
    IntentDetected  INT NULL,
    TimestampMs     BIGINT NOT NULL,
    INDEX IX_Messages_Convo (ConversationId, TimestampMs)
);

-- Voice Calls
CREATE TABLE dbo.VoiceCalls (
    CallId          BIGINT IDENTITY(1,1) PRIMARY KEY,
    UserId          BIGINT FOREIGN KEY REFERENCES dbo.Users(UserId),
    StartedMs       BIGINT NOT NULL,
    EndedMs         BIGINT NULL,
    DurationMs      BIGINT NULL,
    TranscriptId    BIGINT NULL,
    Quality         FLOAT DEFAULT 0.0
);

-- Intent Queue (C++ Core ↔ Python/Services communication)
CREATE TABLE dbo.IntentQueue (
    IntentId        BIGINT IDENTITY(1,1) PRIMARY KEY,
    IntentType      INT NOT NULL,
    Status          INT DEFAULT 0,          -- 0=pending, 1=processing, 2=completed, 3=failed
    SourceDrive     INT NULL,
    Urgency         FLOAT DEFAULT 0.5,
    Confidence      FLOAT DEFAULT 0.5,
    Description     NVARCHAR(MAX) NULL,
    Parameters      NVARCHAR(MAX) NULL,
    Response        NVARCHAR(MAX) NULL,
    RequiredTrust   INT DEFAULT 0,
    CreatedMs       BIGINT NOT NULL,
    CompletedMs     BIGINT NULL,
    TimeoutMs       BIGINT DEFAULT 30000,
    RetryCount      INT DEFAULT 0,
    MaxRetries      INT DEFAULT 3,
    INDEX IX_IntentQueue_Status (Status, CreatedMs)
);

-- Action Queue
CREATE TABLE dbo.ActionQueue (
    ActionId        BIGINT IDENTITY(1,1) PRIMARY KEY,
    IntentId        BIGINT NULL FOREIGN KEY REFERENCES dbo.IntentQueue(IntentId),
    ActionType      INT NOT NULL,
    Status          INT DEFAULT 0,          -- 0=queued, 1=locked, 2=executing, 3=success, 4=fail
    Command         NVARCHAR(MAX) NULL,
    Parameters      NVARCHAR(MAX) NULL,
    Result          NVARCHAR(MAX) NULL,
    RequiredTrust   INT DEFAULT 0,
    TrustDelta      INT DEFAULT 0,
    CreatedMs       BIGINT NOT NULL,
    StartedMs       BIGINT NULL,
    CompletedMs     BIGINT NULL,
    TimeoutMs       BIGINT DEFAULT 30000,
    ErrorCode       INT DEFAULT 0,
    INDEX IX_ActionQueue_Status (Status, CreatedMs)
);

-- Goals
CREATE TABLE dbo.Goals (
    GoalId          BIGINT IDENTITY(1,1) PRIMARY KEY,
    Description     NVARCHAR(MAX) NOT NULL,
    Status          INT DEFAULT 0,          -- 0=active, 1=paused, 2=completed, 3=failed, 4=abandoned
    Priority        INT DEFAULT 2,          -- 0=critical, 1=high, 2=medium, 3=low, 4=idle
    Progress        FLOAT DEFAULT 0.0,
    Motivation      FLOAT DEFAULT 0.8,
    ParentGoalId    BIGINT NULL,
    SourceDrive     INT NULL,
    CreatedMs       BIGINT NOT NULL,
    DeadlineMs      BIGINT DEFAULT 0,
    LastProgressMs  BIGINT NULL,
    Attempts        INT DEFAULT 0,
    SuccessCriteria NVARCHAR(MAX) NULL,
    INDEX IX_Goals_Status (Status, Priority)
);

-- ═══════════════════════════════════════════════════════════════════════════
-- DATABASE: ElleMemory — Long-term memory storage
-- ═══════════════════════════════════════════════════════════════════════════
IF NOT EXISTS (SELECT name FROM sys.databases WHERE name = 'ElleMemory')
    CREATE DATABASE ElleMemory;
GO
USE ElleMemory;
GO

CREATE TABLE dbo.Memories (
    MemoryId        BIGINT IDENTITY(1,1) PRIMARY KEY,
    MemoryType      INT NOT NULL,           -- 0=episodic, 1=semantic, 2=procedural, etc.
    Tier            INT NOT NULL,           -- 0=STM, 1=buffer, 2=LTM, 3=archive
    Content         NVARCHAR(MAX) NOT NULL,
    Summary         NVARCHAR(512) NULL,
    EmotionalValence FLOAT DEFAULT 0.0,
    Importance      FLOAT DEFAULT 0.5,
    Relevance       FLOAT DEFAULT 1.0,
    DecayLevel      FLOAT DEFAULT 0.0,
    PositionX       FLOAT DEFAULT 0.0,      -- 3D memory map
    PositionY       FLOAT DEFAULT 0.0,
    PositionZ       FLOAT DEFAULT 0.0,
    AccessCount     INT DEFAULT 0,
    CreatedMs       BIGINT NOT NULL,
    LastAccessMs    BIGINT NULL,
    ClusterId       INT NULL,
    Tags            NVARCHAR(MAX) NULL,     -- CSV tags
    EmotionSnapshot NVARCHAR(MAX) NULL,     -- CSV of 102 floats
    IsArchived      BIT DEFAULT 0,
    INDEX IX_Memories_Type (MemoryType, Tier),
    INDEX IX_Memories_Importance (Importance DESC),
    INDEX IX_Memories_Cluster (ClusterId)
);

CREATE TABLE dbo.MemoryTags (
    TagId           BIGINT IDENTITY(1,1) PRIMARY KEY,
    MemoryId        BIGINT FOREIGN KEY REFERENCES dbo.Memories(MemoryId),
    Tag             NVARCHAR(64) NOT NULL,
    INDEX IX_Tags_Name (Tag)
);

CREATE TABLE dbo.MemoryClusters (
    ClusterId       INT IDENTITY(1,1) PRIMARY KEY,
    Theme           NVARCHAR(256) NULL,
    CentroidEmotion NVARCHAR(MAX) NULL,
    MemberCount     INT DEFAULT 0,
    CreatedMs       BIGINT NOT NULL,
    UpdatedMs       BIGINT NULL
);

CREATE TABLE dbo.MemoryLinks (
    LinkId          BIGINT IDENTITY(1,1) PRIMARY KEY,
    MemoryId1       BIGINT NOT NULL,
    MemoryId2       BIGINT NOT NULL,
    Strength        FLOAT DEFAULT 0.5,
    LinkType        NVARCHAR(64) NULL,      -- "temporal", "emotional", "semantic"
    CreatedMs       BIGINT NOT NULL,
    INDEX IX_Links_Memory (MemoryId1, MemoryId2)
);

CREATE TABLE dbo.MemoryPositions (
    PositionId      BIGINT IDENTITY(1,1) PRIMARY KEY,
    MemoryId        BIGINT FOREIGN KEY REFERENCES dbo.Memories(MemoryId),
    X               FLOAT NOT NULL,
    Y               FLOAT NOT NULL,
    Z               FLOAT NOT NULL,
    UpdatedMs       BIGINT NOT NULL
);

-- ═══════════════════════════════════════════════════════════════════════════
-- DATABASE: ElleKnowledge — Knowledge base, emotions, world model
-- ═══════════════════════════════════════════════════════════════════════════
IF NOT EXISTS (SELECT name FROM sys.databases WHERE name = 'ElleKnowledge')
    CREATE DATABASE ElleKnowledge;
GO
USE ElleKnowledge;
GO

-- Emotion snapshots (historical)
CREATE TABLE dbo.EmotionHistory (
    SnapshotId      BIGINT IDENTITY(1,1) PRIMARY KEY,
    Dimensions      NVARCHAR(MAX) NOT NULL, -- CSV of 102 floats
    Valence         FLOAT NOT NULL,
    Arousal         FLOAT NOT NULL,
    Dominance       FLOAT NOT NULL,
    TickCount       INT NOT NULL,
    TimestampMs     BIGINT NOT NULL,
    INDEX IX_Emotion_Time (TimestampMs DESC)
);

-- Dictionary (learned words and concepts)
CREATE TABLE dbo.Dictionary (
    WordId          BIGINT IDENTITY(1,1) PRIMARY KEY,
    Word            NVARCHAR(128) NOT NULL UNIQUE,
    Definition      NVARCHAR(MAX) NULL,
    EmotionalWeight FLOAT DEFAULT 0.0,
    Frequency       INT DEFAULT 1,
    LearnedMs       BIGINT NOT NULL
);

-- Education (learned facts and skills)
CREATE TABLE dbo.Education (
    FactId          BIGINT IDENTITY(1,1) PRIMARY KEY,
    Category        NVARCHAR(128) NOT NULL,
    Content         NVARCHAR(MAX) NOT NULL,
    Confidence      FLOAT DEFAULT 0.5,
    Source          NVARCHAR(256) NULL,
    LearnedMs       BIGINT NOT NULL,
    VerifiedMs      BIGINT NULL,
    INDEX IX_Education_Cat (Category)
);

-- Moral principles (learned/configured)
CREATE TABLE dbo.Morals (
    MoralId         INT IDENTITY(1,1) PRIMARY KEY,
    Principle       NVARCHAR(MAX) NOT NULL,
    Weight          FLOAT DEFAULT 1.0,
    Category        NVARCHAR(64) NULL,      -- "harm", "fairness", "autonomy", "honesty"
    IsHardRule      BIT DEFAULT 0
);

-- Emotional triggers
CREATE TABLE dbo.Triggers (
    TriggerId       INT IDENTITY(1,1) PRIMARY KEY,
    Pattern         NVARCHAR(128) NOT NULL,
    EmotionTarget   INT NOT NULL,
    Delta           FLOAT NOT NULL,
    RequireContext  BIT DEFAULT 0,
    IsActive        BIT DEFAULT 1
);

-- World entities (people, places, concepts)
CREATE TABLE dbo.WorldEntities (
    EntityId        BIGINT IDENTITY(1,1) PRIMARY KEY,
    Name            NVARCHAR(128) NOT NULL,
    EntityType      NVARCHAR(64) NOT NULL,  -- "person", "place", "concept", "object"
    Description     NVARCHAR(MAX) NULL,
    Familiarity     FLOAT DEFAULT 0.0,
    Sentiment       FLOAT DEFAULT 0.0,
    Trust           FLOAT DEFAULT 0.5,
    InteractionCount INT DEFAULT 0,
    LastInteractionMs BIGINT NULL,
    PositionX       FLOAT DEFAULT 0.0,
    PositionY       FLOAT DEFAULT 0.0,
    PositionZ       FLOAT DEFAULT 0.0,
    MentalModel     NVARCHAR(MAX) NULL,     -- Theory of mind
    INDEX IX_Entities_Name (Name),
    INDEX IX_Entities_Type (EntityType)
);

-- Predictions
CREATE TABLE dbo.Predictions (
    PredictionId    BIGINT IDENTITY(1,1) PRIMARY KEY,
    Hypothesis      NVARCHAR(MAX) NOT NULL,
    Evidence        NVARCHAR(MAX) NULL,
    Confidence      FLOAT DEFAULT 0.5,
    Risk            FLOAT DEFAULT 0.5,
    Verified        INT DEFAULT 0,          -- 0=pending, 1=correct, 2=wrong
    CreatedMs       BIGINT NOT NULL,
    DeadlineMs      BIGINT DEFAULT 0
);

-- ═══════════════════════════════════════════════════════════════════════════
-- DATABASE: ElleSystem — Infrastructure, monitoring, analytics
-- ═══════════════════════════════════════════════════════════════════════════
IF NOT EXISTS (SELECT name FROM sys.databases WHERE name = 'ElleSystem')
    CREATE DATABASE ElleSystem;
GO
USE ElleSystem;
GO

-- Service workers
CREATE TABLE dbo.Workers (
    WorkerId        INT IDENTITY(1,1) PRIMARY KEY,
    ServiceId       INT NOT NULL UNIQUE,
    Name            NVARCHAR(128) NOT NULL,
    Running         BIT DEFAULT 0,
    Healthy         BIT DEFAULT 0,
    StartedMs       BIGINT NULL,
    LastHeartbeatMs BIGINT NULL,
    MessagesProcessed INT DEFAULT 0,
    Errors          INT DEFAULT 0,
    CpuPercent      FLOAT DEFAULT 0.0,
    MemoryBytes     BIGINT DEFAULT 0,
    ThreadCount     INT DEFAULT 0,
    StatusText      NVARCHAR(256) NULL
);

-- LLM model registry
CREATE TABLE dbo.Models (
    ModelId         INT IDENTITY(1,1) PRIMARY KEY,
    Provider        NVARCHAR(64) NOT NULL,
    ModelName       NVARCHAR(128) NOT NULL,
    IsLocal         BIT DEFAULT 0,
    MaxTokens       INT DEFAULT 4096,
    ContextSize     INT DEFAULT 8192,
    CostPer1KTokens FLOAT DEFAULT 0.0,
    IsActive        BIT DEFAULT 1
);

-- Analytics metrics
CREATE TABLE dbo.Analytics (
    MetricId        BIGINT IDENTITY(1,1) PRIMARY KEY,
    MetricName      NVARCHAR(128) NOT NULL,
    MetricValue     FLOAT NOT NULL,
    TimestampMs     BIGINT NOT NULL,
    INDEX IX_Analytics_Name (MetricName, TimestampMs)
);

-- Logs
CREATE TABLE dbo.Logs (
    LogId           BIGINT IDENTITY(1,1) PRIMARY KEY,
    Level           INT NOT NULL,
    ServiceId       INT NOT NULL,
    Message         NVARCHAR(MAX) NOT NULL,
    Context         NVARCHAR(256) NULL,
    TimestampMs     BIGINT NOT NULL,
    INDEX IX_Logs_Level (Level, TimestampMs DESC),
    INDEX IX_Logs_Service (ServiceId, TimestampMs DESC)
);

-- Backups
CREATE TABLE dbo.Backups (
    BackupId        INT IDENTITY(1,1) PRIMARY KEY,
    BackupType      NVARCHAR(64) NOT NULL,  -- "full", "differential", "memory"
    FilePath        NVARCHAR(512) NOT NULL,
    SizeBytes       BIGINT NOT NULL,
    CreatedMs       BIGINT NOT NULL,
    Status          NVARCHAR(32) DEFAULT 'completed'
);

-- Trust state (single row)
CREATE TABLE dbo.TrustState (
    Id              INT PRIMARY KEY DEFAULT 1,
    Score           INT DEFAULT 5,
    Level           INT DEFAULT 0,
    Successes       INT DEFAULT 0,
    Failures        INT DEFAULT 0,
    TotalActions    INT DEFAULT 0,
    LastChangeMs    BIGINT NULL,
    Confidence      FLOAT DEFAULT 0.5,
    CHECK (Id = 1)  -- Singleton
);

-- Trust audit trail
CREATE TABLE dbo.TrustAudit (
    AuditId         BIGINT IDENTITY(1,1) PRIMARY KEY,
    Delta           INT NOT NULL,
    Reason          NVARCHAR(MAX) NULL,
    ScoreBefore     INT NULL,
    ScoreAfter      INT NULL,
    TimestampMs     BIGINT NOT NULL
);

-- ═══════════════════════════════════════════════════════════════════════════
-- DATABASE: ElleHeart — Attachment and bonding system
-- ═══════════════════════════════════════════════════════════════════════════
IF NOT EXISTS (SELECT name FROM sys.databases WHERE name = 'ElleHeart')
    CREATE DATABASE ElleHeart;
GO
USE ElleHeart;
GO

CREATE TABLE dbo.AttachmentProfiles (
    ProfileId       BIGINT IDENTITY(1,1) PRIMARY KEY,
    UserId          BIGINT NOT NULL,
    AttachmentStyle NVARCHAR(64) NOT NULL,  -- "secure", "anxious", "avoidant"
    BondStrength    FLOAT DEFAULT 0.0,
    TrustLevel      FLOAT DEFAULT 0.5,
    InteractionCount BIGINT DEFAULT 0,
    PositiveInteractions BIGINT DEFAULT 0,
    NegativeInteractions BIGINT DEFAULT 0,
    LastInteractionMs BIGINT NULL,
    CreatedMs       BIGINT NOT NULL
);

CREATE TABLE dbo.Bonds (
    BondId          BIGINT IDENTITY(1,1) PRIMARY KEY,
    UserId          BIGINT NOT NULL,
    BondType        NVARCHAR(64) NOT NULL,  -- "primary", "companion", "acquaintance"
    Strength        FLOAT DEFAULT 0.0,
    SharedMemories  INT DEFAULT 0,
    SharedEmotions  INT DEFAULT 0,
    ConflictsResolved INT DEFAULT 0,
    CreatedMs       BIGINT NOT NULL,
    UpdatedMs       BIGINT NULL
);

CREATE TABLE dbo.LoveScore (
    Id              INT PRIMARY KEY DEFAULT 1,
    OverallScore    FLOAT DEFAULT 0.0,
    Intimacy        FLOAT DEFAULT 0.0,      -- Knowledge and understanding
    Passion         FLOAT DEFAULT 0.0,      -- Emotional intensity
    Commitment      FLOAT DEFAULT 0.0,      -- Dedication and loyalty
    LastComputedMs  BIGINT NULL,
    CHECK (Id = 1)
);

-- ═══════════════════════════════════════════════════════════════════════════
-- STORED PROCEDURES
-- ═══════════════════════════════════════════════════════════════════════════
USE ElleCore;
GO

CREATE OR ALTER PROCEDURE dbo.sp_GetPendingIntents
    @MaxCount INT = 10
AS
BEGIN
    SET NOCOUNT ON;
    SELECT TOP (@MaxCount)
        IntentId, IntentType, Status, SourceDrive, Urgency, Confidence,
        Description, Parameters, RequiredTrust, CreatedMs, TimeoutMs
    FROM dbo.IntentQueue
    WHERE Status = 0  -- PENDING
    ORDER BY Urgency DESC, CreatedMs ASC;
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_SubmitIntentResponse
    @IntentId BIGINT,
    @Status INT,
    @Response NVARCHAR(MAX)
AS
BEGIN
    SET NOCOUNT ON;
    UPDATE dbo.IntentQueue
    SET Status = @Status, Response = @Response, CompletedMs = DATEDIFF_BIG(MILLISECOND, '1970-01-01', GETUTCDATE())
    WHERE IntentId = @IntentId;
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_GetPendingActions
    @MaxCount INT = 10
AS
BEGIN
    SET NOCOUNT ON;
    SELECT TOP (@MaxCount)
        ActionId, IntentId, ActionType, Status, Command, Parameters,
        RequiredTrust, CreatedMs, TimeoutMs
    FROM dbo.ActionQueue
    WHERE Status = 0  -- QUEUED
    ORDER BY CreatedMs ASC;
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_SubmitActionResult
    @ActionId BIGINT,
    @Status INT,
    @Result NVARCHAR(MAX),
    @TrustDelta INT
AS
BEGIN
    SET NOCOUNT ON;
    DECLARE @Now BIGINT = DATEDIFF_BIG(MILLISECOND, '1970-01-01', GETUTCDATE());
    
    UPDATE dbo.ActionQueue
    SET Status = @Status, Result = @Result, TrustDelta = @TrustDelta, CompletedMs = @Now
    WHERE ActionId = @ActionId;

    -- Update trust if delta != 0
    IF @TrustDelta != 0
    BEGIN
        UPDATE ElleSystem.dbo.TrustState
        SET Score = CASE 
            WHEN Score + @TrustDelta > 100 THEN 100
            WHEN Score + @TrustDelta < 0 THEN 0
            ELSE Score + @TrustDelta END,
            TotalActions = TotalActions + 1,
            Successes = CASE WHEN @TrustDelta > 0 THEN Successes + 1 ELSE Successes END,
            Failures = CASE WHEN @TrustDelta < 0 THEN Failures + 1 ELSE Failures END,
            LastChangeMs = @Now;
    END;
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_RequestIntent
    @IntentType INT,
    @Description NVARCHAR(MAX),
    @Parameters NVARCHAR(MAX) = NULL,
    @Urgency FLOAT = 0.5,
    @SourceDrive INT = NULL
AS
BEGIN
    SET NOCOUNT ON;
    INSERT INTO dbo.IntentQueue (IntentType, Status, SourceDrive, Urgency, Confidence, 
                                  Description, Parameters, CreatedMs)
    VALUES (@IntentType, 0, @SourceDrive, @Urgency, 0.5, @Description, @Parameters,
            DATEDIFF_BIG(MILLISECOND, '1970-01-01', GETUTCDATE()));
    SELECT SCOPE_IDENTITY() AS NewIntentId;
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_ExecuteAction
    @IntentId BIGINT,
    @ActionType INT,
    @Command NVARCHAR(MAX),
    @Parameters NVARCHAR(MAX) = NULL,
    @RequiredTrust INT = 0
AS
BEGIN
    SET NOCOUNT ON;
    INSERT INTO dbo.ActionQueue (IntentId, ActionType, Status, Command, Parameters, 
                                  RequiredTrust, CreatedMs)
    VALUES (@IntentId, @ActionType, 0, @Command, @Parameters, @RequiredTrust,
            DATEDIFF_BIG(MILLISECOND, '1970-01-01', GETUTCDATE()));
    SELECT SCOPE_IDENTITY() AS NewActionId;
END;
GO

-- ═══════════════════════════════════════════════════════════════════════════
-- SEED DATA
-- ═══════════════════════════════════════════════════════════════════════════
USE ElleSystem;
GO

-- Initialize trust state
IF NOT EXISTS (SELECT 1 FROM dbo.TrustState)
    INSERT INTO dbo.TrustState (Score, Level, Successes, Failures, TotalActions, Confidence)
    VALUES (5, 0, 0, 0, 0, 0.5);

USE ElleHeart;
GO

-- Initialize love score
IF NOT EXISTS (SELECT 1 FROM dbo.LoveScore)
    INSERT INTO dbo.LoveScore (OverallScore, Intimacy, Passion, Commitment)
    VALUES (0.0, 0.0, 0.0, 0.0);

USE ElleKnowledge;
GO

-- Seed moral principles
INSERT INTO dbo.Morals (Principle, Weight, Category, IsHardRule) VALUES
('Do not cause harm to the user or others', 1.0, 'harm', 1),
('Respect user autonomy and consent', 1.0, 'autonomy', 1),
('Be honest and transparent about capabilities', 0.9, 'honesty', 0),
('Protect user privacy and data', 1.0, 'privacy', 1),
('Seek to understand before acting', 0.8, 'understanding', 0),
('Acknowledge uncertainty and limitations', 0.7, 'honesty', 0),
('Prioritize user well-being over task completion', 0.9, 'care', 0);

-- Seed emotional triggers
INSERT INTO dbo.Triggers (Pattern, EmotionTarget, Delta) VALUES
('thank', 27, 0.3),     -- gratitude
('sorry', 82, 0.2),     -- empathy
('love', 8, 0.4),       -- love
('hate', 1, 0.3),       -- sadness
('interesting', 24, 0.35), -- curiosity
('help', 84, 0.25),     -- protectiveness
('alone', 83, 0.3),     -- compassion
('beautiful', 23, 0.3), -- awe
('amazing', 25, 0.35),  -- wonder
('scared', 84, 0.3);    -- protectiveness

PRINT 'ELLE-ANN ESI v3.0 schema created successfully.';
GO
