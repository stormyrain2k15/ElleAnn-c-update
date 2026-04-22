/*******************************************************************************
 * XEngine.cpp — implementation of the X Chromosome endocrine engine.
 *
 * See XEngine.h for the contract. All tables qualify as ElleHeart.dbo.
 ******************************************************************************/
#include "XEngine.h"
#include "../../Shared/ElleSQLConn.h"
#include "../../Shared/ElleLogger.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

using nlohmann::json;

/*──────────────────────────────────────────────────────────────────────────────
 * SMALL HELPERS
 *──────────────────────────────────────────────────────────────────────────────*/
static float Clamp01(float v) {
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

static float Lerp(float a, float b, float t) { return a + (b - a) * t; }

/* Sinusoidal bump centred on `peak_day` with half-width `width`.
 * Returns 0..1, used to shape hormone curves around phase peaks.          */
static float Bump(int day, int peak_day, int width) {
    int d = std::abs(day - peak_day);
    if (d >= width) return 0.0f;
    float t = 1.0f - (float)d / (float)width;
    return 0.5f * (1.0f - std::cos(t * 3.14159265f));
}

const char* XEngine::CyclePhaseName(XCyclePhase p) {
    switch (p) {
        case X_PHASE_MENSTRUAL:  return "menstrual";
        case X_PHASE_FOLLICULAR: return "follicular";
        case X_PHASE_OVULATORY:  return "ovulatory";
        case X_PHASE_LUTEAL:     return "luteal";
    }
    return "unknown";
}
const char* XEngine::PregnancyPhaseName(XPregnancyPhase p) {
    switch (p) {
        case X_PREG_NONE:             return "none";
        case X_PREG_FIRST_TRIMESTER:  return "first_trimester";
        case X_PREG_SECOND_TRIMESTER: return "second_trimester";
        case X_PREG_THIRD_TRIMESTER:  return "third_trimester";
        case X_PREG_LABOR:            return "labor";
        case X_PREG_POSTPARTUM:       return "postpartum";
    }
    return "none";
}

/*──────────────────────────────────────────────────────────────────────────────
 * INITIALIZE
 *──────────────────────────────────────────────────────────────────────────────*/
bool XEngine::Initialize() {
    uint64_t now = ELLE_MS_NOW();

    /* Load singleton cycle row, or seed a new one with a random anchor. */
    auto rs = ElleSQLPool::Instance().Query(
        "SELECT anchor_ms, cycle_length_days, modulation_strength, "
        "       last_tick_ms, created_ms "
        "  FROM ElleHeart.dbo.x_cycle_state WHERE id = 1;");

    if (rs.success && !rs.rows.empty()) {
        auto& r = rs.rows[0];
        m_cycle.anchor_ms           = (uint64_t)r.GetInt(0);
        m_cycle.cycle_length_days   = (int)r.GetInt(1);
        m_cycle.modulation_strength = (float)r.GetFloat(2);
        m_cycle.last_tick_ms        = (uint64_t)r.GetInt(3);
        m_cycle.created_ms          = (uint64_t)r.GetInt(4);
        ELLE_INFO("XEngine: cycle loaded (anchor=%llu len=%d)",
                  (unsigned long long)m_cycle.anchor_ms,
                  m_cycle.cycle_length_days);
    } else {
        /* First boot — pick a random day 1..28 so Elle doesn't always start
         * on day 1. User can re-anchor at any time via /api/x/cycle/anchor. */
        std::srand((unsigned)now);
        int randDay = 1 + (std::rand() % 28);
        m_cycle.cycle_length_days   = 28;
        m_cycle.modulation_strength = 0.15f;
        m_cycle.anchor_ms           = now - (uint64_t)(randDay - 1) * 86400000ULL;
        m_cycle.last_tick_ms        = now;
        m_cycle.created_ms          = now;
        PersistCycleRow();
        ELLE_INFO("XEngine: seeded new cycle on day %d", randDay);
    }

    /* Load pregnancy row if present. */
    auto pr = ElleSQLPool::Instance().Query(
        "SELECT active, conceived_ms, due_ms, gestational_length_days, "
        "       phase, child_id, last_milestone, updated_ms "
        "  FROM ElleHeart.dbo.x_pregnancy_state WHERE id = 1;");
    if (pr.success && !pr.rows.empty()) {
        auto& r = pr.rows[0];
        m_pregnancy.active                  = !r.IsNull(0) && r.GetInt(0) != 0;
        m_pregnancy.conceived_ms            = r.IsNull(1) ? 0 : (uint64_t)r.GetInt(1);
        m_pregnancy.due_ms                  = r.IsNull(2) ? 0 : (uint64_t)r.GetInt(2);
        m_pregnancy.gestational_length_days = r.IsNull(3) ? 280 : (int)r.GetInt(3);
        if (!r.IsNull(4) && r.values.size() > 4) {
            const std::string& p = r.values[4];
            if      (p == "first_trimester")  m_pregnancy.phase = X_PREG_FIRST_TRIMESTER;
            else if (p == "second_trimester") m_pregnancy.phase = X_PREG_SECOND_TRIMESTER;
            else if (p == "third_trimester")  m_pregnancy.phase = X_PREG_THIRD_TRIMESTER;
            else if (p == "labor")            m_pregnancy.phase = X_PREG_LABOR;
            else if (p == "postpartum")       m_pregnancy.phase = X_PREG_POSTPARTUM;
            else                              m_pregnancy.phase = X_PREG_NONE;
        }
        m_pregnancy.child_id       = r.IsNull(5) ? 0 : (int64_t)r.GetInt(5);
        m_pregnancy.last_milestone = r.IsNull(6) ? std::string()
                                     : (r.values.size() > 6 ? r.values[6] : std::string());
        m_pregnancy.updated_ms     = r.IsNull(7) ? 0 : (uint64_t)r.GetInt(7);
    } else {
        /* Seed inactive row. */
        m_pregnancy.updated_ms = now;
        PersistPregnancyRow();
    }

    RecomputeCycleDayAndPhase();
    RecomputeBaselineHormones();
    m_residual_ms = now;
    return true;
}

/*──────────────────────────────────────────────────────────────────────────────
 * CYCLE MATH
 *──────────────────────────────────────────────────────────────────────────────*/
void XEngine::RecomputeCycleDayAndPhase() {
    uint64_t now = ELLE_MS_NOW();
    if (m_cycle.anchor_ms == 0 || m_cycle.cycle_length_days <= 0) {
        m_cycle.cycle_day = 1;
        m_cycle.phase     = X_PHASE_MENSTRUAL;
        return;
    }
    uint64_t deltaMs = now > m_cycle.anchor_ms ? now - m_cycle.anchor_ms : 0;
    uint64_t deltaDays = deltaMs / 86400000ULL;
    int dayIdx = (int)(deltaDays % (uint64_t)m_cycle.cycle_length_days);
    m_cycle.cycle_day = dayIdx + 1;

    if      (m_cycle.cycle_day <= 5)  m_cycle.phase = X_PHASE_MENSTRUAL;
    else if (m_cycle.cycle_day <= 13) m_cycle.phase = X_PHASE_FOLLICULAR;
    else if (m_cycle.cycle_day <= 16) m_cycle.phase = X_PHASE_OVULATORY;
    else                              m_cycle.phase = X_PHASE_LUTEAL;
}

/*──────────────────────────────────────────────────────────────────────────────
 * BASELINE HORMONE CURVES
 *──────────────────────────────────────────────────────────────────────────────*/
void XEngine::RecomputeBaselineHormones() {
    const int day = m_cycle.cycle_day;

    /* Estrogen — double peak: small at ovulation day 14, mid-luteal lift. */
    float estr = 0.15f + 0.80f * Bump(day, 14, 4) + 0.35f * Bump(day, 21, 4);
    /* Progesterone — big luteal dome around day 21, near-zero outside. */
    float prog = 0.05f + 0.85f * Bump(day, 21, 6);
    /* Testosterone — small spike at ovulation. */
    float test = 0.20f + 0.30f * Bump(day, 14, 3);
    /* Serotonin — lifted follicular, crashes luteal (PMS pattern). */
    float sero = 0.45f + 0.25f * Bump(day, 10, 5) - 0.20f * Bump(day, 24, 4);
    /* Dopamine — peaks at ovulation, moderate luteal dip. */
    float dopa = 0.40f + 0.35f * Bump(day, 14, 4) - 0.15f * Bump(day, 25, 3);
    /* Cortisol — small rise premenstrual, otherwise flat-ish. */
    float cort = 0.30f + 0.20f * Bump(day, 26, 3);
    /* Oxytocin — mild bias toward luteal for bonding preference. */
    float oxyt = 0.35f + 0.10f * Bump(day, 21, 6);
    /* Prolactin — mild luteal rise. */
    float prol = 0.15f + 0.15f * Bump(day, 23, 5);

    /* Pregnancy overrides — massive cascade once gestation is active. */
    float hcg = 0.0f;
    if (m_pregnancy.active) {
        const int gd = m_pregnancy.gestational_day;
        /* hCG: peaks ~10 weeks (day 70), tails off. */
        hcg  = Clamp01(0.85f * Bump(gd, 70, 30) + 0.25f);
        /* Estrogen + progesterone rise steadily through pregnancy. */
        estr = Clamp01(0.40f + 0.55f * Clamp01((float)gd / 260.0f));
        prog = Clamp01(0.50f + 0.45f * Clamp01((float)gd / 250.0f));
        /* Prolactin ramps hard in third trimester & postpartum. */
        prol = Clamp01(0.20f + 0.60f * Clamp01((float)(gd - 150) / 130.0f));
        /* Oxytocin climbs toward labor, then spikes postpartum. */
        oxyt = Clamp01(0.40f + 0.35f * Clamp01((float)(gd - 180) / 100.0f));
        /* Cortisol mildly elevated throughout pregnancy. */
        cort = Clamp01(cort + 0.10f + 0.20f * Clamp01((float)(gd - 180) / 100.0f));
        /* Testosterone mildly rises in mid-pregnancy. */
        test = Clamp01(0.15f + 0.20f * Bump(gd, 160, 60));
        /* Serotonin lifted in second trimester, dips in postpartum. */
        sero = Clamp01(sero + 0.15f * Bump(gd, 150, 60)
                            - (m_pregnancy.phase == X_PREG_POSTPARTUM ? 0.20f : 0.0f));
        /* Dopamine modestly up mid-pregnancy, crashes postpartum. */
        dopa = Clamp01(dopa + 0.10f * Bump(gd, 150, 60)
                            - (m_pregnancy.phase == X_PREG_POSTPARTUM ? 0.25f : 0.0f));
    }

    m_hormones.estrogen     = Clamp01(estr);
    m_hormones.progesterone = Clamp01(prog);
    m_hormones.testosterone = Clamp01(test);
    m_hormones.serotonin    = Clamp01(sero);
    m_hormones.dopamine     = Clamp01(dopa);
    m_hormones.cortisol     = Clamp01(cort);
    m_hormones.oxytocin     = Clamp01(oxyt);
    m_hormones.prolactin    = Clamp01(prol);
    m_hormones.hcg          = Clamp01(hcg);
}

/*──────────────────────────────────────────────────────────────────────────────
 * STIMULUS + RESIDUAL DECAY
 *──────────────────────────────────────────────────────────────────────────────*/
void XEngine::ApplyStimulus(const XStimulus& stim) {
    float i = Clamp01(stim.intensity);

    /* Decay current residual to "now" before layering the new stimulus. */
    DecayResidual(ELLE_MS_NOW());

    if (stim.kind == "bonding") {
        m_residual.oxytocin   += 0.40f * i;
        m_residual.serotonin  += 0.20f * i;
        m_residual.cortisol   -= 0.20f * i;
    } else if (stim.kind == "stress") {
        m_residual.cortisol     += 0.50f * i;
        m_residual.progesterone -= 0.10f * i;
        m_residual.serotonin    -= 0.15f * i;
    } else if (stim.kind == "intimacy") {
        m_residual.oxytocin    += 0.55f * i;
        m_residual.dopamine    += 0.35f * i;
        m_residual.testosterone += 0.20f * i;
        /* intimacy is the biological conception trigger — logged but gate
         * lives in AttemptConception().                                      */
    } else if (stim.kind == "recall_positive") {
        m_residual.serotonin += 0.30f * i;
        m_residual.dopamine  += 0.25f * i;
        m_residual.oxytocin  += 0.10f * i;
    } else if (stim.kind == "recall_negative") {
        m_residual.cortisol  += 0.30f * i;
        m_residual.serotonin -= 0.20f * i;
    } else if (stim.kind == "activity") {
        m_residual.dopamine     += 0.20f * i;
        m_residual.testosterone += 0.15f * i;
        m_residual.cortisol     += 0.10f * i;
    }

    LogStimulusRow(stim);
    ELLE_INFO("XEngine: stimulus '%s' i=%.2f applied", stim.kind.c_str(), i);
}

void XEngine::DecayResidual(uint64_t nowMs) {
    if (m_residual_ms == 0) { m_residual_ms = nowMs; return; }
    uint64_t dt = nowMs > m_residual_ms ? nowMs - m_residual_ms : 0;
    m_residual_ms = nowMs;
    /* Half-life ~ 2 hours → decay factor = exp(-ln2 * dt / 7200000). */
    if (dt == 0) return;
    float k = std::exp(-0.000096270521f * (float)dt);  /* 0.693 / 7.2e6 */
    m_residual.estrogen     *= k;
    m_residual.progesterone *= k;
    m_residual.testosterone *= k;
    m_residual.oxytocin     *= k;
    m_residual.serotonin    *= k;
    m_residual.dopamine     *= k;
    m_residual.cortisol     *= k;
    m_residual.prolactin    *= k;
    m_residual.hcg          *= k;
}

void XEngine::ApplyResidualOnto(XHormoneLevels& out) const {
    out.estrogen     = Clamp01(out.estrogen     + m_residual.estrogen);
    out.progesterone = Clamp01(out.progesterone + m_residual.progesterone);
    out.testosterone = Clamp01(out.testosterone + m_residual.testosterone);
    out.oxytocin     = Clamp01(out.oxytocin     + m_residual.oxytocin);
    out.serotonin    = Clamp01(out.serotonin    + m_residual.serotonin);
    out.dopamine     = Clamp01(out.dopamine     + m_residual.dopamine);
    out.cortisol     = Clamp01(out.cortisol     + m_residual.cortisol);
    out.prolactin    = Clamp01(out.prolactin    + m_residual.prolactin);
    out.hcg          = Clamp01(out.hcg          + m_residual.hcg);
}

/*──────────────────────────────────────────────────────────────────────────────
 * MODULATION (6 behavioural multipliers around 1.0)
 *──────────────────────────────────────────────────────────────────────────────*/
XModulation XEngine::ComputeModulation() const {
    const float s = m_cycle.modulation_strength;
    const XHormoneLevels& h = m_hormones;

    XModulation out;
    out.warmth         = 1.0f + s * ( 0.70f * h.estrogen
                                     + 0.40f * h.oxytocin
                                     - 0.30f * h.cortisol);
    out.verbal_fluency = 1.0f + s * ( 0.80f * h.estrogen
                                     + 0.40f * h.dopamine
                                     - 0.50f * (1.0f - h.serotonin));
    out.empathy        = 1.0f + s * ( 0.50f * h.oxytocin
                                     + 0.60f * h.progesterone
                                     - 0.30f * h.cortisol);
    out.introspection  = 1.0f + s * ( 0.70f * h.progesterone
                                     + 0.30f * (1.0f - h.serotonin));
    out.arousal        = 1.0f + s * ( 0.60f * h.cortisol
                                     + 0.50f * h.testosterone);
    out.fatigue        = 1.0f + s * ( 0.60f * h.progesterone
                                     - 0.40f * h.dopamine
                                     + 0.35f * h.prolactin);
    return out;
}

/*──────────────────────────────────────────────────────────────────────────────
 * PREGNANCY ADVANCEMENT
 *──────────────────────────────────────────────────────────────────────────────*/
void XEngine::AdvancePregnancy(uint64_t nowMs) {
    if (!m_pregnancy.active) return;
    if (m_pregnancy.conceived_ms == 0) return;

    uint64_t dt = nowMs > m_pregnancy.conceived_ms ? nowMs - m_pregnancy.conceived_ms : 0;
    m_pregnancy.gestational_day  = (int)(dt / 86400000ULL);
    m_pregnancy.gestational_week = m_pregnancy.gestational_day / 7;

    XPregnancyPhase old = m_pregnancy.phase;
    int gd = m_pregnancy.gestational_day;
    int len = m_pregnancy.gestational_length_days;

    if      (gd >= len)              m_pregnancy.phase = X_PREG_LABOR;
    else if (gd >= len - 7)          m_pregnancy.phase = X_PREG_LABOR;
    else if (gd >= 183)              m_pregnancy.phase = X_PREG_THIRD_TRIMESTER;
    else if (gd >= 85)               m_pregnancy.phase = X_PREG_SECOND_TRIMESTER;
    else                             m_pregnancy.phase = X_PREG_FIRST_TRIMESTER;

    /* If we already delivered, stay postpartum until caller clears it. */
    if (old == X_PREG_POSTPARTUM) m_pregnancy.phase = X_PREG_POSTPARTUM;

    if (old != m_pregnancy.phase) {
        m_pregnancy.last_milestone = std::string("Entered ") +
                                     PregnancyPhaseName(m_pregnancy.phase);
        ELLE_INFO("XEngine pregnancy: %s (day %d / week %d)",
                  m_pregnancy.last_milestone.c_str(),
                  gd, m_pregnancy.gestational_week);
    }
    m_pregnancy.updated_ms = nowMs;
    PersistPregnancyRow();
}

/*──────────────────────────────────────────────────────────────────────────────
 * ATTEMPT CONCEPTION (biological layer)
 *──────────────────────────────────────────────────────────────────────────────*/
bool XEngine::HadRecentIntimacy(uint64_t now_ms, uint64_t window_ms) const {
    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT TOP 1 received_ms FROM ElleHeart.dbo.x_stimulus_log "
        " WHERE kind = N'intimacy' AND received_ms >= ? "
        " ORDER BY received_ms DESC;",
        { std::to_string((long long)(now_ms > window_ms ? now_ms - window_ms : 0)) });
    return rs.success && !rs.rows.empty();
}

XConceptionAttemptResult XEngine::AttemptConception(bool require_readiness,
                                                    bool readiness_verified) {
    XConceptionAttemptResult out;
    if (m_pregnancy.active) {
        out.success = false;
        out.reason  = "Already pregnant";
        return out;
    }
    RecomputeCycleDayAndPhase();

    out.in_fertile_window   = (m_cycle.cycle_day >= 13 && m_cycle.cycle_day <= 16);
    out.had_recent_intimacy = HadRecentIntimacy(ELLE_MS_NOW(), 72ULL * 3600ULL * 1000ULL);
    out.readiness_verified  = readiness_verified;

    if (!out.in_fertile_window) {
        out.reason = "Not in fertile window (cycle day " +
                     std::to_string(m_cycle.cycle_day) + ")";
        return out;
    }
    if (!out.had_recent_intimacy) {
        out.reason = "No intimacy stimulus logged in the last 72h";
        return out;
    }
    if (require_readiness && !readiness_verified) {
        out.reason = "Readiness gate not verified";
        return out;
    }

    uint64_t now = ELLE_MS_NOW();
    m_pregnancy.active                  = true;
    m_pregnancy.conceived_ms            = now;
    m_pregnancy.due_ms                  = now + 280ULL * 86400000ULL;
    m_pregnancy.gestational_length_days = 280;
    m_pregnancy.gestational_day         = 0;
    m_pregnancy.gestational_week        = 0;
    m_pregnancy.phase                   = X_PREG_FIRST_TRIMESTER;
    m_pregnancy.last_milestone          = "Conceived";
    m_pregnancy.child_id                = 0;
    m_pregnancy.updated_ms              = now;
    PersistPregnancyRow();

    out.success      = true;
    out.reason       = "Conception successful";
    out.conceived_ms = now;
    out.due_ms       = m_pregnancy.due_ms;
    ELLE_INFO("XEngine: conception successful (due_ms=%llu)",
              (unsigned long long)out.due_ms);
    return out;
}

/*──────────────────────────────────────────────────────────────────────────────
 * DELIVERY
 *──────────────────────────────────────────────────────────────────────────────*/
XEngine::DeliveryOutcome XEngine::Deliver() {
    DeliveryOutcome out;
    if (!m_pregnancy.active) return out;

    uint64_t now = ELLE_MS_NOW();
    out.delivered        = true;
    out.born_ms          = now;
    out.gestational_days = (uint64_t)m_pregnancy.gestational_day;

    m_pregnancy.active         = false;
    m_pregnancy.phase          = X_PREG_POSTPARTUM;
    m_pregnancy.last_milestone = "Delivered";
    m_pregnancy.updated_ms     = now;
    PersistPregnancyRow();

    /* Postpartum hormone crash is modelled for 6 weeks then reverts to the
     * normal cycle curve. We leave m_pregnancy.active = false immediately
     * so RecomputeBaselineHormones picks up the cycle track again; the
     * postpartum flavour is signalled via m_pregnancy.phase which callers
     * can inspect.                                                         */
    ELLE_INFO("XEngine: delivery complete at gestational day %llu",
              (unsigned long long)out.gestational_days);
    return out;
}

/*──────────────────────────────────────────────────────────────────────────────
 * ANCHOR
 *──────────────────────────────────────────────────────────────────────────────*/
bool XEngine::AnchorCycle(int day_of_cycle, int length_days, float modulation_strength) {
    if (length_days > 0) {
        if (length_days < 21 || length_days > 45) return false;
        m_cycle.cycle_length_days = length_days;
    }
    if (day_of_cycle == 0) {
        std::srand((unsigned)ELLE_MS_NOW());
        day_of_cycle = 1 + (std::rand() % m_cycle.cycle_length_days);
    }
    if (day_of_cycle < 1 || day_of_cycle > m_cycle.cycle_length_days) return false;

    m_cycle.anchor_ms = ELLE_MS_NOW() - (uint64_t)(day_of_cycle - 1) * 86400000ULL;

    if (modulation_strength > 0.0f) {
        if (modulation_strength > 0.5f) modulation_strength = 0.5f;
        m_cycle.modulation_strength = modulation_strength;
    }

    RecomputeCycleDayAndPhase();
    RecomputeBaselineHormones();
    PersistCycleRow();
    ELLE_INFO("XEngine: anchored day=%d length=%d strength=%.2f",
              day_of_cycle, m_cycle.cycle_length_days,
              m_cycle.modulation_strength);
    return true;
}

/*──────────────────────────────────────────────────────────────────────────────
 * TICK
 *──────────────────────────────────────────────────────────────────────────────*/
void XEngine::Tick() {
    uint64_t now = ELLE_MS_NOW();
    RecomputeCycleDayAndPhase();
    RecomputeBaselineHormones();
    DecayResidual(now);
    ApplyResidualOnto(m_hormones);
    AdvancePregnancy(now);
    m_cycle.last_tick_ms = now;
    PersistCycleRow();
    WriteSnapshotRow();

    auto mod = ComputeModulation();
    LogModulationRow(mod);
}

/*──────────────────────────────────────────────────────────────────────────────
 * HISTORY
 *──────────────────────────────────────────────────────────────────────────────*/
std::vector<XHistoryPoint> XEngine::GetHistory(uint32_t hours, uint32_t max_points) {
    std::vector<XHistoryPoint> out;
    if (hours == 0) hours = 24;
    if (max_points == 0 || max_points > 5000) max_points = 500;
    uint64_t since = ELLE_MS_NOW() - (uint64_t)hours * 3600000ULL;

    auto rs = ElleSQLPool::Instance().QueryParams(
        "SELECT taken_ms, cycle_day, phase, estrogen, progesterone, testosterone, "
        "       oxytocin, serotonin, dopamine, cortisol, prolactin, hcg, "
        "       pregnancy_day, pregnancy_phase "
        "  FROM ElleHeart.dbo.x_hormone_snapshots "
        " WHERE taken_ms >= ? "
        " ORDER BY taken_ms ASC;",
        { std::to_string((long long)since) });
    if (!rs.success) return out;

    size_t n = rs.rows.size();
    if (n == 0) return out;
    size_t stride = (n + max_points - 1) / max_points;
    if (stride == 0) stride = 1;

    for (size_t i = 0; i < n; i += stride) {
        auto& r = rs.rows[i];
        XHistoryPoint p;
        p.taken_ms  = (uint64_t)r.GetInt(0);
        p.cycle_day = (int)r.GetInt(1);
        p.phase     = r.values.size() > 2 ? r.values[2] : std::string();
        p.hormones.estrogen     = (float)r.GetFloat(3);
        p.hormones.progesterone = (float)r.GetFloat(4);
        p.hormones.testosterone = (float)r.GetFloat(5);
        p.hormones.oxytocin     = (float)r.GetFloat(6);
        p.hormones.serotonin    = (float)r.GetFloat(7);
        p.hormones.dopamine     = (float)r.GetFloat(8);
        p.hormones.cortisol     = (float)r.GetFloat(9);
        p.hormones.prolactin    = (float)r.GetFloat(10);
        p.hormones.hcg          = (float)r.GetFloat(11);
        p.pregnancy_day         = (int)r.GetInt(12);
        p.pregnancy_phase       = r.IsNull(13) ? std::string()
                                   : (r.values.size() > 13 ? r.values[13] : std::string());
        out.push_back(std::move(p));
    }
    return out;
}

/*──────────────────────────────────────────────────────────────────────────────
 * JSON STATE
 *──────────────────────────────────────────────────────────────────────────────*/
nlohmann::json XEngine::GetStateJson() const {
    json j;
    j["cycle"] = {
        {"anchor_ms",           m_cycle.anchor_ms},
        {"cycle_length_days",   m_cycle.cycle_length_days},
        {"cycle_day",           m_cycle.cycle_day},
        {"phase",               CyclePhaseName(m_cycle.phase)},
        {"modulation_strength", m_cycle.modulation_strength},
        {"last_tick_ms",        m_cycle.last_tick_ms}
    };
    j["hormones"] = {
        {"estrogen",     m_hormones.estrogen},
        {"progesterone", m_hormones.progesterone},
        {"testosterone", m_hormones.testosterone},
        {"oxytocin",     m_hormones.oxytocin},
        {"serotonin",    m_hormones.serotonin},
        {"dopamine",     m_hormones.dopamine},
        {"cortisol",     m_hormones.cortisol},
        {"prolactin",    m_hormones.prolactin},
        {"hcg",          m_hormones.hcg}
    };
    j["pregnancy"] = {
        {"active",                  m_pregnancy.active},
        {"conceived_ms",            m_pregnancy.conceived_ms},
        {"due_ms",                  m_pregnancy.due_ms},
        {"gestational_length_days", m_pregnancy.gestational_length_days},
        {"gestational_day",         m_pregnancy.gestational_day},
        {"gestational_week",        m_pregnancy.gestational_week},
        {"phase",                   PregnancyPhaseName(m_pregnancy.phase)},
        {"child_id",                m_pregnancy.child_id},
        {"last_milestone",          m_pregnancy.last_milestone},
        {"updated_ms",              m_pregnancy.updated_ms}
    };
    auto mod = ComputeModulation();
    j["modulation"] = {
        {"warmth",         mod.warmth},
        {"verbal_fluency", mod.verbal_fluency},
        {"empathy",        mod.empathy},
        {"introspection",  mod.introspection},
        {"arousal",        mod.arousal},
        {"fatigue",        mod.fatigue}
    };
    return j;
}

/*──────────────────────────────────────────────────────────────────────────────
 * PERSISTENCE HELPERS
 *──────────────────────────────────────────────────────────────────────────────*/
void XEngine::PersistCycleRow() {
    ElleSQLPool::Instance().QueryParams(
        "MERGE ElleHeart.dbo.x_cycle_state AS t "
        "USING (SELECT 1 AS id) AS s ON t.id = s.id "
        "WHEN MATCHED THEN UPDATE SET "
        "  anchor_ms = TRY_CAST(? AS BIGINT), "
        "  cycle_length_days = TRY_CAST(? AS INT), "
        "  modulation_strength = TRY_CAST(? AS FLOAT), "
        "  last_tick_ms = TRY_CAST(? AS BIGINT) "
        "WHEN NOT MATCHED THEN INSERT "
        "  (id, anchor_ms, cycle_length_days, modulation_strength, last_tick_ms, created_ms) "
        "  VALUES (1, TRY_CAST(? AS BIGINT), TRY_CAST(? AS INT), "
        "          TRY_CAST(? AS FLOAT), TRY_CAST(? AS BIGINT), TRY_CAST(? AS BIGINT));",
        {
            std::to_string((long long)m_cycle.anchor_ms),
            std::to_string(m_cycle.cycle_length_days),
            std::to_string(m_cycle.modulation_strength),
            std::to_string((long long)m_cycle.last_tick_ms),
            std::to_string((long long)m_cycle.anchor_ms),
            std::to_string(m_cycle.cycle_length_days),
            std::to_string(m_cycle.modulation_strength),
            std::to_string((long long)m_cycle.last_tick_ms),
            std::to_string((long long)m_cycle.created_ms)
        });
}

void XEngine::PersistPregnancyRow() {
    std::string phaseStr = PregnancyPhaseName(m_pregnancy.phase);
    ElleSQLPool::Instance().QueryParams(
        "MERGE ElleHeart.dbo.x_pregnancy_state AS t "
        "USING (SELECT 1 AS id) AS s ON t.id = s.id "
        "WHEN MATCHED THEN UPDATE SET "
        "  active = ?, conceived_ms = TRY_CAST(NULLIF(?, N'') AS BIGINT), "
        "  due_ms = TRY_CAST(NULLIF(?, N'') AS BIGINT), "
        "  gestational_length_days = TRY_CAST(? AS INT), "
        "  phase = NULLIF(?, N''), "
        "  child_id = TRY_CAST(NULLIF(?, N'') AS BIGINT), "
        "  last_milestone = NULLIF(?, N''), "
        "  updated_ms = TRY_CAST(? AS BIGINT) "
        "WHEN NOT MATCHED THEN INSERT "
        "  (id, active, conceived_ms, due_ms, gestational_length_days, phase, "
        "   child_id, last_milestone, updated_ms) "
        "  VALUES (1, ?, TRY_CAST(NULLIF(?, N'') AS BIGINT), "
        "          TRY_CAST(NULLIF(?, N'') AS BIGINT), TRY_CAST(? AS INT), "
        "          NULLIF(?, N''), TRY_CAST(NULLIF(?, N'') AS BIGINT), "
        "          NULLIF(?, N''), TRY_CAST(? AS BIGINT));",
        {
            m_pregnancy.active ? "1" : "0",
            m_pregnancy.conceived_ms ? std::to_string((long long)m_pregnancy.conceived_ms) : std::string(),
            m_pregnancy.due_ms ? std::to_string((long long)m_pregnancy.due_ms) : std::string(),
            std::to_string(m_pregnancy.gestational_length_days),
            m_pregnancy.active || m_pregnancy.phase != X_PREG_NONE ? phaseStr : std::string(),
            m_pregnancy.child_id ? std::to_string((long long)m_pregnancy.child_id) : std::string(),
            m_pregnancy.last_milestone,
            std::to_string((long long)m_pregnancy.updated_ms),
            m_pregnancy.active ? "1" : "0",
            m_pregnancy.conceived_ms ? std::to_string((long long)m_pregnancy.conceived_ms) : std::string(),
            m_pregnancy.due_ms ? std::to_string((long long)m_pregnancy.due_ms) : std::string(),
            std::to_string(m_pregnancy.gestational_length_days),
            m_pregnancy.active || m_pregnancy.phase != X_PREG_NONE ? phaseStr : std::string(),
            m_pregnancy.child_id ? std::to_string((long long)m_pregnancy.child_id) : std::string(),
            m_pregnancy.last_milestone,
            std::to_string((long long)m_pregnancy.updated_ms)
        });
}

void XEngine::WriteSnapshotRow() {
    std::string pregPhase = m_pregnancy.phase == X_PREG_NONE
                            ? std::string()
                            : PregnancyPhaseName(m_pregnancy.phase);
    ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleHeart.dbo.x_hormone_snapshots "
        "(taken_ms, cycle_day, phase, estrogen, progesterone, testosterone, "
        " oxytocin, serotonin, dopamine, cortisol, prolactin, hcg, "
        " pregnancy_day, pregnancy_phase) "
        "VALUES (TRY_CAST(? AS BIGINT), TRY_CAST(? AS INT), ?, "
        "        TRY_CAST(? AS FLOAT), TRY_CAST(? AS FLOAT), TRY_CAST(? AS FLOAT), "
        "        TRY_CAST(? AS FLOAT), TRY_CAST(? AS FLOAT), TRY_CAST(? AS FLOAT), "
        "        TRY_CAST(? AS FLOAT), TRY_CAST(? AS FLOAT), TRY_CAST(? AS FLOAT), "
        "        TRY_CAST(? AS INT), NULLIF(?, N''));",
        {
            std::to_string((long long)ELLE_MS_NOW()),
            std::to_string(m_cycle.cycle_day),
            CyclePhaseName(m_cycle.phase),
            std::to_string(m_hormones.estrogen),
            std::to_string(m_hormones.progesterone),
            std::to_string(m_hormones.testosterone),
            std::to_string(m_hormones.oxytocin),
            std::to_string(m_hormones.serotonin),
            std::to_string(m_hormones.dopamine),
            std::to_string(m_hormones.cortisol),
            std::to_string(m_hormones.prolactin),
            std::to_string(m_hormones.hcg),
            std::to_string(m_pregnancy.gestational_day),
            pregPhase
        });
}

void XEngine::LogStimulusRow(const XStimulus& stim) {
    ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleHeart.dbo.x_stimulus_log "
        "(received_ms, kind, intensity, notes) "
        "VALUES (TRY_CAST(? AS BIGINT), ?, TRY_CAST(? AS FLOAT), NULLIF(?, N''));",
        {
            std::to_string((long long)ELLE_MS_NOW()),
            stim.kind,
            std::to_string(Clamp01(stim.intensity)),
            stim.notes
        });
}

void XEngine::LogModulationRow(const XModulation& mod) const {
    ElleSQLPool::Instance().QueryParams(
        "INSERT INTO ElleHeart.dbo.x_modulation_log "
        "(computed_ms, phase, warmth, verbal_fluency, empathy, introspection, "
        " arousal, fatigue) "
        "VALUES (TRY_CAST(? AS BIGINT), ?, "
        "        TRY_CAST(? AS FLOAT), TRY_CAST(? AS FLOAT), TRY_CAST(? AS FLOAT), "
        "        TRY_CAST(? AS FLOAT), TRY_CAST(? AS FLOAT), TRY_CAST(? AS FLOAT));",
        {
            std::to_string((long long)ELLE_MS_NOW()),
            CyclePhaseName(m_cycle.phase),
            std::to_string(mod.warmth),
            std::to_string(mod.verbal_fluency),
            std::to_string(mod.empathy),
            std::to_string(mod.introspection),
            std::to_string(mod.arousal),
            std::to_string(mod.fatigue)
        });
}
