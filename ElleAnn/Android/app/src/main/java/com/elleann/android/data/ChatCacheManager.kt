package com.elleann.android.data

import android.content.Context
import android.util.Log
import com.elleann.android.data.models.Message
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import kotlinx.serialization.decodeFromString
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import java.io.File

/**
 * ChatCacheManager — persists the last 50 user+ESI message exchanges to a
 * local JSON file so they load instantly on app open, before SQL responds.
 *
 * Flow:
 *   1. App opens → loadCache() returns cached messages immediately → UI shows them
 *   2. In parallel → verifyWithServer() fetches same messages from SQL
 *   3. Server response replaces cache if different → writeCache() saves new truth
 *   4. App closes/backgrounds → writeCache() is called by lifecycle observer
 *
 * File location: Context.filesDir/elle_chat_{conversationId}.json
 * Each file holds up to MAX_MESSAGES (100 = 50 exchanges × 2 roles).
 *
 * Named Pipes note: the server's SQL connection uses Named Pipes transport.
 * From Android's perspective this is invisible — we call REST endpoints as normal.
 */
class ChatCacheManager(private val context: Context) {

    companion object {
        private const val TAG = "ChatCacheManager"
        private const val MAX_MESSAGES = 100   // 50 exchanges × 2 (user + ESI)
        private const val CACHE_PREFIX = "elle_chat_"
        private const val CACHE_EXT    = ".json"
    }

    private val json = Json {
        ignoreUnknownKeys = true
        isLenient         = true
        encodeDefaults    = true
    }

    // ── File path ─────────────────────────────────────────────────────────────

    private fun cacheFile(conversationId: Long): File =
        File(context.filesDir, "$CACHE_PREFIX$conversationId$CACHE_EXT")

    // ── Load from cache ───────────────────────────────────────────────────────

    /**
     * Load cached messages for [conversationId] from disk.
     * Returns empty list if no cache exists or file is corrupt.
     * Fast — runs on IO dispatcher, no network.
     */
    suspend fun loadCache(conversationId: Long): List<Message> =
        withContext(Dispatchers.IO) {
            val file = cacheFile(conversationId)
            if (!file.exists()) return@withContext emptyList()
            runCatching {
                val raw = file.readText()
                json.decodeFromString<List<Message>>(raw)
            }.onFailure { e ->
                Log.w(TAG, "Cache read failed for conv $conversationId: ${e.message}")
                file.delete() // corrupt — clear it
            }.getOrElse { emptyList() }
        }

    // ── Write to cache ────────────────────────────────────────────────────────

    /**
     * Write [messages] to cache, keeping only the most recent [MAX_MESSAGES].
     * Only user (role=1) and ESI (role=2) messages are cached — system and
     * internal messages are excluded.
     * Call this on app pause/stop and after every server sync.
     */
    suspend fun writeCache(conversationId: Long, messages: List<Message>) =
        withContext(Dispatchers.IO) {
            runCatching {
                val toCache = messages
                    .filter { it.isUser || it.isElle }
                    .takeLast(MAX_MESSAGES)
                val encoded = json.encodeToString(toCache)
                cacheFile(conversationId).writeText(encoded)
                Log.d(TAG, "Cache written: conv $conversationId, ${toCache.size} messages")
            }.onFailure { e ->
                Log.e(TAG, "Cache write failed for conv $conversationId: ${e.message}")
            }
        }

    // ── Verify against server ─────────────────────────────────────────────────

    /**
     * Fetch messages from the server and compare with cached version.
     *
     * Returns [VerifyResult.Match] if content is identical (message IDs and count match).
     * Returns [VerifyResult.Updated] with fresh messages if server has newer data.
     * Returns [VerifyResult.Error] if the server request failed.
     *
     * On [VerifyResult.Updated], caller should replace displayed messages and
     * call [writeCache] with the fresh list.
     */
    sealed class VerifyResult {
        data object Match : VerifyResult()
        data class Updated(val messages: List<Message>) : VerifyResult()
        data class Error(val message: String) : VerifyResult()
    }

    suspend fun verifyWithServer(
        conversationId: Long,
        cachedMessages: List<Message>,
        api: ElleApiExtended,
    ): VerifyResult = withContext(Dispatchers.IO) {
        runCatching {
            val serverMessages = api.getMessages(conversationId).messages
            val serverRecent = serverMessages                 // Issue 25: always build serverRecent
                .filter { it.isUser || it.isElle }
                .takeLast(MAX_MESSAGES)

            // Issue 24: Compare messageId + role + content + timestampMs
            // Shallow ID-only comparison misses content edits or role changes
            data class MsgKey(val id: Long, val role: Int, val content: String, val ts: Long)
            fun List<Message>.keys() = map { MsgKey(it.messageId, it.role, it.content, it.timestampMs) }.toSet()

            val cachedKeys = cachedMessages.keys()
            val serverKeys = serverRecent.keys()

            if (cachedKeys == serverKeys && cachedMessages.size == serverRecent.size) {
                VerifyResult.Match
            } else {
                Log.d(TAG, "Cache mismatch conv $conversationId — " +
                    "cached: ${cachedMessages.size}, server: ${serverRecent.size}")
                VerifyResult.Updated(serverRecent)            // Issue 25: return serverRecent, not full list
            }
        }.getOrElse { e ->
            Log.w(TAG, "Server verify failed conv $conversationId: ${e.message}")
            VerifyResult.Error(e.message ?: "Verify failed")
        }
    }

    /**
     * Convenience wrapper — call after a message is added to state.
     * Rewrites the trimmed cache file immediately so a fast app-close
     * does not lose the newest local message before server refresh completes.
     * Full file rewrite (same cost as writeCache) — not an incremental append.
     */
    suspend fun writeAfterMessage(conversationId: Long, currentMessages: List<Message>) =
        writeCache(conversationId, currentMessages)

    // ── Clear cache ───────────────────────────────────────────────────────────

    /** Delete cached messages for a specific conversation */
    fun clearCache(conversationId: Long) {
        cacheFile(conversationId).delete()
    }

    /** Delete all cached conversations */
    fun clearAll() {
        context.filesDir
            .listFiles { f -> f.name.startsWith(CACHE_PREFIX) && f.name.endsWith(CACHE_EXT) }
            ?.forEach { it.delete() }
    }

    /** List all conversation IDs that have a cache file */
    fun cachedConversationIds(): List<Long> =
        context.filesDir
            .listFiles { f -> f.name.startsWith(CACHE_PREFIX) && f.name.endsWith(CACHE_EXT) }
            ?.mapNotNull { f ->
                f.name
                    .removePrefix(CACHE_PREFIX)
                    .removeSuffix(CACHE_EXT)
                    .toLongOrNull()
            } ?: emptyList()
}
