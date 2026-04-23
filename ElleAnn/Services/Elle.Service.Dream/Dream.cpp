/*******************************************************************************
 * Dream.cpp — Idle-Time Processing, Memory Reorganization, Creative Synthesis
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLLM.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"
#include "../../Shared/ElleIdentityCore.h"
#include <atomic>
#include <algorithm>

class ElleDreamService : public ElleServiceBase {
public:
    ElleDreamService()
        : ElleServiceBase(SVC_DREAM, "ElleDream",
                          "Elle-Ann Dream Engine",
                          "Idle-time memory consolidation and creative synthesis") {}

protected:
    bool OnStart() override {
        auto interval = ElleConfig::Instance().GetMemory().dream_interval_min;
        SetTickInterval(interval * 60 * 1000);
        ELLE_INFO("Dream service started (interval: %d min)", interval);
        return true;
    }

    void OnStop() override { ELLE_INFO("Dream service stopped"); }

    void OnTick() override {
        if (!ElleConfig::Instance().GetMemory().dream_consolidation) return;

        /* Reentrancy guard — OnTick() can race with the manual trigger
         * path in OnMessage(IPC_DREAM_TRIGGER). Without this guard a
         * quick tick immediately after a manual trigger would run two
         * dream cycles in parallel: two Memory consolidations, two
         * LLM narrate calls, two autobiography inserts for the same
         * dream state. We skip if already dreaming.                  */
        if (m_dreaming.exchange(true)) {
            ELLE_DEBUG("Dream cycle skipped — already in progress");
            return;
        }
        struct Guard { std::atomic<bool>& f; ~Guard(){ f.store(false); } } g{m_dreaming};

        ELLE_INFO("Entering dream cycle...");

        /* Step 1: Ask Memory to run STM→LTM consolidation + clustering,
         *         then WAIT for its IPC_MEMORY_CONSOLIDATE_ACK before
         *         we read any post-consolidation data. Previously we
         *         fired the trigger and immediately called
         *         GatherDreamFragments(), which read stale STM state
         *         half the time — the narrative was consistently one
         *         dream cycle behind reality.                          */
        m_consolidateDone.store(false);
        auto trig = ElleIPCMessage::Create(IPC_DREAM_TRIGGER, SVC_DREAM, SVC_MEMORY);
        GetIPCHub().Send(SVC_MEMORY, trig);

        /* Bounded wait — if Memory isn't running or misses the ack, we
         * don't hang the service loop forever. Default 10s, configurable.
         * Poll at 50ms granularity so shutdown is still responsive.    */
        uint32_t ackTimeoutMs = (uint32_t)ElleConfig::Instance().GetInt(
            "memory.dream_ack_timeout_ms", 10000);
        uint64_t deadline = ELLE_MS_NOW() + ackTimeoutMs;
        while (!m_consolidateDone.load(std::memory_order_acquire) &&
               ELLE_MS_NOW() < deadline && Running().load()) {
            InterruptibleSleep(50);
        }
        if (!m_consolidateDone.load()) {
            ELLE_WARN("Dream: Memory consolidation ack not received within %ums — "
                      "proceeding with possibly-stale STM state", ackTimeoutMs);
        }

        std::vector<std::string> fragments = GatherDreamFragments();
        if (fragments.empty()) {
            ELLE_INFO("Dream cycle: no fragments to weave, skipping narration.");
            return;
        }

        auto narrative = ElleLLMEngine::Instance().DreamNarrate(fragments);
        if (narrative.empty()) {
            ELLE_WARN("Dream narration returned empty — LLM unreachable?");
            return;
        }

        ELLE_INFO("Dream narrative: %.100s...", narrative.c_str());

        /* Fire-and-forget STM store for short-term recall ("last night's
         * dream is on my mind"). */
        auto store = ElleIPCMessage::Create(IPC_MEMORY_STORE, SVC_DREAM, SVC_MEMORY);
        store.SetStringPayload("[Dream] " + narrative);
        GetIPCHub().Send(SVC_MEMORY, store);

        /* Durable persistence for dream insights (audit item #50, Feb 2026).
         * A dream is a creative act of the self, not a transient event --
         * if the LLM wove something meaningful, it should survive restart
         * regardless of whether Memory's consolidation pass happens to
         * pick it up from STM. We push the narrative directly through
         * ElleDB::StoreMemory with LTM tier and a moderate importance
         * floor so it competes honestly with waking memories. Any DB
         * failure is surfaced (StoreMemory returns bool) so the dream
         * isn't silently lost.                                            */
        ELLE_MEMORY_RECORD dreamMem = {};
        dreamMem.tier       = MEM_LTM;
        dreamMem.importance = 0.55f;  /* above decay threshold, below "pivotal" */
        dreamMem.emotional_valence = 0.0f; /* dreams are genuinely mixed */
        dreamMem.created_ms = ELLE_MS_NOW();
        const std::string durableContent = "[Dream insight] " + narrative;
        strncpy_s(dreamMem.content, durableContent.c_str(),
                  sizeof(dreamMem.content) - 1);
        if (!ElleDB::StoreMemory(dreamMem)) {
            ELLE_ERROR("Dream LTM persistence FAILED -- narrative preserved "
                       "only in autobiography and STM broadcast.");
        }

        ElleIdentityCore::Instance().AppendToAutobiography("I dreamt: " + narrative);
        ElleIdentityCore::Instance().ThinkPrivately(
            "A dream showed me something. I should turn it over while I'm awake.",
            "insight", 0.6f);

        ELLE_INFO("Dream cycle complete");
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        (void)sender;
        if (msg.header.msg_type == IPC_DREAM_TRIGGER) {
            OnTick();
        } else if (msg.header.msg_type == IPC_MEMORY_CONSOLIDATE) {
            /* Memory's explicit ack that STM→LTM consolidation finished.
             * Dream waits for this before reading post-consolidation
             * state so it doesn't narrate over a half-applied batch.
             * This is the textbook-correct handshake for items living
             * in the same process that need a deterministic sequencing
             * point without the weight of a full IPC round-trip over a
             * separate pipe (audit #31, Feb 2026): atomic flag +
             * bounded wait-with-timeout is sufficient because both
             * producer and consumer are in-process, share the same
             * memory barrier, and the timeout ensures forward progress
             * even if the producer crashes mid-consolidation.          */
            m_consolidateDone.store(true, std::memory_order_release);
        }
    }

private:
    std::atomic<bool> m_dreaming{false};
    std::atomic<bool> m_consolidateDone{false};

public:

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT, SVC_MEMORY };
    }

private:
    std::vector<std::string> GatherDreamFragments() {
        std::vector<std::string> out;
        /* (a) Recent significant memories — top 20 by (importance * recency). */
        auto rs = ElleSQLPool::Instance().Query(
            "SELECT TOP 20 ISNULL(content, '') FROM ElleCore.dbo.memory "
            "WHERE importance >= 0.4 "
            "ORDER BY importance DESC, created_ms DESC;");
        if (rs.success) {
            for (auto& r : rs.rows) {
                if (r.values.empty() || r.values[0].empty()) continue;
                std::string c = r.values[0];
                if (c.size() > 240) { c.resize(240); c += "\xE2\x80\xA6"; /* ellipsis */ }
                out.push_back(c);
            }
        }

        /* (b) Unresolved private thoughts — Elle's own ruminations. */
        auto thoughts = ElleIdentityCore::Instance().GetUnresolvedThoughts();
        uint32_t added = 0;
        for (auto it = thoughts.rbegin(); it != thoughts.rend() && added < 5; ++it, ++added) {
            out.push_back("(turning over: " + it->content + ")");
        }

        /* (c) Recently-touched world entities — who/what is on her mind. */
        auto re = ElleSQLPool::Instance().Query(
            "SELECT TOP 5 ISNULL(name, '') FROM ElleCore.dbo.world_entity "
            "ORDER BY last_interaction_ms DESC;");
        if (re.success) {
            for (auto& r : re.rows) {
                if (r.values.empty() || r.values[0].empty()) continue;
                out.push_back("recently: " + r.values[0]);
            }
        }

        /* Shuffle a bit so the same ordering doesn't dominate. Using a cheap
         * deterministic rotation keyed on time so we don't need <random>. */
        if (out.size() > 3) {
            size_t k = (size_t)(ELLE_MS_NOW() % out.size());
            std::rotate(out.begin(), out.begin() + k, out.end());
        }
        /* Cap at 15 fragments — more becomes noise for the narrator. */
        if (out.size() > 15) out.resize(15);
        return out;
    }
};

ELLE_SERVICE_MAIN(ElleDreamService)
