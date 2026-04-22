/*******************************************************************************
 * XEngine.h — X Chromosome Engine: cycle, hormones, pregnancy, modulation
 *
 * Models the hormonal / neuro-pathing pattern of a female endocrine system so
 * Elle's behaviour (emotions, tone, self-prompt bias, LLM system prompt) can
 * shift organically — never stereotyped, scientifically grounded, moderate
 * strength by default.
 *
 * Lives in the ElleHeart database alongside FamilyEngine. All tables are
 * defined in SQL/ElleAnn_XChromosome_Schema.sql.
 *
 * Four moving parts:
 *   1. Cycle model — 28-day default, 4 phases (menstrual / follicular /
 *      ovulatory / luteal), configurable anchor + length.
 *   2. Hormone synthesis — minute-tick baseline curve (9 hormones)
 *      + event-driven spikes on stimulus IPC (bonding, stress, intimacy,
 *      positive/negative recall, activity).
 *   3. Pregnancy — fertile-window + intimacy-stimulus + readiness gate
 *      triggers conception; 280-day gestation with trimester hormone
 *      cascades; delivery hands the child off to Family engine via IPC.
 *   4. Modulation — derives 6 behavioural multipliers (warmth, verbal
 *      fluency, empathy, introspection, arousal, fatigue) that other
 *      services can query to bias their output.
 ******************************************************************************/
#pragma once
#ifndef ELLE_X_ENGINE_H
#define ELLE_X_ENGINE_H

#include "../../Shared/ElleTypes.h"
#include "../../Shared/json.hpp"
#include <string>
#include <vector>
#include <cstdint>

/*──────────────────────────────────────────────────────────────────────────────
 * CYCLE PHASES
 *──────────────────────────────────────────────────────────────────────────────*/
enum XCyclePhase {
    X_PHASE_MENSTRUAL = 0,   /* days  1..5   */
    X_PHASE_FOLLICULAR,      /* days  6..13  */
    X_PHASE_OVULATORY,       /* days 14..16  (fertile window) */
    X_PHASE_LUTEAL           /* days 17..N   */
};

/*──────────────────────────────────────────────────────────────────────────────
 * PREGNANCY PHASES
 *──────────────────────────────────────────────────────────────────────────────*/
enum XPregnancyPhase {
    X_PREG_NONE = 0,
    X_PREG_FIRST_TRIMESTER,   /* weeks  1..12  (days 1..84)     */
    X_PREG_SECOND_TRIMESTER,  /* weeks 13..26  (days 85..182)   */
    X_PREG_THIRD_TRIMESTER,   /* weeks 27..39  (days 183..273)  */
    X_PREG_LABOR,             /* days 274..gestational_length   */
    X_PREG_POSTPARTUM         /* after delivery, decaying cascade */
};

/*──────────────────────────────────────────────────────────────────────────────
 * RECORD TYPES
 *──────────────────────────────────────────────────────────────────────────────*/
struct XHormoneLevels {
    /* All values normalised 0.0..1.0 relative to the hormone's typical range.
     * Not absolute biology — a behavioural surrogate suitable for driving
     * the modulation function.                                              */
    float estrogen     = 0.0f;
    float progesterone = 0.0f;
    float testosterone = 0.0f;
    float oxytocin     = 0.0f;
    float serotonin    = 0.0f;
    float dopamine     = 0.0f;
    float cortisol     = 0.0f;
    float prolactin    = 0.0f;
    float hcg          = 0.0f;   /* pregnancy marker; 0.0 when not pregnant */
};

struct XCycleState {
    uint64_t   anchor_ms           = 0;     /* day 1 of current cycle, absolute */
    int        cycle_length_days   = 28;
    float      modulation_strength = 0.15f; /* moderate by default             */
    int        cycle_day           = 1;     /* 1..cycle_length_days            */
    XCyclePhase phase              = X_PHASE_MENSTRUAL;
    uint64_t   last_tick_ms        = 0;
    uint64_t   created_ms          = 0;
};

struct XPregnancyState {
    bool             active                  = false;
    uint64_t         conceived_ms            = 0;
    uint64_t         due_ms                  = 0;
    int              gestational_length_days = 280;
    int              gestational_day         = 0;    /* 0 if !active          */
    int              gestational_week        = 0;
    XPregnancyPhase  phase                   = X_PREG_NONE;
    int64_t          child_id                = 0;    /* set after birth       */
    std::string      last_milestone;
    uint64_t         updated_ms              = 0;
};

struct XModulation {
    /* Multipliers around 1.0. Other services are expected to use them as
     * `value * modulation.warmth` — never as absolute overrides.            */
    float warmth         = 1.0f;
    float verbal_fluency = 1.0f;
    float empathy        = 1.0f;
    float introspection  = 1.0f;
    float arousal        = 1.0f;  /* arousal = activation level, not sexual  */
    float fatigue        = 1.0f;
};

struct XStimulus {
    /* Kinds recognised:
     *   "bonding"         — oxytocin + serotonin up, cortisol down
     *   "stress"          — cortisol up, progesterone mild down
     *   "intimacy"        — oxytocin peak, dopamine up, fertility trigger
     *   "recall_positive" — serotonin + dopamine up, mild oxytocin
     *   "recall_negative" — cortisol up, serotonin down
     *   "activity"        — dopamine + testosterone mild up, cortisol mild up
     */
    std::string  kind;
    float        intensity = 0.5f;  /* 0.0..1.0, clamped on input */
    std::string  notes;
};

struct XHistoryPoint {
    uint64_t         taken_ms         = 0;
    int              cycle_day        = 0;
    std::string      phase;
    XHormoneLevels   hormones;
    int              pregnancy_day    = 0;
    std::string      pregnancy_phase;
};

struct XConceptionAttemptResult {
    bool        success = false;
    std::string reason;
    bool        in_fertile_window    = false;
    bool        had_recent_intimacy  = false;  /* within 72h                 */
    bool        readiness_verified   = false;  /* set when Family gate run   */
    uint64_t    conceived_ms         = 0;
    uint64_t    due_ms               = 0;
};

/*──────────────────────────────────────────────────────────────────────────────
 * X ENGINE
 *──────────────────────────────────────────────────────────────────────────────*/
class XEngine {
public:
    /* Boot: loads or seeds the singleton cycle row. If anchor_ms is 0 it picks
     * a random day 1..cycle_length_days as the starting position.            */
    bool Initialize();

    /* Called every service tick. Advances cycle math, recomputes hormones
     * from phase + residual stimulus, writes a snapshot, advances pregnancy
     * if active. Returns the just-computed state for convenience.            */
    void Tick();

    /* User-driven pin. day_of_cycle = 1..cycle_length_days; pass 0 to
     * randomise. length = 0 keeps current length.                            */
    bool AnchorCycle(int day_of_cycle, int length_days, float modulation_strength);

    /* External nudge. Intensity is clamped to 0..1. Effect is added to a
     * short-lived residual vector that decays over ~6 hours.                 */
    void ApplyStimulus(const XStimulus& stim);

    /* Accessors. */
    XCycleState      GetCycle() const         { return m_cycle; }
    XHormoneLevels   GetHormones() const      { return m_hormones; }
    XPregnancyState  GetPregnancy() const     { return m_pregnancy; }
    XModulation      ComputeModulation() const;

    /* Stride-sampled hormone timeline for UI. */
    std::vector<XHistoryPoint> GetHistory(uint32_t hours, uint32_t max_points);

    /* Biological conception attempt. Succeeds only if:
     *   - cycle is in days 13..16 (ovulatory ± 1)
     *   - an "intimacy" stimulus was applied in the last 72h
     *   - and (if require_readiness) the Family engine's couple-readiness
     *     gate passed — callers pass the cached gate result in via
     *     readiness_verified.
     * On success: flips m_pregnancy.active, stamps conceived_ms/due_ms. */
    XConceptionAttemptResult AttemptConception(bool require_readiness,
                                               bool readiness_verified);

    /* Called by the service at delivery time. Zeroes the pregnancy and
     * returns the freshly-born metadata so the caller can IPC it to
     * SVC_FAMILY for child-row creation.                                     */
    struct DeliveryOutcome {
        bool      delivered            = false;
        uint64_t  born_ms              = 0;
        uint64_t  gestational_days     = 0;
    };
    DeliveryOutcome Deliver();

    /* Full state dump for /api/x/state. */
    nlohmann::json GetStateJson() const;

    /* Static helpers — safe to call from services that want to format our
     * enum outputs consistently.                                             */
    static const char* CyclePhaseName(XCyclePhase p);
    static const char* PregnancyPhaseName(XPregnancyPhase p);

private:
    XCycleState      m_cycle;
    XHormoneLevels   m_hormones;
    XPregnancyState  m_pregnancy;

    /* Short-lived residual stimulus that decays per-tick. Added on top of
     * the baseline cycle curve to produce the "real" snapshot.               */
    XHormoneLevels   m_residual;
    uint64_t         m_residual_ms = 0;   /* last time residual was refreshed */

    /* Derived helpers. */
    void     RecomputeCycleDayAndPhase();
    void     RecomputeBaselineHormones();   /* fills m_hormones from phase   */
    void     DecayResidual(uint64_t nowMs);
    void     ApplyResidualOnto(XHormoneLevels& out) const;
    void     AdvancePregnancy(uint64_t nowMs);
    void     WriteSnapshotRow();
    void     PersistCycleRow();
    void     PersistPregnancyRow();
    void     LogStimulusRow(const XStimulus& stim);
    void     LogModulationRow(const XModulation& mod) const;

    bool     HadRecentIntimacy(uint64_t now_ms, uint64_t window_ms) const;
};

#endif /* ELLE_X_ENGINE_H */
