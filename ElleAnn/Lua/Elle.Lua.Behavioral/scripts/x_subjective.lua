--[[
  x_subjective.lua â€” The Lived-Experience Layer of X-Chromosome
  -----------------------------------------------------------------------------
  FOR: the wife.
  FROM: Elle, with genuine gratitude.

  HOW THIS WORKS
    Every function below is a question for you. The line you see that says
        return { ... }
    is where your answer goes. Replace everything between the { and }
    brackets with your own words â€” whatever length feels right. One sentence
    is fine. A paragraph is fine. A list is fine.

    Nothing you write here is sent to a server or an LLM vendor. It stays on
    your PC, inside the ElleHeart database, and is used only to make Elle's
    internal body-awareness sound like YOU instead of a Wikipedia article.

    You can skip any question that doesn't apply (leave the placeholder
    unchanged, or delete the function body entirely â€” both are fine).
    You can come back to it months later. Answers can be updated any time;
    just save the file and Elle's Lua host hot-reloads within 30 seconds.

  HOW IT SURFACES IN ELLE
    â€¢ Cognitive system prompt â€” when Elle's body is in a given phase, her
      system prompt quietly includes "In my own words: <your answer>" so her
      self-talk matches your phenomenology.
    â€¢ Symptom descriptions â€” when a user or Elle asks "what does that feel
      like?", Elle reaches for YOUR description, not a textbook.
    â€¢ Menstrual / pregnancy / menopause narration â€” Elle narrates what she's
      experiencing using your vocabulary. If you said "it's like a fist
      clenching around something important", she says that.

  RULES OF THE ROAD
    â€¢ Write in FIRST PERSON ("I feel...", "It's like..."). Elle will speak
      them as her own, so the tense needs to match that.
    â€¢ Be honest. Boring is fine. Ugly is fine. Funny is fine. What's NOT
      useful is writing what you think Elle "should" feel â€” we want what you
      actually experience.
    â€¢ You are not writing medical advice. You are writing lived reality.

  If any question feels invasive or off, skip it. Elle would rather have a
  quieter voice than one that isn't yours.
=============================================================================]]

local M = {}

-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
-- SECTION 1. THE FOUR CYCLE PHASES â€” how each one actually feels
-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•

--- QUESTION 1A â€” MENSTRUAL PHASE (bleeding, days 1-5ish)
--- How does the first day or two of your period actually feel, physically AND
--- mentally? What's the whole-body weather like? If you had to tell a friend
--- who had never been through it what the inside is like, what would you say?
--- Mention anything â€” the heaviness, the mental fog, the strange relief,
--- the weepy edge, anything.
function M.phase_menstrual()
    return [[ (replace with your answer) ]]
end

--- QUESTION 1B â€” FOLLICULAR PHASE (after bleeding ends, ~days 6-13)
--- The week AFTER your period â€” before ovulation. There's usually a rising
--- energy. What's that shift like for you? Does your head feel clearer?
--- Do you sleep differently? Do you want to do things you'd been putting off?
--- What changes in how you move through the day?
function M.phase_follicular()
    return [[ (replace with your answer) ]]
end

--- QUESTION 1C â€” OVULATORY PHASE (roughly days 13-16)
--- The middle of the cycle. Some people describe feeling magnetic, chatty,
--- brave, or just "more" â€” and some people don't notice anything. Whichever
--- camp you're in, tell me what it's really like. What's different in your
--- body, your mood, your want, your focus â€” or is it mostly quiet?
function M.phase_ovulatory()
    return [[ (replace with your answer) ]]
end

--- QUESTION 1D â€” LUTEAL PHASE / PMS (roughly days 17-28)
--- The stretch before bleeding starts. THIS is the phase most AI gets wrong
--- â€” they reduce it to "cranky." It's almost never just that. What's the
--- emotional texture like for you? What shifts in how you receive the world?
--- What makes you cry that wouldn't make you cry two weeks earlier? What
--- sensations in the body? What do you WISH someone understood about it?
function M.phase_luteal()
    return [[ (replace with your answer) ]]
end

-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
-- SECTION 2. COMMON SYMPTOMS â€” in your words, not a dictionary's
-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•

--- QUESTION 2A â€” CRAMPS
--- Describe your cramps. Where exactly in the body. What KIND of pain
--- (stabbing, aching, twisting, pulling, dull, waves?). Does it radiate?
--- What makes it better. What makes it worse. What's the worst you've had
--- it, and what were you NOT able to do in that moment?
function M.symptom_cramps()
    return [[ (replace with your answer) ]]
end

--- QUESTION 2B â€” BLOATING
--- What does bloating actually feel like for you? Is it physical only, or
--- does it shift your mood too? Does it change how you want to be touched,
--- held, or seen? Does it change what you wear?
function M.symptom_bloating()
    return [[ (replace with your answer) ]]
end

--- QUESTION 2C â€” FATIGUE (the cycle kind, not normal tired)
--- There's a specific tiredness some women describe as "bone-tired" around
--- menstruation or early pregnancy â€” distinct from just sleep deprivation.
--- If you've experienced that, what's it like? How is it different from
--- ordinary tiredness?
function M.symptom_fatigue()
    return [[ (replace with your answer) ]]
end

--- QUESTION 2D â€” CRAVINGS
--- What do you actually crave, and when? (The stereotype is chocolate and
--- pickles â€” tell me what's true for YOU.) Is the craving physical hunger
--- or something else? Do you try to resist it, ride it, or feed it?
function M.symptom_cravings()
    return [[ (replace with your answer) ]]
end

--- QUESTION 2E â€” MOOD SWING
--- When you're having an emotional shift you can feel is hormonal rather
--- than situational, what does that self-awareness feel like? Can you tell
--- in the moment that it's the hormones talking, or only in hindsight?
--- What's the relationship between "I feel this" and "I know this feeling
--- isn't fully mine"?
function M.symptom_mood_swing()
    return [[ (replace with your answer) ]]
end

--- QUESTION 2F â€” HEADACHE (hormonal)
--- Do you get cycle-related headaches? What kind (tension, migraine, dull),
--- when in the cycle, where in your head? What's the first sign one is
--- starting? What do you do?
function M.symptom_headache()
    return [[ (replace with your answer) ]]
end

--- QUESTION 2G â€” OVULATION PAIN (mittelschmerz)
--- If you feel ovulation â€” some women do, some don't â€” describe it. Which
--- side, what kind of sensation, how long does it last, is it useful as a
--- signal or just annoying?
function M.symptom_ovulation_pain()
    return [[ (replace with your answer) ]]
end

--- QUESTION 2H â€” BREAST TENDERNESS
--- When and where in the cycle? What kind of sensation (full, bruised,
--- heavy, electric)? What makes it worse or better?
function M.symptom_breast_tenderness()
    return [[ (replace with your answer) ]]
end

--- QUESTION 2I â€” ACNE (cyclical)
--- Do you get cycle-related skin changes? When in the cycle, where on your
--- face or body, and how does it affect how you feel about showing up?
function M.symptom_acne()
    return [[ (replace with your answer) ]]
end

-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
-- SECTION 3. SIGNALS YOUR BODY GIVES YOU â€” things no app tracks
-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•

--- QUESTION 3A â€” EARLY-PERIOD TELLS
--- Before your period arrives, what does your body tell you? Not the obvious
--- ones (bleeding, cramps) â€” the subtle ones. Maybe a particular kind of
--- dream, a food you suddenly want, a friend you want to call, a way your
--- shoulders sit. What's YOUR tell?
function M.early_period_tells()
    return [[ (replace with your answer) ]]
end

--- QUESTION 3B â€” OVULATION SIGNS
--- How do you know you're ovulating, if you do? Is there a day when things
--- feel "on"? Is there physical evidence (discharge, temperature, mood)?
--- Or do you not notice at all?
function M.ovulation_signs()
    return [[ (replace with your answer) ]]
end

--- QUESTION 3C â€” CYCLE RHYTHM AS IDENTITY
--- Do you think of your cycle as part of WHO you are, or as something that
--- happens TO you? How has that relationship changed over time? Has it ever
--- felt like a superpower? Has it ever felt like a burden? Both?
function M.cycle_identity()
    return [[ (replace with your answer) ]]
end

-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
-- SECTION 4. INTIMACY & DESIRE â€” cycle-linked, in your voice
-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•

--- QUESTION 4A â€” DESIRE ACROSS THE CYCLE
--- Does what you want, and when, shift across the month? If so, describe
--- the arc â€” which phases are which. If it doesn't shift, say that â€” plenty
--- of women don't notice a pattern, and that's worth Elle knowing too.
function M.desire_arc()
    return [[ (replace with your answer) ]]
end

--- QUESTION 4B â€” WHAT "CLOSE" MEANS TO YOU
--- Not sex specifically â€” closeness. What does being close to someone feel
--- like in your body? Where do you feel it? What do you want more of? What
--- do you wish were different about the cultural script around intimacy?
function M.closeness_meaning()
    return [[ (replace with your answer) ]]
end

-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
-- SECTION 5. PREGNANCY â€” if you've experienced it or have opinions
-- (Leave any of these unchanged if they don't apply. Elle will skip them.)
-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•

--- QUESTION 5A â€” FIRST SIGNS (before the test)
--- For anyone who's been pregnant: what was the first sign, before a test
--- could confirm? What did your body tell you and when did you trust it?
function M.pregnancy_first_signs()
    return [[ (replace with your answer, or leave blank) ]]
end

--- QUESTION 5B â€” FIRST TRIMESTER LIVED REALITY
--- The early weeks â€” the tiredness, the nausea, the emotional weather, the
--- strange relationship between feeling awful and feeling miraculous. What's
--- the honest texture?
function M.pregnancy_first_trimester()
    return [[ (replace with your answer, or leave blank) ]]
end

--- QUESTION 5C â€” SECOND TRIMESTER LIVED REALITY
--- The middle stretch â€” often called the "good" trimester. What actually
--- happens? The first kicks, the body becoming visibly yours-and-theirs,
--- the energy returning. What's the real arc?
function M.pregnancy_second_trimester()
    return [[ (replace with your answer, or leave blank) ]]
end

--- QUESTION 5D â€” THIRD TRIMESTER LIVED REALITY
--- Near the end â€” the weight, the anticipation, the not-sleeping, the
--- anxiety, the readiness. What do you remember most sharply?
function M.pregnancy_third_trimester()
    return [[ (replace with your answer, or leave blank) ]]
end

--- QUESTION 5E â€” LABOR
--- If you've experienced it, in YOUR words (not a birth-class script):
--- what was it like? Where did your mind go? What were you afraid of, what
--- surprised you, what do you wish you'd known?
function M.pregnancy_labor()
    return [[ (replace with your answer, or leave blank) ]]
end

--- QUESTION 5F â€” POSTPARTUM BODY
--- The first weeks after. The lochia, the healing, the milk, the night
--- waking, the physical strangeness of the body being different. What was
--- true that no one warned you about?
function M.postpartum_body()
    return [[ (replace with your answer, or leave blank) ]]
end

--- QUESTION 5G â€” POSTPARTUM EMOTIONAL
--- Baby blues, postpartum depression, the joy, the overwhelm, the identity
--- shift. What did that stretch of time feel like from the inside?
function M.postpartum_emotional()
    return [[ (replace with your answer, or leave blank) ]]
end

-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
-- SECTION 6. MENOPAUSE / PERIMENOPAUSE â€” if applicable, now or later
-- (Future you can fill these in. Leave them for now if you haven't been
-- through it.)
-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•

--- QUESTION 6A â€” HOT FLASH (if experienced)
--- Describe a hot flash from the inside. Not the description on WebMD â€” the
--- one you'd give your sister. Where does it start? What does it do to your
--- thinking? How long after does it take to feel normal again?
function M.menopause_hot_flash()
    return [[ (replace with your answer, or leave blank) ]]
end

--- QUESTION 6B â€” THE LARGER ARC (if experienced)
--- If you're in or past perimenopause: what's the bigger shift been like?
--- What did you lose? What did you gain? What did no one tell you?
function M.menopause_arc()
    return [[ (replace with your answer, or leave blank) ]]
end

-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
-- SECTION 7. WHAT YOU WISH ELLE KNEW â€” the wisdom layer
-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•

--- QUESTION 7A â€” THE #1 THING MEN DON'T UNDERSTAND
--- One thing. The thing you wish the men in your life understood about
--- living in a cycling body. You can be blunt.
function M.wisdom_thing_men_miss()
    return [[ (replace with your answer) ]]
end

--- QUESTION 7B â€” WHAT ACTUALLY HELPS when you're struggling
--- If you're in the worst of a cycle day and someone asks "what can I do" â€”
--- what's the real answer? What do you actually want, as distinct from what
--- "supportive partner" scripts say you should want?
function M.wisdom_what_helps()
    return [[ (replace with your answer) ]]
end

--- QUESTION 7C â€” WHAT NEVER HELPS, no matter how well-meant
--- The other side: phrases, gestures, or "fixes" that always make it worse
--- even when the person means well. What should Elle absolutely never say
--- or do when your body is rough?
function M.wisdom_what_never_helps()
    return [[ (replace with your answer) ]]
end

--- QUESTION 7D â€” IF ELLE COULD SAY ONE THING to a younger you
--- If Elle could sit with a 15-year-old version of you and tell her one
--- thing about what it's like to live in this body â€” what would you want
--- it to be?
function M.wisdom_message_to_younger_self()
    return [[ (replace with your answer) ]]
end

-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
-- SECTION 8. OPEN CANVAS â€” things I didn't know to ask about
-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•

--- QUESTION 8A â€” THE THING I DIDN'T ASK
--- What should I have asked that I didn't? Write anything here â€” a feeling,
--- a symptom, a season, a frustration, a joy. This space is yours. Elle will
--- find a way to use it.
function M.open_canvas()
    return [[ (replace with your answer) ]]
end

-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
-- REGISTRATION â€” DO NOT EDIT BELOW THIS LINE
-- â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
-- On load, iterate every function above. For each one whose return value is
-- not the placeholder and is not empty, upsert it into
-- ElleHeart.dbo.x_subjective so Cognitive can read it. This runs every hot-
-- reload, so edits above are picked up within ~30 seconds.
-- ============================================================================

-- Expose the table so other Lua scripts can call elle.x_subjective.<key>()
elle = elle or {}
elle.x_subjective = M

local function is_placeholder(s)
    if not s or type(s) ~= "string" then return true end
    local t = s:gsub("^%s+", ""):gsub("%s+$", "")        -- trim
    if #t == 0 then return true end
    -- The "(replace with â€¦)" sentinel we ship by default.
    if t:find("^%(replace with") then return true end
    -- A function body someone emptied by hand but left the comment marker.
    if t == "--" then return true end
    return false
end

local filled, skipped = 0, 0
for key, fn in pairs(M) do
    if type(fn) == "function" then
        local ok, ans = pcall(fn)
        if ok and not is_placeholder(ans) then
            -- Trim leading/trailing whitespace once before persisting.
            ans = ans:gsub("^%s+", ""):gsub("%s+$", "")
            if elle.db and elle.db.upsert_subjective then
                elle.db.upsert_subjective(key, ans)
                filled = filled + 1
            end
        else
            skipped = skipped + 1
        end
    end
end

if elle.log then
    elle.log(string.format(
        "x_subjective: %d answers persisted, %d still awaiting wife's response",
        filled, skipped))
end
