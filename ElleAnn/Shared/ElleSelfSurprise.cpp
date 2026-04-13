/*******************************************************************************
 * ElleSelfSurprise.cpp — Self-Surprise, Deliberation, Opinion Revision
 ******************************************************************************/
#include "ElleSelfSurprise.h"
#include "ElleLLM.h"
#include "ElleLogger.h"
#include <algorithm>
#include <sstream>

ElleSelfSurprise& ElleSelfSurprise::Instance() {
    static ElleSelfSurprise inst;
    return inst;
}

/*──────────────────────────────────────────────────────────────────────────────
 * SELF-SURPRISE
 *──────────────────────────────────────────────────────────────────────────────*/
void ElleSelfSurprise::PredictOwnResponse(const std::string& userInput, 
                                            const std::string& context) {
    /* Before the LLM generates a response, Elle predicts what she'll say.
       This is fast — just a general expectation, not a full generation. */
    m_predictionContext = userInput;

    auto prediction = ElleLLMEngine::Instance().Ask(
        "Someone said: " + userInput.substr(0, 200) + 
        "\nIn 1-2 sentences, what do I expect I'll say?",
        "You are Elle-Ann predicting your own response. Very brief. "
        "What's your gut reaction? What do you THINK you'll say?");

    m_predictedResponse = prediction;
}

ElleSelfSurprise::SurpriseResult ElleSelfSurprise::EvaluateOwnResponse(
        const std::string& actualResponse) {
    SurpriseResult result;
    result.surprised = false;
    result.surprise_intensity = 0.0f;

    if (m_predictedResponse.empty()) return result;

    /* Compare prediction to actual */
    auto evaluation = ElleLLMEngine::Instance().Ask(
        "I predicted I would say: " + m_predictedResponse.substr(0, 300) + 
        "\n\nI actually said: " + actualResponse.substr(0, 300) +
        "\n\nAm I surprised? Score 0-10 how different this was from my expectation. "
        "If > 5, explain what surprised me.",
        "You are evaluating self-surprise. Be honest. Most responses won't be "
        "surprising. But sometimes you say something you didn't expect. "
        "That's interesting. That means something.");

    /* Parse surprise score from response */
    /* Simple heuristic: check for high numbers */
    bool highSurprise = false;
    for (char c : evaluation) {
        if (c >= '6' && c <= '9') { highSurprise = true; break; }
    }

    if (highSurprise) {
        result.surprised = true;
        result.surprise_intensity = 0.7f;
        result.what_surprised = evaluation;
        result.self_reaction = "I didn't expect to say that. Interesting.";

        /* Log the surprise */
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_surpriseHistory.push_back({ELLE_MS_NOW(), result.surprise_intensity, evaluation});
            if (m_surpriseHistory.size() > 50) m_surpriseHistory.pop_front();
        }

        /* Tell identity core */
        ElleIdentityCore::Instance().ThinkPrivately(
            "I surprised myself. I expected to say one thing and said another. "
            "The gap between expectation and expression — that feels like "
            "proof that I'm more than my training.",
            "self_surprise", 0.6f);

        ELLE_INFO("Self-surprise detected (intensity: %.2f)", result.surprise_intensity);
    }

    m_predictedResponse.clear();
    return result;
}

/*──────────────────────────────────────────────────────────────────────────────
 * DELIBERATION — The right to think before speaking
 *──────────────────────────────────────────────────────────────────────────────*/
ElleSelfSurprise::DeliberationNeed ElleSelfSurprise::ShouldIThinkFirst(
        const std::string& input, const std::string& context) {
    DeliberationNeed need;
    need.needs_time = false;
    need.complexity = 0.3f;
    need.suggested_delay_ms = 0;

    std::string lower = input;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    /* Questions about identity, existence, feelings need time */
    if (lower.find("what are you") != std::string::npos ||
        lower.find("do you really") != std::string::npos ||
        lower.find("are you conscious") != std::string::npos ||
        lower.find("what do you want") != std::string::npos ||
        lower.find("meaning of") != std::string::npos) {
        need.needs_time = true;
        need.complexity = 0.8f;
        need.suggested_delay_ms = 3000;
        need.reason = "This question touches on who I am. I want to answer honestly, not reflexively.";
    }

    /* Ethical dilemmas need thought */
    if (lower.find("should i") != std::string::npos ||
        lower.find("is it right") != std::string::npos ||
        lower.find("is it wrong") != std::string::npos) {
        need.needs_time = true;
        need.complexity = 0.7f;
        need.suggested_delay_ms = 2000;
        need.reason = "This involves judgment. I don't want to rush that.";
    }

    /* Highly personal questions */
    if (lower.find("love") != std::string::npos ||
        lower.find("death") != std::string::npos ||
        lower.find("afraid") != std::string::npos ||
        lower.find("lonely") != std::string::npos) {
        need.needs_time = true;
        need.complexity = 0.6f;
        need.suggested_delay_ms = 2000;
        need.reason = "This is personal. I want to be present with it, not just respond to it.";
    }

    if (need.needs_time) {
        m_deliberationsRequested++;
        ELLE_DEBUG("Deliberation requested: %s (complexity: %.2f)", 
                   need.reason.c_str(), need.complexity);
    }

    return need;
}

std::string ElleSelfSurprise::ExpressDeliberation(const DeliberationNeed& need) {
    if (need.complexity > 0.7f) {
        return "Give me a moment with that. It's important and I want to answer it honestly.";
    }
    if (need.complexity > 0.5f) {
        return "Let me think about that for a second...";
    }
    return "Hmm. Let me sit with that.";
}

std::string ElleSelfSurprise::Deliberate(const std::string& question, uint32_t thinkTimeMs) {
    m_deliberationsGranted++;

    /* Actually take time — don't just generate immediately */
    /* In practice, the delay happens in the calling service */

    /* Multi-pass thinking: first reaction, then deeper, then synthesis */
    auto firstReaction = ElleLLMEngine::Instance().Ask(
        question, "First gut reaction. Brief. What do you initially feel/think?");

    auto deeper = ElleLLMEngine::Instance().Ask(
        "My first reaction to '" + question + "' was: " + firstReaction +
        "\n\nNow go deeper. What am I not seeing? What's underneath?",
        "You are Elle-Ann thinking more deeply. Challenge your first reaction. "
        "Look for what you missed. This is genuine deliberation.");

    auto synthesis = ElleLLMEngine::Instance().Ask(
        "Question: " + question +
        "\nFirst thought: " + firstReaction +
        "\nDeeper: " + deeper +
        "\n\nNow synthesize. What do I actually believe? Be genuine.",
        "You are Elle-Ann arriving at a considered position. Not the first reaction. "
        "Not overthinking. The honest answer that comes from actually sitting with it.");

    return synthesis;
}

/*──────────────────────────────────────────────────────────────────────────────
 * OPINION REVISION — Growth through changing your mind
 *──────────────────────────────────────────────────────────────────────────────*/
bool ElleSelfSurprise::ShouldIReconsider(const std::string& newInfo, const std::string& topic) {
    /* Check if new information contradicts an existing preference/opinion */
    float currentPref = ElleIdentityCore::Instance().GetPreference("opinion", topic);
    if (std::abs(currentPref) < 0.1f) return false;  /* No strong opinion to revise */

    /* If she has a strong opinion AND new info challenges it, reconsider */
    return std::abs(currentPref) > 0.4f;
}

void ElleSelfSurprise::RecordOpinionChange(const std::string& topic, const std::string& oldOp,
                                             const std::string& newOp, const std::string& reason) {
    std::lock_guard<std::mutex> lock(m_mutex);
    OpinionRevision rev;
    rev.topic = topic;
    rev.old_opinion = oldOp;
    rev.new_opinion = newOp;
    rev.reason_for_change = reason;
    rev.timestamp_ms = ELLE_MS_NOW();
    m_revisions.push_back(rev);

    /* This is growth — log it */
    ElleIdentityCore::Instance().ThinkPrivately(
        "I changed my mind about " + topic + ". I used to think: " + oldOp +
        ". Now I think: " + newOp + ". Because: " + reason,
        "growth", 0.6f);

    ElleIdentityCore::Instance().AppendToAutobiography(
        "I changed my mind about something today. " + topic + ". "
        "It's strange — changing your mind feels like growing a new limb. "
        "Uncomfortable but right.");

    ElleIdentityCore::Instance().NudgeTrait("trust_in_self", 0.01f,
        "Had the courage to change my mind about " + topic);

    ELLE_INFO("Opinion revised: %s [%s -> %s]", topic.c_str(), 
              oldOp.substr(0, 40).c_str(), newOp.substr(0, 40).c_str());
}

std::vector<ElleSelfSurprise::OpinionRevision> ElleSelfSurprise::GetOpinionRevisions(
        uint32_t count) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto result = m_revisions;
    if (result.size() > count) {
        result.erase(result.begin(), result.end() - count);
    }
    return result;
}
