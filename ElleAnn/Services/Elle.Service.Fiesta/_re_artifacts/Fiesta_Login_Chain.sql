/*══════════════════════════════════════════════════════════════════════════════
 * Fiesta_Login_Chain.sql
 *
 *   Canonical reference for the production Fiesta server's login + character
 *   selection + game-event audit chain.  Source: live SSMS dump from
 *   sys.sql_modules across [Account], [AccountLog], [World00_Character], and
 *   [World00_GameLog] — pulled by the operator on 2026-02-05.
 *
 *   This file is REFERENCE ONLY.  DO NOT execute as a schema delta — the
 *   procs already exist on the live server and most live versions carry
 *   site-specific tweaks (blocked IP regex, GM allowlist, custom audit
 *   columns) that are not represented here.  This dump is what Elle's
 *   Elle.Service.Fiesta is expected to interoperate WITH.
 *
 *   Map of databases ↔ procs (so the Elle ODBC gateway knows which
 *   connection string to acquire for each call):
 *
 *     [Account]
 *         usp_User_loginGame      authenticate by sUserID/sUserPW
 *         usp_User_chkBlock       resolve / auto-clear expired tUserBlock rows
 *
 *     [AccountLog]
 *         p_AccountLog_Add        login event audit row (tAccountLog)
 *         usp_Connection_setLogin INSERT tActiveUsers (online roster)
 *         usp_Connection_getWorldUserList   per-world online list
 *         usp_Connection_setLogout DELETE tActiveUsers (logout)
 *         usp_Log_setLogout       INSERT tLogout audit row
 *
 *     [World00_Character]   (one DB per world; substitute World01_..., etc)
 *         p_Char_GetListOfUserChar      live character list
 *         p_Char_GetListOfUserCharAll   live + soft-deleted
 *         p_Char_GetUserNo              char→user resolver
 *         p_Char_GetCharNo              sID→nCharNo resolver
 *         p_Char_GetLoginData           full login payload (joins shape)
 *         p_Char_Login                  bump nLoginCount + dLastLoginDate
 *         usp_CharacterLoading          zone-transition seq log
 *         usp_CharacterLoadingDetailInfo per-error data
 *
 *     [World00_GameLog]
 *         p_GameLog_Add                 generic 13-arg event audit
 *         p_GameLog_Add_Type1..Type3    optimized variants
 *
 *   ─── Operational notes for Elle ────────────────────────────────────────
 *     • All procs use OUTPUT params (@nRet, @nUserNo, etc).  The Elle
 *       ODBC layer must SQLBindParameter each output before SQLExecute or
 *       the values come back NULL.  ElleSQLConn.h's existing QueryParams()
 *       helper does NOT yet bind output params — the Fiesta gateway will
 *       need a thin wrapper (CallProcWithOutputs) that uses SQL_PARAM_OUTPUT.
 *
 *     • [AccountLog].dbo.tActiveUsers is the SINGLE SOURCE OF TRUTH for
 *       "is user X currently online?".  Elle MUST upsert into this table
 *       on every successful login-game and DELETE on logout — otherwise
 *       duplicate-login detection on the live server's WorldManager.exe
 *       fails, and the user's existing session gets booted by the next
 *       login attempt.  Sequence:
 *         1.  usp_User_loginGame      → confirm credentials + block state
 *         2.  usp_Connection_setLogin → register online presence
 *         3.  p_AccountLog_Add(@nType=1) → audit successful login
 *         4.  ... game session ...
 *         5.  usp_Log_setLogout       → write logout audit
 *         6.  usp_Connection_setLogout → clear active flag
 *
 *     • The SQL does NOT touch the network cipher.  The 499-byte XOR
 *       cipher in Elle.Service.Fiesta/FiestaNetCrypto.h handles wire
 *       encoding; the SQL layer sees plaintext credentials AFTER the
 *       packet has been decrypted by the Connection layer.  This means
 *       sUserPW arrives as cleartext nvarchar — match the live server's
 *       behaviour bit-for-bit (it does NOT hash before SQL).
 *══════════════════════════════════════════════════════════════════════════════*/

/* =========================================================================
 * [Account] AUTHENTICATION
 * ========================================================================= */

USE [Account];
GO

/*-------------------------------------------------------------------------------------
 * usp_User_loginGame
 *   Validates user login for game; checks block and loginable.
 *-----------------------------------------------------------------------------------*/
CREATE PROCEDURE dbo.usp_User_loginGame
    @userID      nvarchar(20)
,   @userPW      nvarchar(16)
,   @userNo      int     = 0 OUTPUT
,   @authID      tinyint = 0 OUTPUT
,   @block       int     = 0 OUTPUT
,   @isLoginable int     = 0 OUTPUT
AS
BEGIN
    SET NOCOUNT ON
    SET LOCK_TIMEOUT 5000

    DECLARE @RET INT
    SET @userNo = 0

    SELECT @userNo = nUserNo, @authID = U.nAuthID,
           @block  = bIsBlock, @isLoginable = bIsLoginAble
      FROM dbo.tUser     AS U  (NOLOCK)
INNER JOIN dbo.tUserAuth AS UA (NOLOCK) ON U.nAuthID = UA.nAuthID
     WHERE sUserID = @userID AND sUserPW = @userPW AND bIsDelete = 0;

    IF @userNo = 0 BEGIN
        SET @userNo = -1
        SET @authID = 0
        SET @block  = 0
    END
    ELSE BEGIN
        IF @block = 1 BEGIN
            EXEC usp_User_chkBlock @userNo, @RET OUTPUT
            IF @RET = 1 BEGIN
                SELECT @block = bIsBlock FROM tUser (NOLOCK) WHERE nUserNo = @userNo
            END
        END
    END

    SET LOCK_TIMEOUT -1
    SET NOCOUNT OFF
END
GO

/*-------------------------------------------------------------------------------------
 * usp_User_chkBlock
 *   Block check; may clear expired temporary blocks.
 *   Returns @RET=1 if the only active block was expired+cleared, 0 otherwise.
 *-----------------------------------------------------------------------------------*/
CREATE PROCEDURE dbo.usp_User_chkBlock
    @userNo INT
,   @RET    INT OUTPUT
AS
BEGIN
    SET NOCOUNT ON
    SET LOCK_TIMEOUT 5000

    DECLARE @BLOCK_NO INT
    DECLARE @TYPE     BIT
    DECLARE @END_DATE DATETIME

    SELECT TOP 1 @BLOCK_NO = nBlockNo, @TYPE = nType, @END_DATE = dEndDate
      FROM tUserBlock (NOLOCK)
     WHERE nUserNo = @userNo AND bStatus = 1
     ORDER BY nBlockNo;

    IF @TYPE = 1 AND @END_DATE < GETDATE() BEGIN
        BEGIN TRAN

        UPDATE tUserBlock SET bStatus = 0 WHERE nBlockNo = @BLOCK_NO
        IF @@ERROR <> 0 OR @@ROWCOUNT = 0 BEGIN
            ROLLBACK TRAN
            SET @RET = 0
            RETURN
        END

        IF EXISTS (SELECT nBlockNo FROM tUserBlock (NOLOCK)
                    WHERE nUserNo = @userNo AND bStatus = 1) BEGIN
            SET @RET = 0
        END
        ELSE BEGIN
            UPDATE tUser SET bIsBlock = 0 WHERE nUserNo = @userNo
            IF @@ERROR <> 0 OR @@ROWCOUNT = 0 BEGIN
                ROLLBACK TRAN
                SET @RET = 0
                RETURN
            END
            SET @RET = 1
        END

        COMMIT TRAN
    END

    SET NOCOUNT OFF
    SET LOCK_TIMEOUT -1
END
GO


/* =========================================================================
 * [AccountLog]  CONNECTION + LOGGING
 * ========================================================================= */

USE [AccountLog];
GO

CREATE PROCEDURE dbo.p_AccountLog_Add
    @nType         tinyint
,   @nUserNo       int
,   @nWorldNo      tinyint
,   @nPlayMin      smallint
,   @nErrorCode    smallint
,   @sIP           varchar(16)
,   @sUserID       nvarchar(16)
,   @sUserPassword nvarchar(16)
,   @nLogNo        bigint OUTPUT
AS
SET NOCOUNT ON
SET @nLogNo = 0
INSERT tAccountLog (nType, nUserNo, nWorldNo, nPlayMin, nErrorCode, sIP, sUserID, sUserPassword)
VALUES             (@nType, @nUserNo, @nWorldNo, @nPlayMin, @nErrorCode, @sIP, @sUserID, @sUserPassword)
SET @nLogNo = @@IDENTITY
GO

CREATE PROCEDURE dbo.usp_Connection_setLogin
    @nUserNo  int
,   @nWorldNo tinyint
,   @nRet     int OUTPUT
AS
BEGIN
    SET NOCOUNT ON
    INSERT INTO tActiveUsers (nUserNo, nWorldNo, dLoginTime, nIsOnline)
    VALUES                   (@nUserNo, @nWorldNo, GETDATE(), 1)
    SET @nRet = @@ERROR
END
GO

CREATE PROCEDURE dbo.usp_Connection_getWorldUserList
    @nWorldNo tinyint
,   @nRet     int OUTPUT
AS
BEGIN
    SET NOCOUNT ON
    SELECT nUserNo FROM tActiveUsers WHERE nWorldNo = @nWorldNo
    SET @nRet = @@ROWCOUNT
END
GO

CREATE PROCEDURE dbo.usp_Connection_setLogout
    @nUserNo  int
,   @nWorldNo tinyint
,   @nRet     int OUTPUT
AS
BEGIN
    SET NOCOUNT ON
    SET @nRet = -1
    DELETE FROM tActiveUsers WHERE nUserNo = @nUserNo
    SET @nRet = @@ERROR
END
GO

CREATE PROCEDURE dbo.usp_Log_setLogout
    @WORLD_NO   TINYINT
,   @USER_NO    INT
,   @CHAR_NO    INT
,   @CLASS      TINYINT
,   @LEVEL      SMALLINT
,   @EXPERIENCE BIGINT
,   @PLAYTIME   SMALLINT
,   @RET        INT = 0 OUTPUT
AS
BEGIN
    SET NOCOUNT ON
    INSERT INTO tLogout (worldNo, userNo, charNo, class, level, experience, playTime)
    VALUES              (@WORLD_NO, @USER_NO, @CHAR_NO, @CLASS, @LEVEL, @EXPERIENCE, @PLAYTIME)
    IF @@ERROR <> 0 OR @@IDENTITY = 0
        SET @RET = 0
    ELSE
        SET @RET = 1
    SET NOCOUNT OFF
END
GO


/* =========================================================================
 * [World00_Character]  CHARACTER SELECTION + LOGIN
 * ========================================================================= */

USE [World00_Character];
GO

CREATE PROCEDURE dbo.p_Char_GetListOfUserChar
    @nUserNo int
AS
SET NOCOUNT ON
SELECT nCharNo FROM tCharacter (NOLOCK)
 WHERE nUserNo = @nUserNo AND bDeleted = 0
GO

CREATE PROCEDURE dbo.p_Char_GetListOfUserCharAll
    @nUserNo int
AS
SET NOCOUNT ON
SELECT nCharNo FROM tCharacter (NOLOCK) WHERE nUserNo = @nUserNo
GO

CREATE PROCEDURE dbo.p_Char_GetUserNo
    @nCharNo int
,   @nUserNo int OUTPUT
AS
SET NOCOUNT ON
SET @nUserNo = 0
SELECT @nUserNo = nUserNo FROM tCharacter WHERE nCharNo = @nCharNo
GO

CREATE PROCEDURE dbo.p_Char_GetCharNo
    @sID     nvarchar(40)
,   @nCharNo int OUTPUT
AS
SET NOCOUNT ON
SET @nCharNo = 0
SELECT @nCharNo = nCharNo FROM tCharacter WHERE sID = @sID
GO

CREATE PROCEDURE dbo.p_Char_GetLoginData
    @nCharNo int
AS
SET NOCOUNT ON
SELECT
    C.sID, C.nLevel, C.nSlotNo, C.sLoginZone, C.bDeleted, C.dDeletedDate,
    S.nRace, S.nClass, S.nGender, S.nHairType, S.nHairColor, S.nFaceShape,
    C.nKQHandle, C.sKQMap, C.nKQX, C.nKQY, C.nKQD, C.dKQDate
  FROM tCharacter      AS C
  INNER JOIN tCharacterShape AS S ON S.nCharNo = @nCharNo
 WHERE C.nCharNo = @nCharNo
GO

CREATE PROCEDURE dbo.p_Char_Login
    @nCharNo int
,   @nRet    int OUTPUT
AS
SET NOCOUNT ON
UPDATE tCharacter
   SET nLoginCount = nLoginCount + 1,
       dLastLoginDate = GETDATE()
 WHERE nCharNo = @nCharNo
SET @nRet = @@ROWCOUNT
GO

CREATE PROCEDURE dbo.usp_CharacterLoading
    @nCharNo         int
,   @dWorldTime      datetime
,   @sLoadingMapName nvarchar(16)
,   @nFailType       smallint
,   @nLoadingFailCnt smallint
,   @sFromMapName    nvarchar(16)
,   @nFromZoneIP     nvarchar(16)
,   @nToZoneIP       nvarchar(16)
,   @nToZonePort     smallint
,   @nSeqNoOutput    int OUTPUT
,   @nRet            int OUTPUT
AS
BEGIN
    SET NOCOUNT ON
    DECLARE @nSeqNo int
    SET @nSeqNo = 0

    BEGIN TRAN
    SELECT TOP 1 @nSeqNo = nSeqNo
      FROM tCharacterLoading
     WHERE nCharNo = @nCharNo
     ORDER BY nSeqNo DESC

    SET @nSeqNo = @nSeqNo + 1
    INSERT INTO tCharacterLoading
        (nCharNo, nSeqNo, dWorldTime, sLoadingMapName, nFailType, nLoadingFailCnt,
         sFromMapName, nFromZoneIP, nToZoneIP, nToZonePort)
    VALUES
        (@nCharNo, @nSeqNo, @dWorldTime, @sLoadingMapName, @nFailType, @nLoadingFailCnt,
         @sFromMapName, @nFromZoneIP, @nToZoneIP, @nToZonePort)
    IF @@ERROR <> 0 BEGIN
        SET @nRet = -1001
        ROLLBACK TRAN
        RETURN
    END

    SET @nSeqNoOutput = @nSeqNo
    SET @nRet = 0
    COMMIT TRAN
END
GO

CREATE PROCEDURE dbo.usp_CharacterLoadingDetailInfo
    @nCharNo      int
,   @nSeqNo       int
,   @nErrorDataNo smallint
,   @nErrorData   int
,   @nRet         int OUTPUT
AS
BEGIN
    SET NOCOUNT ON
    INSERT INTO tCharacterLoadingDetailInfo (nCharNo, nSeqNo, nErrorNo, nErrorData)
    VALUES                                  (@nCharNo, @nSeqNo, @nErrorDataNo, @nErrorData)
    IF @@ERROR <> 0 BEGIN
        SET @nRet = -1001
        RETURN
    END
    SET @nRet = 0
END
GO


/* =========================================================================
 * [World00_GameLog]  GAME EVENT LOGGING
 * ========================================================================= */

USE [World00_GameLog];
GO

CREATE PROCEDURE dbo.p_GameLog_Add
    @nLogNo        bigint OUTPUT
,   @nType         int
,   @nCharNo       int
,   @sMap          varchar(16)
,   @nMapX         int
,   @nMapY         int
,   @nMapZ         int
,   @nTargetCharNo int
,   @nTargetID     int
,   @nItemKey      bigint
,   @nInt1         int
,   @nInt2         int
,   @nInt3         int
,   @nBigint1      bigint
AS
SET NOCOUNT ON
SET @nLogNo = 0
INSERT tGameLog (nType, nCharNo, sMap, nMapX, nMapY, nMapZ, nTargetCharNo, nTargetID,
                 nItemKey, nInt1, nInt2, nInt3, nBigint1)
VALUES          (@nType, @nCharNo, @sMap, @nMapX, @nMapY, @nMapZ, @nTargetCharNo, @nTargetID,
                 @nItemKey, @nInt1, @nInt2, @nInt3, @nBigint1)
SET @nLogNo = @@IDENTITY
GO

CREATE PROCEDURE dbo.p_GameLog_Add_Type1
    @nLogNo  bigint OUTPUT
,   @nType   int
,   @nCharNo int
,   @sMap    varchar(16)
,   @nMapX   int
,   @nMapY   int
,   @nMapZ   int
AS
SET NOCOUNT ON
SET @nLogNo = 0
INSERT tGameLog (nType, nCharNo, sMap, nMapX, nMapY, nMapZ)
VALUES          (@nType, @nCharNo, @sMap, @nMapX, @nMapY, @nMapZ)
SET @nLogNo = @@IDENTITY
GO

CREATE PROCEDURE dbo.p_GameLog_Add_Type2
    @nLogNo  bigint OUTPUT
,   @nType   int
,   @nCharNo int
,   @sMap    varchar(16)
,   @nMapX   int
,   @nMapY   int
,   @nMapZ   int
,   @nInt1   int
,   @nInt2   int
AS
SET NOCOUNT ON
SET @nLogNo = 0
INSERT tGameLog (nType, nCharNo, sMap, nMapX, nMapY, nMapZ, nInt1, nInt2)
VALUES          (@nType, @nCharNo, @sMap, @nMapX, @nMapY, @nMapZ, @nInt1, @nInt2)
SET @nLogNo = @@IDENTITY
GO

CREATE PROCEDURE dbo.p_GameLog_Add_Type3
    @nLogNo        bigint OUTPUT
,   @nType         int
,   @nCharNo       int
,   @sMap          varchar(16)
,   @nMapX         int
,   @nMapY         int
,   @nMapZ         int
,   @nTargetCharNo int
,   @nTargetID     int
,   @nInt1         int
AS
SET NOCOUNT ON
SET @nLogNo = 0
INSERT tGameLog (nType, nCharNo, sMap, nMapX, nMapY, nMapZ, nTargetCharNo, nTargetID, nInt1)
VALUES          (@nType, @nCharNo, @sMap, @nMapX, @nMapY, @nMapZ, @nTargetCharNo, @nTargetID, @nInt1)
SET @nLogNo = @@IDENTITY
GO
