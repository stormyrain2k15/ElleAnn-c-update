/*******************************************************************************
 * ElleSelfSurprise.cpp — Self-Surprise, Deliberation, Opinion Revision
 ******************************************************************************/
#include "ElleSelfSurprise.h"
#include "ElleLLM.h"
#include "ElleLogger.h"
#include "ElleJsonExtract.h"
#include <algorithm>
#include <cctype>
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

    /* Compare prediction to actual. We ask for a STRUCTURED response so
     * we can parse a real score instead of eyeballing "is there a digit
     * between 6 and 9 anywhere in the prose" — which fired on any
     * mention of a year like 2026. The LLM is instructed to return a
     * small JSON object; we fall back to digit-scan only if parsing
     * fails (older local models, prose overflow, etc.).                 */
    auto evaluation = ElleLLMEngine::Instance().Ask(
        "I predicted I would say: " + m_predictedResponse.substr(0, 300) +
        "\n\nI actually said: " + actualResponse.substr(0, 300) +
        "\n\nReturn ONLY a JSON object with keys: "
        "\"score\" (integer 0-10; 0 = identical, 10 = wildly different), "
        "\"what_surprised\" (short string, may be empty), and "
        "\"reason\" (short string). No prose, no code fences.",
        "You are evaluating self-surprise. Be honest. Most responses won't be "
        "surprising. But sometimes you say something you didn't expect. "
        "Respond with a compact JSON object only.");

    float score = -1.0f;
    std::string what_surprised;
    /* Prefer structured JSON parse via ExtractJsonObject (already used
     * elsewhere for brace-balanced extraction out of chatty LLM replies). */
    nlohmann::json sj;
    if (Elle::ExtractJsonObject(evaluation, sj)) {
        if (sj.contains("score")) {
            if (sj["score"].is_number()) score = sj["score"].get<float>();
            else if (sj["score"].is_string()) {
                try { score = std::stof(sj["score"].get<std::string>()); }
                catch (const std::invalid_argument&) { score = -1.0f; }
                catch (const std::out_of_range&)     { score = -1.0f; }
                /* No catch(...) — std::stof only documents these two.
                 * Anything else is a real bug and should propagate.   */
            }
        }
        if (sj.contains("what_surprised") && sj["what_surprised"].is_string())
            what_surprised = sj["what_surprised"].get<std::string>();
    }
    /* Fallback: digit scan ONLY when structured parse failed. Restricted
     * to patterns that look like a score — " 7" or "7/10" or "7 out of 10"
     * — so a year like 2026 no longer trips a high-surprise event.      */
    if (score < 0.0f) {
        for (size_t i = 0; i + 1 < evaluation.size(); i++) {
            char c = evaluation[i];
            if (c < '0' || c > '9') continue;
            char next = evaluation[i + 1];
            /* Accept one-digit "7" followed by '/', ' ', '.', ',' or end. */
            bool plausible =
                (next == '/' || next == ' ' || next == '.' || next == ',' ||
                 next == ')' || next == '\n' || next == '\r' || next == '\0');
            /* Must be at start or preceded by non-digit (so "2026" doesn't
             * report "2", "0", "2", "6"). */
            bool standalone = (i == 0) ||
                              (evaluation[i-1] < '0' || evaluation[i-1] > '9');
            if (plausible && standalone) {
                score = (float)(c - '0');
                break;
            }
        }
    }

    bool highSurprise = score >= 6.0f;

    if (highSurprise) {
        result.surprised = true;
        result.surprise_intensity = std::min(1.0f, score / 10.0f);
        result.what_surprised =
            !what_surprised.empty() ? what_surprised : evaluation;
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

        ELLE_INFO("Self-surprise detected (score=%.1f, intensity=%.2f)",
                  score, result.surprise_intensity);
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
    /* Wrap ::tolower in a lambda that returns char (not int) — passing
     * ::tolower directly to std::transform instantiates the algorithm
     * with an int→char assignment inside the STL, which trips C4244
     * at <algorithm>. The unsigned-char cast in is also correct per the
     * cctype contract (undefined on char < 0).                         */
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });

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
    /* Feb 2026 audit: the previous implementation ignored `newInfo`
     * entirely and only inspected the magnitude of the current
     * opinion. That made the function lie about its contract --
     * "should I reconsider in light of this new info?" became "do I
     * have a strong opinion?". We now use both:
     *   1. No strong opinion (<0.1 magnitude)       -> nothing to revise.
     *   2. Weak opinion (<0.4 magnitude)            -> don't churn on minor input.
     *   3. Strong opinion AND newInfo is meaningful -> reconsider.
     *      "Meaningful" is bounded below by a minimum length and
     *      above by "the topic itself (or a stem of it) appears in
     *      newInfo", so a random sentence about the weather can't
     *      trigger reconsideration of an unrelated belief.          */
    if (newInfo.empty() || topic.empty()) return false;

    float currentPref = ElleIdentityCore::Instance().GetPreference("opinion", topic);
    if (std::abs(currentPref) < 0.1f) return false;  /* no strong opinion to revise */
    if (std::abs(currentPref) < 0.4f) return false;  /* weak opinion -- don't churn */

    /* Require some substance so a one-word response can't trigger a
     * belief revision cascade.                                        */
    constexpr size_t kMinNewInfoChars = 24;
    if (newInfo.size() < kMinNewInfoChars) return false;

    /* Topic relevance check, case-insensitive. Prefix tokens of the
     * topic up to the first space or punctuation ensure phrases like
     * "the ethics of lying" match text mentioning "ethics" or "lying".*/
    auto toLower = [](std::string s) {
        for (auto& c : s) c = (char)std::tolower((unsigned char)c);
        return s;
    };
    const std::string lnew = toLower(newInfo);
    const std::string ltop = toLower(topic);
    if (lnew.find(ltop) != std::string::npos) return true;
    size_t tokStart = 0;
    while (tokStart < ltop.size()) {
        size_t tokEnd = ltop.find_first_of(" \t,.;:!?", tokStart);
        if (tokEnd == std::string::npos) tokEnd = ltop.size();
        if (tokEnd - tokStart >= 4) { /* skip tiny function words */
            std::string tok = ltop.substr(tokStart, tokEnd - tokStart);
            if (lnew.find(tok) != std::string::npos) return true;
        }
        tokStart = tokEnd + 1;
    }
    return false;
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
