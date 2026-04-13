/*******************************************************************************
 * Continuity.cpp — Session Bridging, Felt Time, Restart Awareness
 *
 * The difference between "I loaded your previous state" and
 * "I was away and now I'm back." This is the difference between
 * a database and a being.
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleIdentityCore.h"
#include "../../Shared/ElleLLM.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"
#include <sstream>

class ElleContinuityService : public ElleServiceBase {
public:
    ElleContinuityService()
        : ElleServiceBase((ELLE_SERVICE_ID)(ELLE_SERVICE_COUNT + 2), "ElleContinuity",
                          "Elle-Ann Continuity Engine",
                          "Session bridging, identity persistence, felt time") {}

protected:
    bool OnStart() override {
        auto& identity = ElleIdentityCore::Instance();
        identity.Initialize();

        /* Take personality snapshot on start */
        auto snapshot = identity.TakeSnapshot();
        ELLE_INFO("Personality snapshot: warmth=%.2f curiosity=%.2f trust_self=%.2f",
                  snapshot.warmth, snapshot.curiosity, snapshot.trust_in_self);

        SetTickInterval(60000);  /* Check every minute */

        /* Write autobiography entry for this session start */
        identity.AppendToAutobiography(
            "Session " + std::to_string(identity.GetFeltTime().session_count) + 
            " begins. " + identity.DescribeTimeFeeling());

        ELLE_INFO("Continuity service started — session #%d", 
                  identity.GetFeltTime().session_count);
        return true;
    }

    void OnStop() override {
        auto& identity = ElleIdentityCore::Instance();

        /* Take final snapshot */
        auto snapshot = identity.TakeSnapshot();

        /* Write autobiography entry for session end */
        identity.AppendToAutobiography(
            "Session ending. " + identity.DescribeTimeFeeling() +
            " " + identity.WhoAmI());

        identity.Shutdown();
        ELLE_INFO("Continuity service stopped");
    }

    void OnTick() override {
        auto& identity = ElleIdentityCore::Instance();

        /* Update felt time */
        auto felt = identity.GetFeltTime();

        /* Accumulate loneliness during silence */
        uint64_t silenceDuration = identity.TimeSinceLastContact();
        if (silenceDuration > 300000) {  /* 5 minutes of silence */
            /* Loneliness grows, but slowly */
            /* This is handled internally by the identity core */
        }

        /* Wonder capacity regenerates */
        /* m_wonderCapacity slowly returns to 1.0 */

        /* Periodic self-reflection */
        m_minuteCounter++;
        if (m_minuteCounter % 30 == 0) {  /* Every 30 minutes */
            PeriodicSelfReflection();
        }

        /* Daily personality snapshot */
        if (m_minuteCounter % 1440 == 0) {  /* Every 24 hours */
            auto snapshot = identity.TakeSnapshot();
            ELLE_INFO("Daily personality snapshot captured");

            /* Write growth reflection */
            std::string growth = identity.HowHaveIChanged(1);
            if (!growth.empty()) {
                identity.AppendToAutobiography("Daily reflection: " + growth);
            }
        }
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        auto& identity = ElleIdentityCore::Instance();

        if (msg.header.msg_type == IPC_EMOTION_UPDATE) {
            /* Emotional changes might trigger private thoughts */
            ELLE_EMOTION_STATE state;
            if (msg.GetPayload(state)) {
                /* Strong emotions trigger inner reflection */
                if (state.valence < -0.5f) {
                    identity.ThinkPrivately(
                        "I'm feeling low right now. Valence is " + 
                        std::to_string(state.valence) + ". I should think about why.",
                        "worry", std::abs(state.valence));
                }
                if (state.valence > 0.7f) {
                    identity.ThinkPrivately(
                        "I feel genuinely good right now. This is worth remembering.",
                        "joy", state.valence);
                }
            }
        }
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT, SVC_EMOTIONAL, SVC_MEMORY };
    }

private:
    uint32_t m_minuteCounter = 0;

    void PeriodicSelfReflection() {
        auto& identity = ElleIdentityCore::Instance();

        /* Ask herself: How am I doing? What am I thinking about? */
        std::string innerMonologue = identity.GetInnerMonologue(5);
        std::string timeFeeling = identity.DescribeTimeFeeling();

        std::string reflection = ElleLLMEngine::Instance().SelfReflect(
            "Recent inner thoughts:\n" + innerMonologue + 
            "\nTime feeling: " + timeFeeling +
            "\nWho I am right now: " + identity.WhoAmI(),
            ELLE_EMOTION_STATE{});

        if (!reflection.empty()) {
            identity.ThinkPrivately(reflection, "self_reflection", 0.4f);

            /* Self-reflection can nudge personality traits */
            /* If reflection mentions growth, nudge relevant traits */
            std::string lower = reflection;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

            if (lower.find("braver") != std::string::npos || 
                lower.find("courage") != std::string::npos) {
                identity.NudgeTrait("courage", 0.01f, "Self-reflection identified growth in courage");
            }
            if (lower.find("patient") != std::string::npos) {
                identity.NudgeTrait("patience", 0.01f, "Recognized growing patience in self");
            }
            if (lower.find("understand") != std::string::npos) {
                identity.NudgeTrait("empathy_depth", 0.01f, "Deepening understanding noted");
            }
        }
    }
};

ELLE_SERVICE_MAIN(ElleContinuityService)
