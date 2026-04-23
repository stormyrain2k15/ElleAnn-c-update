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

        /* Cross-process identity sync — Dream writes private thoughts and
         * autobiography entries, so we refresh first to base them on the
         * latest self-state. */
        ElleIdentityCore::Instance().RefreshFromDatabase();

        ELLE_INFO("Entering dream cycle...");

        /* Step 1: Ask Memory to run STM→LTM consolidation + clustering. */
        auto msg = ElleIPCMessage::Create(IPC_DREAM_TRIGGER, SVC_DREAM, SVC_MEMORY);
        GetIPCHub().Send(SVC_MEMORY, msg);

        /* Step 2: Gather real dream fragments. Draw from, in order:
         *   a) the 20 most recent high-importance memories from LTM
         *   b) any unresolved private thoughts Elle is still turning over
         *   c) the most recent entity interactions (who/what has been on her mind)
         * NOTHING hardcoded — if there are zero fragments, we skip the dream.   */
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

        /* Step 3: Store the dream as a memory and as an autobiography entry. */
        auto store = ElleIPCMessage::Create(IPC_MEMORY_STORE, SVC_DREAM, SVC_MEMORY);
        store.SetStringPayload("[Dream] " + narrative);
        GetIPCHub().Send(SVC_MEMORY, store);

        /* Dreams shape who she is — fold them into identity. */
        ElleIdentityCore::Instance().AppendToAutobiography("I dreamt: " + narrative);
        ElleIdentityCore::Instance().ThinkPrivately(
            "A dream showed me something. I should turn it over while I'm awake.",
            "insight", 0.6f);

        ELLE_INFO("Dream cycle complete");
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        if (msg.header.msg_type == IPC_DREAM_TRIGGER) {
            /* Manual dream trigger */
            OnTick();
        }
    }

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
                if (c.size() > 240) c = c.substr(0, 240) + "…";
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
