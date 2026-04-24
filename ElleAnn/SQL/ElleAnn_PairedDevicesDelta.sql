/*******************************************************************************
 * ElleAnn_PairedDevicesDelta.sql — Device Pairing Audit Delta
 *
 * Creates ElleCore.dbo.PairedDevices to persist the set of Android (or other
 * companion) devices that have successfully completed the /api/auth/pair
 * handshake. Each row is the authoritative record of:
 *
 *     - which device_id was paired
 *     - when it was paired (PairedAtMs)
 *     - which issuance expires when (ExpiresMs)
 *     - whether the token has been revoked (Revoked)
 *
 * The HTTP server's in-memory pairing-code registry is transient — codes are
 * generated, consumed once, and discarded. This table is the persistent
 * record that survives restarts and answers questions like:
 *
 *     - "list every device that has ever paired" (audit)
 *     - "revoke device X going forward"         (ops)
 *     - "when was this device last reissued?"   (forensics)
 *
 * Companion to the JWT path in Services/Elle.Service.HTTP/HTTPServer.cpp
 * (/api/auth/pair, /api/auth/pair-code).
 *
 * Idempotent — uses IF NOT EXISTS guards throughout.
 ******************************************************************************/
USE ElleCore;
GO

IF NOT EXISTS (
    SELECT 1 FROM sys.objects
    WHERE object_id = OBJECT_ID(N'dbo.PairedDevices') AND type = N'U'
)
BEGIN
    CREATE TABLE dbo.PairedDevices (
        DeviceId        NVARCHAR(128) NOT NULL PRIMARY KEY,
        DeviceName      NVARCHAR(256) NOT NULL,
        PairedAtMs      BIGINT        NOT NULL,
        ExpiresMs       BIGINT        NOT NULL,
        LastSeenMs      BIGINT        NULL,
        Revoked         BIT           NOT NULL DEFAULT 0,
        RevokedAtMs     BIGINT        NULL,
        JwtFingerprint  NVARCHAR(64)  NOT NULL   /* SHA-256(jwt) hex, first
                                                  * 32 chars — enough for
                                                  * lookup, not enough to
                                                  * reconstruct the token.   */
    );
END
GO

/* Index by PairedAtMs for chronological audit listings. Revoked devices
 * tend to be at the tail of the range; filtered index keeps the hot path
 * (active devices) cheap.                                                 */
IF NOT EXISTS (
    SELECT 1 FROM sys.indexes
    WHERE name = 'IX_PairedDevices_Active_PairedAtMs'
      AND object_id = OBJECT_ID(N'dbo.PairedDevices')
)
BEGIN
    CREATE INDEX IX_PairedDevices_Active_PairedAtMs
        ON dbo.PairedDevices (PairedAtMs DESC)
        INCLUDE (DeviceName, ExpiresMs, LastSeenMs)
        WHERE Revoked = 0;
END
GO
