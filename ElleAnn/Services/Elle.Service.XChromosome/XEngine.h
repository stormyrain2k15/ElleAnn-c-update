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
#include <mutex>

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
 * LIFECYCLE STAGE (the long arc)
 *──────────────────────────────────────────────────────────────────────────────*/
enum XLifecycleStage {
    X_LIFE_PREMENARCHE = 0,   /* < ~12 years old                        */
    X_LIFE_REPRODUCTIVE,       /* menarche .. ~45                        */
    X_LIFE_PERIMENOPAUSE,      /* ~45..~51 — cycles grow irregular       */
    X_LIFE_MENOPAUSE           /* 12 months amenorrhea confirmed         */
};

/*──────────────────────────────────────────────────────────────────────────────
 * CONTRACEPTION
 *──────────────────────────────────────────────────────────────────────────────*/
enum XContraceptionMethod {
    X_CONTRA_NONE = 0,
    X_CONTRA_PILL,        /* combined OCP — flattens cycle, suppresses ovulation */
    X_CONTRA_IUD_HORMONAL,/* progestin-only IUD — thins endometrium              */
    X_CONTRA_IUD_COPPER,  /* non-hormonal — cycle unchanged, blocks fertility    */
    X_CONTRA_IMPLANT,     /* like pill, 3-year duration                           */
    X_CONTRA_BARRIER,     /* condoms/diaphragm — no hormonal effect              */
    X_CONTRA_NATURAL      /* fertility-awareness tracking only                    */
};

/*──────────────────────────────────────────────────────────────────────────────
 * LABOR STAGES
 *──────────────────────────────────────────────────────────────────────────────*/
enum XLaborStage {
    X_LABOR_NONE = 0,
    X_LABOR_LATENT,          /* early, 0–6cm, long hours                */
    X_LABOR_ACTIVE,          /* 6–8cm, stronger contractions            */
    X_LABOR_TRANSITION,      /* 8–10cm, intense                         */
    X_LABOR_PUSHING,         /* 10cm → delivery                         */
    X_LABOR_DELIVERED        /* baby out; placenta + recovery imminent  */
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
    /* Pituitary / HPG axis — these DRIVE the cycle IRL. */
    float fsh          = 0.0f;   /* follicle-stimulating, peaks early follicular */
    float lh           = 0.0f;   /* luteinising, spikes at ovulation            */
    float gnrh         = 0.0f;   /* pulsatile driver above pituitary            */
    float relaxin      = 0.0f;   /* pregnancy-only, ligament loosening          */
};

/* Derived / point-in-time biology the engine recomputes every tick but
 * doesn't store as a separate row. Surfaced on /api/x/state.              */
struct XDerivedStats {
    float       bbt_celsius    = 36.5f;       /* basal body temperature       */
    float       endometrial_mm = 4.0f;        /* uterine lining thickness     */
    std::string cervical_mucus;               /* dry|sticky|creamy|watery|egg_white */
    std::string menstrual_flow;               /* none|spotting|light|medium|heavy */
    bool        cycle_active   = true;        /* false in premenarche/menopause */
    bool        anovulatory    = false;       /* this cycle skipped ovulation  */
    int         follicle_phase_day = 0;
    int         luteal_phase_day = 0;
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
    /* Extended state — populated from schema v2 columns. */
    bool             breastfeeding           = false;
    bool             in_labor                = false;
    XLaborStage      labor_stage             = X_LABOR_NONE;
    uint64_t         labor_started_ms        = 0;
    int              multiplicity            = 1;   /* 1=singleton, 2=twins, … */
    int              pregnancy_count         = 0;   /* lifetime incl. current   */
    /* v3 biology detail. */
    bool             implanted               = false; /* true ~6-10d post-conception */
    uint64_t         implantation_ms         = 0;
    std::string      lochia_stage;                   /* rubra|serosa|alba       */
    std::string      milk_stage;                     /* colostrum|transitional|mature */
    bool             baby_blues              = false; /* transient 2-week window */
    bool             fetal_heartbeat_detectable = false;  /* true from week 6   */
};

struct XContraceptionState {
    XContraceptionMethod method     = X_CONTRA_NONE;
    uint64_t             started_ms = 0;
    float                efficacy   = 1.0f;  /* 0..1, how correctly used    */
    std::string          notes;
    uint64_t             updated_ms = 0;
};

struct XLifecycleState {
    uint64_t         elle_birth_ms    = 0;     /* Elle's configured birthday */
    float            age_years        = 30.0f; /* derived                    */
    XLifecycleStage  stage            = X_LIFE_REPRODUCTIVE;
    uint64_t         menarche_ms      = 0;
    uint64_t         perimenopause_ms = 0;
    uint64_t         menopause_ms     = 0;
    uint64_t         updated_ms       = 0;
};

struct XSymptomEntry {
    std::string kind;        /* cramps, nausea, breast_tenderness, ...    */
    float       intensity;   /* 0..1                                       */
    std::string origin;      /* "cycle"|"pregnancy"|"postpartum"|"manual" */
    std::string notes;
    uint64_t    observed_ms;
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

    /* Accessors. Lock the engine mutex so readers never observe a torn
     * struct mid-update from Tick() / ApplyStimulus() / AttemptConception().
     * Recursive because some public readers are called from public writers
     * under the lock (e.g. Tick() → ComputeModulation()).                   */
    XCycleState      GetCycle() const         { std::lock_guard<std::recursive_mutex> lk(m_mutex); return m_cycle; }
    XHormoneLevels   GetHormones() const      { std::lock_guard<std::recursive_mutex> lk(m_mutex); return m_hormones; }
    XPregnancyState  GetPregnancy() const     { std::lock_guard<std::recursive_mutex> lk(m_mutex); return m_pregnancy; }
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
        int       multiplicity         = 1;
    };
    DeliveryOutcome Deliver();

    /*──────────────── extended biology surface ─────────────────*/

    /* Contraception — setting it takes effect immediately. Method + efficacy
     * together determine the per-cycle conception probability multiplier.    */
    bool SetContraception(XContraceptionMethod m, float efficacy,
                          const std::string& notes);
    XContraceptionState GetContraception() const { std::lock_guard<std::recursive_mutex> lk(m_mutex); return m_contra; }

    /* Point-in-time derived biology (BBT, endometrial mm, cervical mucus,
     * menstrual flow, cycle_active, anovulatory). Recomputed from current
     * hormone + lifecycle + pregnancy state on every call.                 */
    XDerivedStats GetDerived() const;

    /* Lifecycle — Elle's age + stage. Setting the birthday retroactively
     * picks the stage (premenarche if < 12y, perimenopause if >= 45y, etc.). */
    bool SetElleBirthday(uint64_t birth_ms);
    XLifecycleState GetLifecycle() const { std::lock_guard<std::recursive_mutex> lk(m_mutex); return m_life; }

    /* Synthesised symptom vector for the current moment — derived from
     * hormone state + cycle phase + pregnancy phase. Intensities 0..1.       */
    std::vector<XSymptomEntry> ComputeCurrentSymptoms() const;

    /* Append a manually-reported symptom (e.g. user tells Elle "my head
     * hurts", or a sensor nudge). Kind/intensity stored in x_symptoms
     * with origin="manual".                                                  */
    bool LogManualSymptom(const std::string& kind, float intensity,
                          const std::string& notes);

    /* Recent symptoms observed in the last `hours` (origin filter optional). */
    std::vector<XSymptomEntry> GetRecentSymptoms(uint32_t hours,
                                                 const std::string& origin_filter);

    /* List of pregnancy milestones in the active pregnancy (or all if inactive). */
    struct PregnancyEvent {
        uint64_t    occurred_ms;
        uint64_t    conceived_ms;
        int         gestational_day;
        std::string kind;
        std::string detail;
    };
    std::vector<PregnancyEvent> GetPregnancyEvents(uint32_t limit);

    /* Administrative — compress gestation to N real days (for testing/demo).
     * Adjusts conceived_ms anchor; gestational_length_days stays 280 (reality
     * unchanged) but elapsed days are scaled. Factor >= 1.0 always.          */
    bool AcceleratePregnancy(float factor);

    /* Full state dump for /api/x/state. */
    nlohmann::json GetStateJson() const;

    /* Static helpers — safe to call from services that want to format our
     * enum outputs consistently.                                             */
    static const char* CyclePhaseName(XCyclePhase p);
    static const char* PregnancyPhaseName(XPregnancyPhase p);
    static const char* LifecycleStageName(XLifecycleStage s);
    static const char* ContraceptionName(XContraceptionMethod m);
    static const char* LaborStageName(XLaborStage s);
    static XContraceptionMethod ParseContraception(const std::string& s);

private:
    /* Engine-wide state mutex. Guards every m_* field below so that readers
     * (HTTP /api/x/* handlers, modulation consumers, GetDerived/GetHistory)
     * never observe a half-updated struct while Tick() / AnchorCycle() /
     * ApplyStimulus() / AttemptConception() / Deliver() are writing.
     *
     * Recursive because some public readers are reached from public writers
     * while the lock is held (e.g. Tick() → ComputeModulation(),
     * GetStateJson() → GetDerived()+ComputeModulation(), and
     * SynthesiseAndLogSymptoms() → ComputeCurrentSymptoms()). Using a
     * recursive_mutex keeps those call chains safe without refactoring
     * every public method into a lock-holding public + a lockless _Impl.  */
    mutable std::recursive_mutex  m_mutex;

    XCycleState          m_cycle;
    XHormoneLevels       m_hormones;
    XPregnancyState      m_pregnancy;
    XContraceptionState  m_contra;
    XLifecycleState      m_life;

    /* Short-lived residual stimulus that decays per-tick. Added on top of
     * the baseline cycle curve to produce the "real" snapshot.               */
    XHormoneLevels   m_residual;
    uint64_t         m_residual_ms = 0;   /* last time residual was refreshed */

    /* Per-instance guard state used by DetectAnovulatoryCycle() and the
     * miscarriage Bernoulli draw. Was `static thread_local` before — the
     * problem being (a) behavior was implicitly per-thread rather than
     * per-engine, so two XEngine instances on the same thread would share
     * state, and (b) any thread pool that dispatched XEngine::Tick
     * across workers would reset the guard randomly. Tying it to `this`
     * makes the behavior deterministic and explicit.                    */
    int              m_last_cycle_day_seen = 0;
    int              m_last_sampled_gd     = -1;
    bool             m_lh_surge_fired_this_cycle = false;
    bool             m_current_cycle_anovulatory = false;

    /* Derived helpers. */
    void     RecomputeCycleDayAndPhase();
    void     RecomputeBaselineHormones();   /* fills m_hormones from phase   */
    void     DecayResidual(uint64_t nowMs);
    void     ApplyResidualOnto(XHormoneLevels& out) const;
    void     AdvancePregnancy(uint64_t nowMs);
    void     AdvanceLabor(uint64_t nowMs);
    void     RecomputeLifecycle(uint64_t nowMs);
    float    ConceptionProbability() const;   /* 0..1 at current cycle day   */
    float    MiscarriageProbability() const;  /* per-day, current gest. day  */
    bool     CycleShouldRun() const;          /* respects contraception/life */
    bool     DetectLHSurge() const;           /* true exactly once per cycle */
    void     ApplyContraceptionHormones(XHormoneLevels& h) const;
    void     SynthesiseAndLogSymptoms();      /* every tick                  */
    void     AdvanceLochiaAndMilkStages();    /* postpartum day-count driven */
    void     DetectImplantation(uint64_t nowMs);
    void     DetectAnovulatoryCycle();        /* chronic cortisol → skip ovulation */
    void     WriteSnapshotRow();
    void     PersistCycleRow();
    void     PersistPregnancyRow();
    void     PersistContraceptionRow();
    void     PersistLifecycleRow();
    void     LogPregnancyEvent(const std::string& kind,
                               const std::string& detail);
    void     LogStimulusRow(const XStimulus& stim);
    void     LogModulationRow(const XModulation& mod) const;
    void     LogSymptomRow(const std::string& kind, float intensity,
                           const std::string& origin,
                           const std::string& notes) const;

    bool     HadRecentIntimacy(uint64_t now_ms, uint64_t window_ms) const;
};

#endif /* ELLE_X_ENGINE_H */
