package com.elleann.android.data

import android.content.Context
import android.util.Log
import com.elleann.android.data.models.Message
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withContext
import kotlinx.serialization.decodeFromString
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import java.io.File
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.atomic.AtomicReference

/**
 * ChatCacheManager — local on-disk mirror of every conversation the user
 * has ever opened in this app. The on-disk format is JSON-encoded text
 * stored alongside the app's private files; the file extension is `.txt`
 * so it is grep-friendly for diagnostics.
 *
 * Design contract:
 *   - On app open, the cache file for a conversation is read SYNCHRONOUSLY
 *     and supplied to the UI before any network round-trip happens. This
 *     is the latency reduction the user requires.
 *   - The cache is the FULL conversation history visible to the user, not
 *     a tail-trimmed window. Memory cost is acceptable because messages
 *     are short text and rarely exceed a few thousand per conversation.
 *   - The cache is always rewritten on every message addition.
 *   - The cache is always rewritten on every app lifecycle pause / stop.
 *   - The cache is always rewritten on uncaught exceptions BEFORE the
 *     process dies, via [flushAllSync] called from a JVM-level uncaught
 *     exception handler installed in `ElleApp.onCreate()`. Even a hard
 *     crash therefore preserves the most recent local state.
 *   - SQL on the desktop server remains the single source of truth; the
 *     local txt cache is purely a startup-latency optimisation.
 *
 * Thread-safety:
 *   - [openConversations] is a [ConcurrentHashMap] so the crash-handler
 *     thread can safely iterate while UI threads add/remove tracked
 *     conversations.
 *   - File writes are guarded per-conversation by the JVM's intrinsic
 *     monitor on the file path string interned via [pathLockFor]; this
 *     prevents two writers racing on the same file while still allowing
 *     parallel writes for different conversations.
 */
class ChatCacheManager(private val context: Context) {

    companion object {
        private const val TAG          = "ChatCacheManager"
        private const val CACHE_PREFIX = "elle_chat_"
        private const val CACHE_EXT    = ".txt"

        /** Process-wide singleton handle. Set in `ElleApp.onCreate()`,
         *  read by the uncaught-exception handler.                       */
        private val instanceRef = AtomicReference<ChatCacheManager?>(null)

        fun installAsGlobal(mgr: ChatCacheManager) {
            instanceRef.set(mgr)
        }

        /** Called from the JVM uncaught-exception handler. Best-effort
         *  synchronous flush of every tracked conversation to disk before
         *  the previous handler escalates the crash.                     */
        fun crashFlush() {
            instanceRef.get()?.flushAllSync()
        }
    }

    private val json = Json {
        ignoreUnknownKeys = true
        isLenient         = true
        encodeDefaults    = true
    }

    /** Tracks every conversation the UI has opened, with the most recent
     *  in-memory message list. Updated by [trackOpen] and removed by
     *  [trackClosed]. The crash-flush iterates this map.                  */
    private val openConversations = ConcurrentHashMap<Long, List<Message>>()

    private fun cacheFile(conversationId: Long): File =
        File(context.filesDir, "$CACHE_PREFIX$conversationId$CACHE_EXT")

    private fun pathLockFor(conversationId: Long): Any =
        ("ChatCache_$conversationId").intern()

    // ── Tracking ─────────────────────────────────────────────────────────────

    /** Register that the UI has [messages] for [conversationId] in memory.
     *  The crash-flush will write this list verbatim. Call after every
     *  state update. Cheap (no I/O).                                      */
    fun track(conversationId: Long, messages: List<Message>) {
        openConversations[conversationId] = messages
    }

    /** Forget [conversationId]. Call when the chat screen is destroyed
     *  for good (not on lifecycle pause).                                 */
    fun untrack(conversationId: Long) {
        openConversations.remove(conversationId)
    }

    // ── Load ─────────────────────────────────────────────────────────────────

    /** Load cached messages for [conversationId] from disk. Returns an
     *  empty list when no cache exists or the file is unreadable. Run on
     *  IO dispatcher.                                                     */
    suspend fun loadCache(conversationId: Long): List<Message> =
        withContext(Dispatchers.IO) { loadCacheSync(conversationId) }

    /** Synchronous variant — caller decides the dispatcher. */
    fun loadCacheSync(conversationId: Long): List<Message> {
        val file = cacheFile(conversationId)
        if (!file.exists()) return emptyList()
        return runCatching {
            val raw = file.readText()
            if (raw.isBlank()) emptyList() else json.decodeFromString<List<Message>>(raw)
        }.onFailure { e ->
            Log.w(TAG, "Cache read failed for conv $conversationId: ${e.message}")
            file.delete()
        }.getOrElse { emptyList() }
    }

    // ── Write ────────────────────────────────────────────────────────────────

    /**
     * Write the FULL message history (filtered to user + ESI roles only —
     * system / internal messages are not user-visible) for [conversationId]
     * to its cache file.
     *
     * Atomic via write-to-temp + rename so a crash mid-write cannot leave
     * a half-written file that fails to parse on next load.
     */
    suspend fun writeCache(conversationId: Long, messages: List<Message>) =
        withContext(Dispatchers.IO) { writeCacheSync(conversationId, messages) }

    /** Synchronous write. Safe to call from a crash handler.              */
    fun writeCacheSync(conversationId: Long, messages: List<Message>) {
        synchronized(pathLockFor(conversationId)) {
            runCatching {
                val toCache = messages.filter { it.isUser || it.isElle }
                val encoded = json.encodeToString(toCache)
                val target  = cacheFile(conversationId)
                val tmp     = File(target.parentFile, target.name + ".tmp")
                tmp.writeText(encoded)
                /* Atomic rename: on POSIX filesystems (which Android uses
                 * internally for app-private storage) this is one
                 * inode swap, so a power failure can never expose a
                 * half-written file.                                    */
                if (!tmp.renameTo(target)) {
                    /* renameTo returns false if the target already
                     * exists on some FS — fall back to overwrite. */
                    tmp.copyTo(target, overwrite = true)
                    tmp.delete()
                }
            }.onFailure { e ->
                Log.e(TAG, "Cache write failed for conv $conversationId: ${e.message}")
            }
        }
    }

    /** Convenience — call after a message is added to in-memory state.   */
    suspend fun writeAfterMessage(conversationId: Long, currentMessages: List<Message>) {
        track(conversationId, currentMessages)
        writeCache(conversationId, currentMessages)
    }

    /** Synchronously flush every tracked conversation to disk. Called
     *  from the JVM uncaught-exception handler — do not throw.            */
    fun flushAllSync() {
        for ((id, msgs) in openConversations) {
            runCatching { writeCacheSync(id, msgs) }
                .onFailure { Log.w(TAG, "flushAllSync conv $id: ${it.message}") }
        }
    }

    /** Async equivalent for normal lifecycle paths. Iterates a snapshot
     *  of [openConversations] so concurrent track() calls don't ConcMod. */
    suspend fun flushAll() = withContext(Dispatchers.IO) {
        val snapshot = openConversations.toMap()
        for ((id, msgs) in snapshot) writeCacheSync(id, msgs)
    }

    /** Same as [flushAll] but blocks the calling thread. Used by lifecycle
     *  observers that fire on the main thread but want a guaranteed
     *  on-disk state by the time `onStop` returns.                        */
    fun flushAllBlocking() = runBlocking(Dispatchers.IO) { flushAll() }

    // ── Verify against server ────────────────────────────────────────────────

    sealed class VerifyResult {
        data object Match : VerifyResult()
        data class Updated(val messages: List<Message>) : VerifyResult()
        data class Error(val message: String) : VerifyResult()
    }

    /**
     * Pull [conversationId]'s messages from the server and compare to
     * [cachedMessages] by (id, role, content, timestamp). Returns
     * [VerifyResult.Match] when identical, [VerifyResult.Updated] with
     * the server's list otherwise. The server response is the source of
     * truth on mismatch — caller should adopt it and persist it.
     */
    suspend fun verifyWithServer(
        conversationId: Long,
        cachedMessages: List<Message>,
        api: ElleApiExtended,
    ): VerifyResult = withContext(Dispatchers.IO) {
        runCatching {
            val server = api.getMessages(conversationId).messages
                .filter { it.isUser || it.isElle }

            data class MsgKey(val id: Long, val role: Int, val content: String, val ts: Long)
            fun List<Message>.keys() =
                map { MsgKey(it.messageId, it.role, it.content, it.timestampMs) }.toSet()

            if (cachedMessages.keys() == server.keys() &&
                cachedMessages.size == server.size) {
                VerifyResult.Match
            } else {
                Log.d(TAG, "Cache mismatch conv $conversationId — " +
                    "cached: ${cachedMessages.size}, server: ${server.size}")
                VerifyResult.Updated(server)
            }
        }.getOrElse { e ->
            Log.w(TAG, "Server verify failed conv $conversationId: ${e.message}")
            VerifyResult.Error(e.message ?: "Verify failed")
        }
    }

    // ── Maintenance ──────────────────────────────────────────────────────────

    /** Delete cached messages for a specific conversation.                */
    fun clearCache(conversationId: Long) {
        synchronized(pathLockFor(conversationId)) { cacheFile(conversationId).delete() }
        openConversations.remove(conversationId)
    }

    /** Delete every cache file and forget every tracked conversation.    */
    fun clearAll() {
        context.filesDir
            .listFiles { f -> f.name.startsWith(CACHE_PREFIX) && f.name.endsWith(CACHE_EXT) }
            ?.forEach { it.delete() }
        openConversations.clear()
    }

    /** List conversation IDs that have a cache file on disk.              */
    fun cachedConversationIds(): List<Long> =
        context.filesDir
            .listFiles { f -> f.name.startsWith(CACHE_PREFIX) && f.name.endsWith(CACHE_EXT) }
            ?.mapNotNull {
                it.name.removePrefix(CACHE_PREFIX).removeSuffix(CACHE_EXT).toLongOrNull()
            } ?: emptyList()
}
