/*******************************************************************************
 * Dream.cpp — Idle-Time Processing, Memory Reorganization, Creative Synthesis
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLLM.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"

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

        ELLE_INFO("Entering dream cycle...");

        /* Trigger memory consolidation */
        auto msg = ElleIPCMessage::Create(IPC_DREAM_TRIGGER, SVC_DREAM, SVC_MEMORY);
        GetIPCHub().Send(SVC_MEMORY, msg);

        /* Generate dream narrative */
        auto narrative = ElleLLMEngine::Instance().DreamNarrate({
            "A conversation about meaning",
            "A moment of curiosity",
            "An unresolved question"
        });

        if (!narrative.empty()) {
            ELLE_INFO("Dream narrative: %.100s...", narrative.c_str());
            
            /* Store dream as memory */
            auto store = ElleIPCMessage::Create(IPC_MEMORY_STORE, SVC_DREAM, SVC_MEMORY);
            store.SetStringPayload("[Dream] " + narrative);
            GetIPCHub().Send(SVC_MEMORY, store);
        }

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
};

ELLE_SERVICE_MAIN(ElleDreamService)
