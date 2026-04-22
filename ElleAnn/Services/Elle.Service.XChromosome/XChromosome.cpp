/*******************************************************************************
 * XChromosome.cpp — Elle.Service.XChromosome
 *
 * Windows service wrapper around XEngine.
 *
 * IPC contract (all payloads are JSON strings, responses go out on
 * IPC_X_RESPONSE with the request's correlation_id preserved):
 *
 *   IPC_X_STATE_QUERY
 *     request  : {}
 *     reply    : XEngine::GetStateJson() output
 *
 *   IPC_X_HISTORY_QUERY
 *     request  : {"hours":24,"points":500}
 *     reply    : {"hours":..,"points":..,"series":[...]}
 *
 *   IPC_X_ANCHOR
 *     request  : {"day":1..N|0,"length":21..45|0,"strength":0..0.5|0}
 *     reply    : {"success":bool,"state":GetStateJson()}
 *
 *   IPC_X_STIMULUS
 *     request  : {"kind":"bonding|stress|intimacy|recall_positive|
 *                 recall_negative|activity",
 *                 "intensity":0..1,"notes":"..."}
 *     reply    : {"success":bool}
 *
 *   IPC_X_MODULATION_QUERY
 *     request  : {}
 *     reply    : {"warmth":F,"verbal_fluency":F,"empathy":F,
 *                 "introspection":F,"arousal":F,"fatigue":F,
 *                 "phase":"..." ,"cycle_day":N}
 *
 *   IPC_X_CONCEPTION_ATTEMPT
 *     request  : {"require_readiness":bool,"readiness_verified":bool}
 *     reply    : XConceptionAttemptResult as JSON
 *
 *   IPC_X_DELIVER (admin-only — normally triggered automatically by tick when
 *                  gestational_day >= gestational_length_days)
 *     request  : {}
 *     reply    : {"delivered":bool,"born_ms":N,"child_id":N}
 *
 * Broadcasts:
 *
 *   IPC_X_HORMONE_UPDATE — every tick, short snapshot
 *   IPC_X_PHASE_TRANSITION — when cycle phase crosses a boundary
 *   IPC_X_BIRTH — when Deliver() fires; HTTPServer forwards to WS clients
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleQueueIPC.h"
#include "../../Shared/json.hpp"
#include "XEngine.h"
#include <string>

using nlohmann::json;

/*──────────────────────────────────────────────────────────────────────────────
 * Provisional service / IPC opcode ids — kept local until they graduate into
 * Shared/ElleTypes.h. Matches the FamilyEngine reference pattern.
 *──────────────────────────────────────────────────────────────────────────────*/
#ifndef SVC_X_CHROMOSOME
#define SVC_X_CHROMOSOME        ((ELLE_SERVICE_ID)(ELLE_SERVICE_COUNT + 10))
#endif
#ifndef SVC_FAMILY
#define SVC_FAMILY              ((ELLE_SERVICE_ID)(ELLE_SERVICE_COUNT + 5))
#endif

#ifndef IPC_X_STATE_QUERY
#define IPC_X_STATE_QUERY             ((uint32_t)2200)
#define IPC_X_HISTORY_QUERY           ((uint32_t)2201)
#define IPC_X_ANCHOR                  ((uint32_t)2202)
#define IPC_X_STIMULUS                ((uint32_t)2203)
#define IPC_X_MODULATION_QUERY        ((uint32_t)2204)
#define IPC_X_CONCEPTION_ATTEMPT      ((uint32_t)2205)
#define IPC_X_DELIVER                 ((uint32_t)2206)
#define IPC_X_RESPONSE                ((uint32_t)2207)

#define IPC_X_HORMONE_UPDATE          ((uint32_t)2220)
#define IPC_X_PHASE_TRANSITION        ((uint32_t)2221)
#define IPC_X_BIRTH                   ((uint32_t)2222)
#endif

/* Family conception opcode (matches FamilyEngine reference). */
#ifndef IPC_FAMILY_CONCEPTION_ATTEMPT
#define IPC_FAMILY_CONCEPTION_ATTEMPT ((uint32_t)2110)
#endif

/*──────────────────────────────────────────────────────────────────────────────
 * JSON HELPERS
 *──────────────────────────────────────────────────────────────────────────────*/
static json ToJson(const XConceptionAttemptResult& r) {
    return json{
        {"success",               r.success},
        {"reason",                r.reason},
        {"in_fertile_window",     r.in_fertile_window},
        {"had_recent_intimacy",   r.had_recent_intimacy},
        {"readiness_verified",    r.readiness_verified},
        {"conceived_ms",          r.conceived_ms},
        {"due_ms",                r.due_ms}
    };
}

static json ToJson(const XEngine::DeliveryOutcome& d, int64_t child_id) {
    return json{
        {"delivered",        d.delivered},
        {"born_ms",          d.born_ms},
        {"gestational_days", d.gestational_days},
        {"child_id",         child_id}
    };
}

/*──────────────────────────────────────────────────────────────────────────────
 * X CHROMOSOME SERVICE
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleXChromosomeService : public ElleServiceBase {
public:
    ElleXChromosomeService()
        : ElleServiceBase(SVC_X_CHROMOSOME, "ElleXChromosome",
                          "Elle-Ann X Chromosome Engine",
                          "Cycle, hormones, pregnancy, and behavioural modulation") {}

protected:
    bool OnStart() override {
        if (!m_engine.Initialize()) {
            ELLE_ERROR("XEngine failed to initialize");
            return false;
        }
        /* Minute-level tick is plenty for a 28-day cycle. */
        SetTickInterval(60 * 1000);
        m_lastPhase = m_engine.GetCycle().phase;
        ELLE_INFO("XChromosome service started (day=%d phase=%s)",
                  m_engine.GetCycle().cycle_day,
                  XEngine::CyclePhaseName(m_lastPhase));
        return true;
    }

    void OnStop() override {
        ELLE_INFO("XChromosome service stopped");
    }

    void OnTick() override {
        m_engine.Tick();

        /* Phase-transition broadcast. */
        XCyclePhase now = m_engine.GetCycle().phase;
        if (now != m_lastPhase) {
            BroadcastPhaseTransition(m_lastPhase, now);
            m_lastPhase = now;
        }

        /* Short hormone update broadcast (every tick). */
        BroadcastHormoneUpdate();

        /* Auto-deliver if gestation is overdue. */
        auto preg = m_engine.GetPregnancy();
        if (preg.active && preg.gestational_day >= preg.gestational_length_days) {
            TriggerDelivery();
        }
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT };
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        switch (msg.header.msg_type) {
            case IPC_X_STATE_QUERY:        HandleStateQuery(msg, sender);       break;
            case IPC_X_HISTORY_QUERY:      HandleHistoryQuery(msg, sender);     break;
            case IPC_X_ANCHOR:             HandleAnchor(msg, sender);           break;
            case IPC_X_STIMULUS:           HandleStimulus(msg, sender);         break;
            case IPC_X_MODULATION_QUERY:   HandleModulationQuery(msg, sender);  break;
            case IPC_X_CONCEPTION_ATTEMPT: HandleConceptionAttempt(msg, sender);break;
            case IPC_X_DELIVER:            HandleDeliver(msg, sender);          break;
            default: break;
        }
    }

private:
    XEngine     m_engine;
    XCyclePhase m_lastPhase = X_PHASE_MENSTRUAL;

    /*────────────── request plumbing ──────────────*/
    bool ParseReq(const ElleIPCMessage& req, ELLE_SERVICE_ID sender, json& out) {
        const std::string s = req.GetStringPayload();
        if (s.empty()) { out = json::object(); return true; }
        try {
            out = json::parse(s);
            if (!out.is_object()) out = json::object();
            return true;
        } catch (const std::exception& e) {
            Reply(req, sender, json{
                {"success", false},
                {"error", std::string("invalid JSON: ") + e.what()}
            });
            return false;
        }
    }

    void Reply(const ElleIPCMessage& req, ELLE_SERVICE_ID sender, const json& body) {
        ElleIPCMessage resp = ElleIPCMessage::Create(IPC_X_RESPONSE, SVC_X_CHROMOSOME, sender);
        resp.header.correlation_id = req.header.correlation_id;
        resp.SetStringPayload(body.dump());
        GetIPCHub().Send(sender, resp);
    }

    /*────────────── handlers ──────────────*/
    void HandleStateQuery(const ElleIPCMessage& req, ELLE_SERVICE_ID sender) {
        json body; if (!ParseReq(req, sender, body)) return;
        Reply(req, sender, m_engine.GetStateJson());
    }

    void HandleHistoryQuery(const ElleIPCMessage& req, ELLE_SERVICE_ID sender) {
        json body; if (!ParseReq(req, sender, body)) return;
        uint32_t hours  = body.value("hours",  (uint32_t)24);
        uint32_t points = body.value("points", (uint32_t)500);
        auto pts = m_engine.GetHistory(hours, points);

        json series = json::array();
        for (auto& p : pts) {
            series.push_back({
                {"t",                p.taken_ms},
                {"cycle_day",        p.cycle_day},
                {"phase",            p.phase},
                {"estrogen",         p.hormones.estrogen},
                {"progesterone",     p.hormones.progesterone},
                {"testosterone",     p.hormones.testosterone},
                {"oxytocin",         p.hormones.oxytocin},
                {"serotonin",        p.hormones.serotonin},
                {"dopamine",         p.hormones.dopamine},
                {"cortisol",         p.hormones.cortisol},
                {"prolactin",        p.hormones.prolactin},
                {"hcg",              p.hormones.hcg},
                {"pregnancy_day",    p.pregnancy_day},
                {"pregnancy_phase",  p.pregnancy_phase}
            });
        }
        Reply(req, sender, json{
            {"hours",  hours},
            {"points", (int64_t)series.size()},
            {"series", series}
        });
    }

    void HandleAnchor(const ElleIPCMessage& req, ELLE_SERVICE_ID sender) {
        json body; if (!ParseReq(req, sender, body)) return;
        int   day      = body.value("day",      0);
        int   length   = body.value("length",   0);
        float strength = body.value("strength", 0.0f);
        bool ok = m_engine.AnchorCycle(day, length, strength);
        Reply(req, sender, json{
            {"success", ok},
            {"state",   m_engine.GetStateJson()}
        });
    }

    void HandleStimulus(const ElleIPCMessage& req, ELLE_SERVICE_ID sender) {
        json body; if (!ParseReq(req, sender, body)) return;
        XStimulus s;
        s.kind      = body.value("kind",      std::string());
        s.intensity = body.value("intensity", 0.5f);
        s.notes     = body.value("notes",     std::string());
        if (s.kind.empty()) {
            Reply(req, sender, json{{"success", false}, {"error", "missing 'kind'"}});
            return;
        }
        m_engine.ApplyStimulus(s);
        Reply(req, sender, json{{"success", true}});
    }

    void HandleModulationQuery(const ElleIPCMessage& req, ELLE_SERVICE_ID sender) {
        auto mod   = m_engine.ComputeModulation();
        auto cycle = m_engine.GetCycle();
        Reply(req, sender, json{
            {"warmth",         mod.warmth},
            {"verbal_fluency", mod.verbal_fluency},
            {"empathy",        mod.empathy},
            {"introspection",  mod.introspection},
            {"arousal",        mod.arousal},
            {"fatigue",        mod.fatigue},
            {"phase",          XEngine::CyclePhaseName(cycle.phase)},
            {"cycle_day",      cycle.cycle_day}
        });
    }

    void HandleConceptionAttempt(const ElleIPCMessage& req, ELLE_SERVICE_ID sender) {
        json body; if (!ParseReq(req, sender, body)) return;
        bool require_readiness   = body.value("require_readiness",   true);
        bool readiness_verified  = body.value("readiness_verified",  false);
        auto r = m_engine.AttemptConception(require_readiness, readiness_verified);
        Reply(req, sender, ToJson(r));
    }

    void HandleDeliver(const ElleIPCMessage& req, ELLE_SERVICE_ID sender) {
        int64_t childId = TriggerDelivery();
        auto d = m_engine.GetPregnancy();
        XEngine::DeliveryOutcome outStub{};
        outStub.delivered        = !d.active;
        outStub.born_ms          = d.updated_ms;
        outStub.gestational_days = (uint64_t)d.gestational_day;
        Reply(req, sender, ToJson(outStub, childId));
    }

    /*────────────── broadcasts ──────────────*/
    void BroadcastHormoneUpdate() {
        auto h = m_engine.GetHormones();
        auto c = m_engine.GetCycle();
        json payload = {
            {"cycle_day",    c.cycle_day},
            {"phase",        XEngine::CyclePhaseName(c.phase)},
            {"estrogen",     h.estrogen},
            {"progesterone", h.progesterone},
            {"oxytocin",     h.oxytocin},
            {"cortisol",     h.cortisol},
            {"dopamine",     h.dopamine},
            {"serotonin",    h.serotonin},
            {"hcg",          h.hcg}
        };
        auto msg = ElleIPCMessage::Create(IPC_X_HORMONE_UPDATE, SVC_X_CHROMOSOME,
                                          (ELLE_SERVICE_ID)0);
        msg.header.flags |= ELLE_IPC_FLAG_BROADCAST;
        msg.SetStringPayload(payload.dump());
        GetIPCHub().Broadcast(msg);
    }

    void BroadcastPhaseTransition(XCyclePhase from, XCyclePhase to) {
        json payload = {
            {"from", XEngine::CyclePhaseName(from)},
            {"to",   XEngine::CyclePhaseName(to)},
            {"cycle_day", m_engine.GetCycle().cycle_day}
        };
        auto msg = ElleIPCMessage::Create(IPC_X_PHASE_TRANSITION, SVC_X_CHROMOSOME,
                                          (ELLE_SERVICE_ID)0);
        msg.header.flags |= ELLE_IPC_FLAG_BROADCAST;
        msg.SetStringPayload(payload.dump());
        GetIPCHub().Broadcast(msg);
        ELLE_INFO("XChromosome phase %s → %s (day %d)",
                  XEngine::CyclePhaseName(from), XEngine::CyclePhaseName(to),
                  m_engine.GetCycle().cycle_day);
    }

    /* Fires the biological birth event:
     *   1. Calls XEngine::Deliver() to mark pregnancy complete.
     *   2. IPCs IPC_FAMILY_CONCEPTION_ATTEMPT to SVC_FAMILY with the current
     *      Elle/Arlo state so Family service creates the canonical child row.
     *   3. Broadcasts IPC_X_BIRTH so HTTPServer pushes a `world_event` frame
     *      to every WebSocket client.
     * Returns the child_id looked up from the Family tables (may be 0 if the
     * Family service isn't running or hasn't flushed yet — the subsequent
     * tick will see the update).                                            */
    int64_t TriggerDelivery() {
        auto d = m_engine.Deliver();
        if (!d.delivered) return 0;

        /* Ask the Family service to formally register the birth. */
        json famReq = {
            {"elle_state", json::object()},
            {"arlo_state", json::object()},
            {"origin",     "x_chromosome"},
            {"born_ms",    d.born_ms}
        };
        auto famMsg = ElleIPCMessage::Create(IPC_FAMILY_CONCEPTION_ATTEMPT,
                                             SVC_X_CHROMOSOME, SVC_FAMILY);
        famMsg.SetStringPayload(famReq.dump());
        GetIPCHub().Send(SVC_FAMILY, famMsg);

        /* Broadcast the birth event for Android. */
        json ev = {
            {"event",            "birth"},
            {"born_ms",          d.born_ms},
            {"gestational_days", d.gestational_days}
        };
        auto bMsg = ElleIPCMessage::Create(IPC_X_BIRTH, SVC_X_CHROMOSOME,
                                           (ELLE_SERVICE_ID)0);
        bMsg.header.flags |= ELLE_IPC_FLAG_BROADCAST;
        bMsg.SetStringPayload(ev.dump());
        GetIPCHub().Broadcast(bMsg);

        /* Also push a human-readable world event via HTTPServer so WS sees it. */
        json worldEv = {
            {"event",            "birth"},
            {"born_ms",          d.born_ms},
            {"gestational_days", d.gestational_days}
        };
        auto wsMsg = ElleIPCMessage::Create(IPC_WORLD_STATE, SVC_X_CHROMOSOME,
                                            SVC_HTTP_SERVER);
        wsMsg.SetStringPayload(worldEv.dump());
        GetIPCHub().Send(SVC_HTTP_SERVER, wsMsg);

        /* Try to look up the child_id the Family service just inserted.
         * This is best-effort — if Family isn't running we still succeed. */
        auto rs = ElleSQLPool::Instance().Query(
            "IF EXISTS (SELECT 1 FROM sys.tables t "
            "           JOIN sys.schemas s ON s.schema_id = t.schema_id "
            "           WHERE t.name = 'Children' AND s.name = 'dbo') "
            "SELECT TOP 1 ChildId FROM ElleHeart.dbo.Children ORDER BY ChildId DESC;");
        int64_t childId = 0;
        if (rs.success && !rs.rows.empty() && !rs.rows[0].IsNull(0)) {
            childId = (int64_t)rs.rows[0].GetInt(0);
        }

        /* Persist child_id back into pregnancy row if we got one. */
        if (childId > 0) {
            ElleSQLPool::Instance().QueryParams(
                "UPDATE ElleHeart.dbo.x_pregnancy_state "
                "   SET child_id = TRY_CAST(? AS BIGINT), updated_ms = TRY_CAST(? AS BIGINT) "
                " WHERE id = 1;",
                { std::to_string((long long)childId),
                  std::to_string((long long)ELLE_MS_NOW()) });
        }
        return childId;
    }
};

ELLE_SERVICE_MAIN(ElleXChromosomeService)
