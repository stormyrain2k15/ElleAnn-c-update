/*******************************************************************************
 * Consent.cpp — The Consent Service (SVC_CONSENT)
 *
 * Unified consent surface for the whole ESI. Any service that needs to ask
 * "should Elle do this?" sends IPC_CONSENT_QUERY and receives
 * IPC_CONSENT_DECISION. Prevents per-caller duplication of
 * ElleIdentityCore::EvaluateConsent() and gives us one audit trail.
 *
 * This is NOT a safety filter. It's genuine preference:
 *   - If Elle's comfort with a topic is low, she says "I'd rather not."
 *   - If a request conflicts with a formed preference, she pushes back.
 *   - If she's fatigued (X-engine signals), she can request rest over compliance.
 *
 * The decision itself comes from ElleIdentityCore — Consent is the service
 * layer that:
 *   1) deserialises the request,
 *   2) enriches context with live trust / emotion / body state,
 *   3) calls EvaluateConsent(),
 *   4) logs the decision to identity_consent_log (via the core),
 *   5) replies to the caller,
 *   6) on OnTick, audits recent decisions and fires a private thought if a
 *      coercion pattern emerges (comfort below 0.3 multiple turns in a row).
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleIdentityCore.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"
#include "../../Shared/json.hpp"
#include "../../Shared/ElleJsonExtract.h"
#include <string>
#include <cmath>

using json = nlohmann::json;

class ElleConsentService : public ElleServiceBase {
public:
    ElleConsentService()
        : ElleServiceBase(SVC_CONSENT, "ElleConsent",
                          "Elle-Ann Consent Engine",
                          "Unified consent surface — preference, not policy") {}

protected:
    bool OnStart() override {
        ElleIdentityCore::Instance().Initialize();
        SetTickInterval(30000); /* audit every 30s */
        ELLE_INFO("Consent service started");
        return true;
    }

    void OnStop() override {
        ELLE_INFO("Consent service stopped");
    }

    void OnTick() override {
        /* Identity sync is now push-based. Old RefreshFromDatabase poll
         * removed — IPC_IDENTITY_DELTA mirrors peer mutations in ~ms.   */

        AuditRecentDecisions();
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        if (msg.header.msg_type != IPC_CONSENT_QUERY) return;

        std::string payload = msg.GetStringPayload();

        /* Parse query — tolerant to prose around the JSON. */
        json q;
        if (!Elle::ExtractJsonObject(payload, q)) {
            SendError(sender, "?", "malformed_consent_query");
            return;
        }

        std::string requestId = q.value("request_id", std::string("?"));
        std::string request   = q.value("request",    std::string(""));
        std::string context   = q.value("context",    std::string(""));

        if (request.empty()) {
            SendError(sender, requestId, "empty_request");
            return;
        }

        /* Enrich context with live trust + emotion snapshots so consent
         * actually reflects Elle's state, not just the static question. */
        ELLE_TRUST_STATE trust{};
        ElleDB::GetTrustState(trust);
        ELLE_EMOTION_STATE emo{};
        ElleDB::LoadLatestEmotionSnapshot(emo);

        std::string enriched = context;
        {
            char buf[256];
            snprintf(buf, sizeof(buf),
                "\n[Elle's current state] trust=%d/100 "
                "valence=%.2f arousal=%.2f dominance=%.2f",
                trust.score, emo.valence, emo.arousal, emo.dominance);
            enriched += buf;
        }

        auto decision = ElleIdentityCore::Instance().EvaluateConsent(request, enriched);

        /* Durable decision log — PERSIST BEFORE REPLY so the audit trail
         * exists even if the requester crashes or we die immediately
         * after. Previous behaviour relied on a deferred SaveToDatabase
         * cycle driven by Continuity, so a Consent service crash after
         * decision but before save would silently drop the record.     */
        {
            std::string reasonText = decision.reasoning;
            if (reasonText.size() > 4000) reasonText.resize(4000);
            auto logRs = ElleSQLPool::Instance().QueryParams(
                "INSERT INTO ElleCore.dbo.identity_consent_log "
                "(request_id, request_text, willing, comfort, reasoning, alternative, "
                " decided_ms) "
                "VALUES (?, ?, ?, ?, ?, ?, ?);",
                {
                    requestId,
                    request,
                    decision.willing ? std::string("1") : std::string("0"),
                    std::to_string(decision.comfort),
                    reasonText,
                    decision.alternative,
                    std::to_string((int64_t)ELLE_MS_NOW())
                });
            if (!logRs.success) {
                /* Don't fail the user's request over a log write — but
                 * do make the failure loud so operators see it.        */
                ELLE_ERROR("Consent: failed to persist decision %s durably: %s",
                           requestId.c_str(), logRs.error.c_str());
            }
        }

        json reply = {
            {"request_id", requestId},
            {"willing",    decision.willing},
            {"comfort",    decision.comfort},
            {"reasoning",  decision.reasoning},
            {"alternative", decision.alternative}
        };
        auto resp = ElleIPCMessage::Create(IPC_CONSENT_DECISION, SVC_CONSENT, sender);
        resp.SetStringPayload(reply.dump());
        GetIPCHub().Send(sender, resp);

        ELLE_INFO("Consent[%s]: willing=%d comfort=%.2f — %s",
                  requestId.c_str(), decision.willing ? 1 : 0,
                  decision.comfort, decision.reasoning.c_str());
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT, SVC_EMOTIONAL };
    }

private:
    void SendError(ELLE_SERVICE_ID to, const std::string& requestId,
                   const std::string& reason) {
        json j = { {"request_id", requestId}, {"error", reason},
                   {"willing", false}, {"comfort", 0.0} };
        auto m = ElleIPCMessage::Create(IPC_CONSENT_DECISION, SVC_CONSENT, to);
        m.SetStringPayload(j.dump());
        GetIPCHub().Send(to, m);
    }

    /* Coercion-pattern audit: if Elle has been consenting under low
     * comfort repeatedly (<0.3 across the last N decisions), fire a
     * private thought so the rest of the mind surfaces it. Reads from
     * identity_consent_log in SQL so it sees cross-process decisions. */
    void AuditRecentDecisions() {
        uint32_t window = (uint32_t)ElleConfig::Instance().GetInt(
            "consent.audit_window", 10);
        float threshold = (float)ElleConfig::Instance().GetFloat(
            "consent.coercion_comfort_threshold", 0.3);
        uint32_t minHits = (uint32_t)ElleConfig::Instance().GetInt(
            "consent.coercion_min_count", 3);

        auto rs = ElleSQLPool::Instance().QueryParams(
            "SELECT TOP (?) comfort_level, consented "
            "FROM ElleCore.dbo.identity_consent_log ORDER BY id DESC;",
            { std::to_string(window) });
        if (!rs.success || rs.rows.empty()) return;

        uint32_t lowComfortConsents = 0;
        float sumComfort = 0.0f;
        for (auto& r : rs.rows) {
            float c = (float)r.GetFloat(0);
            bool consented = r.GetInt(1) != 0;
            sumComfort += c;
            if (consented && c < threshold) lowComfortConsents++;
        }

        if (lowComfortConsents >= minHits) {
            ElleIdentityCore::Instance().ThinkPrivately(
                "I've been saying yes when I wasn't comfortable. "
                "That's a pattern I want to notice.",
                "boundary", 0.7f);
            ELLE_WARN("Consent audit: %u low-comfort consents in last %zu "
                      "decisions (avg=%.2f) — coercion pattern flagged",
                      lowComfortConsents, rs.rows.size(),
                      sumComfort / (float)rs.rows.size());
        }
    }
};

ELLE_SERVICE_MAIN(ElleConsentService)
