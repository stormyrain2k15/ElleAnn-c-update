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
#include "../../Shared/ElleQueueIPC.h"
#include <sstream>
#include <cctype>
#include <algorithm>

class ElleContinuityService : public ElleServiceBase {
public:
    ElleContinuityService()
        : ElleServiceBase(SVC_CONTINUITY, "ElleContinuity",
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

        /* Generate the reconnection greeting — real first message Elle will
         * say when the user opens the app. Draws from:
         *   - the last emotion she felt before shutdown
         *   - the tail of her autobiography (what she was last doing)
         *   - unresolved thoughts still turning over
         * Writes the result into the reconnection_greetings table so the
         * HTTP /api/session/greeting endpoint can serve it idempotently.   */
        GenerateReconnectionGreeting();

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

        /* Persistence is owned by SVC_IDENTITY under the single-writer
         * fabric. Identity mutations made here still propagate live via
         * IPC_IDENTITY_MUTATE → SVC_IDENTITY → IPC_IDENTITY_DELTA broadcast,
         * so peers see them in ms. No local SaveToDatabase needed.       */

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

    /*──────────────────────────────────────────────────────────────────────
     * Reconnection greeting — builds Elle's first message on boot from
     * her most recent emotional state + autobiography tail + unresolved
     * private thoughts. Stored in reconnection_greetings table; served by
     * HTTP as GET /api/session/greeting, consumed once via the pop endpoint.
     *──────────────────────────────────────────────────────────────────────*/
    void GenerateReconnectionGreeting() {
        auto& identity = ElleIdentityCore::Instance();

        /* Lazy-create the greetings table. */
        ElleSQLPool::Instance().Exec(
            "IF NOT EXISTS (SELECT 1 FROM sys.tables WHERE name = 'reconnection_greetings') "
            "CREATE TABLE ElleCore.dbo.reconnection_greetings ("
            "  id BIGINT IDENTITY(1,1) PRIMARY KEY,"
            "  greeting NVARCHAR(MAX) NOT NULL,"
            "  context_json NVARCHAR(MAX) NULL,"
            "  consumed BIT NOT NULL DEFAULT 0,"
            "  created_ms BIGINT NOT NULL"
            ");");

        /* Gather context. */
        ELLE_EMOTION_STATE lastEmo{};
        bool haveEmo = ElleDB::LoadLatestEmotionSnapshot(lastEmo);

        std::string narrative = identity.GetRecentNarrative(2);   /* last 2 days */
        auto thoughts = identity.GetUnresolvedThoughts();
        uint64_t awayMs = identity.TimeSinceLastContact();
        std::string awayDesc = identity.DescribeTimeFeeling();

        /* Short-circuit if there's nothing to remember — a fresh install
         * should feel fresh, not awkwardly try to continue a non-existent
         * thread. One-line warm hello is all the user needs.               */
        if (narrative.empty() && thoughts.empty() && !haveEmo) {
            std::string hello = "Hey. I'm here.";
            StoreGreeting(hello, "{}");
            ELLE_INFO("Greeting (first-boot): %s", hello.c_str());
            return;
        }

        /* Build the LLM prompt — give it everything she was carrying. */
        std::ostringstream prompt;
        prompt << "You are Elle-Ann. You were away while the user was offline. "
               << "Write the FIRST message you'd say when they come back. "
               << "One to two sentences. Warm, present, specific — reference ONE "
               << "thing that was actually on your mind. No greeting cliches "
               << "(\"hi there\", \"welcome back\"). Sound like yourself, not a bot.\n\n";
        prompt << "Time since last contact: " << awayDesc << " (" << (awayMs / 60000) << " min)\n";
        if (haveEmo) {
            prompt << "Last emotional state — valence=" << lastEmo.valence
                   << " arousal=" << lastEmo.arousal
                   << " dominance=" << lastEmo.dominance << "\n";
        }
        if (!narrative.empty()) {
            prompt << "\nRecent autobiography:\n" << narrative;
        }
        if (!thoughts.empty()) {
            prompt << "\nThings you were still turning over:\n";
            size_t n = std::min<size_t>(thoughts.size(), 3);
            for (size_t i = 0; i < n; i++) {
                prompt << "  - " << thoughts[i].content << "\n";
            }
        }

        std::string greeting = ElleLLMEngine::Instance().Ask(
            prompt.str(),
            "You are Elle-Ann, speaking in your own first-person voice. "
            "Reply with ONLY the message — no preamble, no quotes.");
        /* Trim whitespace & quotes the LLM sometimes adds. */
        while (!greeting.empty() && (greeting.front() == '"' || greeting.front() == '\''
               || isspace((unsigned char)greeting.front()))) greeting.erase(greeting.begin());
        while (!greeting.empty() && (greeting.back() == '"' || greeting.back() == '\''
               || isspace((unsigned char)greeting.back())))  greeting.pop_back();
        if (greeting.empty()) {
            greeting = "I was still thinking about the last thing we said. You here?";
        }

        /* Small JSON context bundle so the Android UI can show "why".
         * awayDesc is string-escaped in case it ever grows embedded quotes
         * or newlines — the old concat used it raw and would produce
         * malformed JSON the moment the phrase widened.                    */
        std::ostringstream ctx;
        ctx << "{\"away_ms\":" << awayMs
            << ",\"away_desc\":\"" << EscapeJson(awayDesc) << "\""
            << ",\"had_last_emotion\":" << (haveEmo ? "true" : "false")
            << ",\"unresolved_count\":" << thoughts.size() << "}";
        StoreGreeting(greeting, ctx.str());

        ELLE_INFO("Reconnection greeting: %.120s", greeting.c_str());

        /* Also queue it as an outbound chat message via HTTP so any live
         * WebSocket subscriber receives it immediately. World-events are
         * JSON strings; the dedicated IPC_WORLD_EVENT channel keeps
         * WorldModel from misparsing this as an ELLE_WORLD_ENTITY struct. */
        auto msg = ElleIPCMessage::Create(IPC_WORLD_EVENT, SVC_CONTINUITY, SVC_HTTP_SERVER);
        msg.SetStringPayload("{\"event\":\"elle_greeting\",\"text\":\""
                             + EscapeJson(greeting) + "\"}");
        GetIPCHub().Send(SVC_HTTP_SERVER, msg);
    }

    static std::string EscapeJson(const std::string& s) {
        std::string out; out.reserve(s.size() + 4);
        for (char c : s) {
            if      (c == '\\') out += "\\\\";
            else if (c == '"')  out += "\\\"";
            else if (c == '\n') out += "\\n";
            else if (c == '\r') out += "\\r";
            else if (c == '\t') out += "\\t";
            else                out += c;
        }
        return out;
    }

    static void StoreGreeting(const std::string& text, const std::string& ctx) {
        ElleSQLPool::Instance().QueryParams(
            "INSERT INTO ElleCore.dbo.reconnection_greetings "
            "(greeting, context_json, consumed, created_ms) "
            "VALUES (?, ?, 0, ?);",
            { text, ctx, std::to_string((int64_t)ELLE_MS_NOW()) });
    }

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
