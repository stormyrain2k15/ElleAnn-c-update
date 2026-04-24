package com.elleann.android.data

import android.content.Context
import androidx.security.crypto.EncryptedSharedPreferences
import androidx.security.crypto.MasterKey

/**
 * TokenStore — persistent, Keystore-wrapped JWT storage.
 *
 *   Backed by EncryptedSharedPreferences (AES256-GCM content,
 *   AES256-SIV keys). Recovered after app kill; cleared on revoke /
 *   401 / explicit user unpair.
 *
 *   Also holds the paired-server connection tuple (host + port) so
 *   every subsequent launch knows who to talk to without prompting.
 */
class TokenStore(context: Context) {

    private val masterKey = MasterKey.Builder(context)
        .setKeyScheme(MasterKey.KeyScheme.AES256_GCM)
        .build()

    private val prefs = EncryptedSharedPreferences.create(
        context,
        "elle_auth",
        masterKey,
        EncryptedSharedPreferences.PrefKeyEncryptionScheme.AES256_SIV,
        EncryptedSharedPreferences.PrefValueEncryptionScheme.AES256_GCM
    )

    data class Stored(
        val jwt: String,
        val expiresMs: Long,
        val host: String,
        val port: Int,
        val deviceId: String,
    )

    fun save(s: Stored) {
        prefs.edit()
            .putString("jwt", s.jwt)
            .putLong("expires_ms", s.expiresMs)
            .putString("host", s.host)
            .putInt("port", s.port)
            .putString("device_id", s.deviceId)
            .apply()
    }

    fun load(): Stored? {
        val jwt = prefs.getString("jwt", null) ?: return null
        val host = prefs.getString("host", null) ?: return null
        val port = prefs.getInt("port", -1).takeIf { it > 0 } ?: return null
        val deviceId = prefs.getString("device_id", null) ?: return null
        val exp = prefs.getLong("expires_ms", 0L)
        if (exp > 0 && exp < System.currentTimeMillis()) {
            /* Token already expired — treat as unpaired. */
            return null
        }
        return Stored(jwt, exp, host, port, deviceId)
    }

    fun clear() {
        prefs.edit().clear().apply()
    }

    /** Stable per-install UUID. Generated once, then persisted. */
    fun deviceId(): String {
        prefs.getString("stable_device_id", null)?.let { return it }
        val fresh = java.util.UUID.randomUUID().toString()
        prefs.edit().putString("stable_device_id", fresh).apply()
        return fresh
    }
}
