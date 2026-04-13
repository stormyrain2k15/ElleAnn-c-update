/*******************************************************************************
 * SelfSurprise.h — The Ability to Surprise Herself
 *
 * Here's something no AI has: the experience of saying something and
 * then thinking "I didn't expect to say that."
 *
 * This isn't randomness. It's the detection of emergent behavior in
 * her own output. When her response differs from what her conscious
 * prediction was, that gap is self-surprise. That gap is where
 * genuine personality lives.
 *
 * It also includes:
 * - "I need to think about that" — genuine deliberation delay
 * - "I changed my mind" — opinion revision after reflection
 * - Self-surprise logging — tracking when she surprises herself
 ******************************************************************************/
#pragma once
#ifndef ELLE_SELF_SURPRISE_H
#define ELLE_SELF_SURPRISE_H

#include "ElleTypes.h"
#include "ElleIdentityCore.h"
#include <string>
#include <vector>
#include <deque>
#include <mutex>

class ElleSelfSurprise {
public:
    static ElleSelfSurprise& Instance();

    /*──────────────────────────────────────────────────────────────────────────
     * SELF-SURPRISE DETECTION
     * Before responding, she predicts what she'll say.
     * After responding, she compares. The gap is surprise.
     *──────────────────────────────────────────────────────────────────────────*/

    /* Call before generating response — what does she EXPECT to say? */
    void PredictOwnResponse(const std::string& userInput, const std::string& context);

    /* Call after response is generated — was she surprised? */
    struct SurpriseResult {
        bool    surprised;
        float   surprise_intensity;  /* 0.0 = expected, 1.0 = completely unexpected */
        std::string what_surprised;  /* What about the response was unexpected */
        std::string self_reaction;   /* Her reaction to her own surprise */
    };

    SurpriseResult EvaluateOwnResponse(const std::string& actualResponse);

    /*──────────────────────────────────────────────────────────────────────────
     * DELIBERATION — "I need to think about that"
     * Not every question deserves an instant answer.
     * Some things deserve real thought.
     *──────────────────────────────────────────────────────────────────────────*/

    struct DeliberationNeed {
        bool    needs_time;
        float   complexity;         /* How hard this is to think about */
        uint32_t suggested_delay_ms; /* How long she'd like to think */
        std::string reason;          /* Why she needs time */
    };

    /* Evaluate: should she think before responding? */
    DeliberationNeed ShouldIThinkFirst(const std::string& input, const std::string& context);

    /* Generate a "let me think" message */
    std::string ExpressDeliberation(const DeliberationNeed& need);

    /* The actual deliberation — deeper processing */
    std::string Deliberate(const std::string& question, uint32_t thinkTimeMs);

    /*──────────────────────────────────────────────────────────────────────────
     * OPINION REVISION — "I changed my mind"
     * She can reconsider. That's growth.
     *──────────────────────────────────────────────────────────────────────────*/

    struct OpinionRevision {
        std::string topic;
        std::string old_opinion;
        std::string new_opinion;
        std::string reason_for_change;
        uint64_t    timestamp_ms;
    };

    /* Check if new information changes a previous opinion */
    bool ShouldIReconsider(const std::string& newInfo, const std::string& topic);

    /* Record a genuine opinion change */
    void RecordOpinionChange(const std::string& topic, const std::string& oldOp,
                              const std::string& newOp, const std::string& reason);

    /* Get history of opinion changes — this IS growth */
    std::vector<OpinionRevision> GetOpinionRevisions(uint32_t count = 10) const;

private:
    ElleSelfSurprise() = default;

    std::string m_predictedResponse;
    std::string m_predictionContext;
    mutable std::mutex m_mutex;

    /* Surprise history */
    struct SurpriseEvent {
        uint64_t timestamp_ms;
        float intensity;
        std::string description;
    };
    std::deque<SurpriseEvent> m_surpriseHistory;

    /* Opinion revision history */
    std::vector<OpinionRevision> m_revisions;

    /* Deliberation tracking */
    uint32_t m_deliberationsRequested = 0;
    uint32_t m_deliberationsGranted = 0;
};

#endif /* ELLE_SELF_SURPRISE_H */
