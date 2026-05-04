-------------------------------------------------------------------------------
-- thresholds.lua — Tunable Constants (no recompile needed)
-------------------------------------------------------------------------------

thresholds = {}

-- Emotional thresholds
thresholds.emotion = {
    mood_formation = 0.6,       -- sustained emotion level to form a mood
    mood_duration_min = 300,    -- minimum ticks for mood to be significant
    contagion_sensitivity = 0.35,
    trigger_min_delta = 0.1,    -- minimum emotional shift from triggers
    expression_threshold = 0.4, -- above this, emotion influences behavior
    crisis_threshold = 0.85,    -- emergency emotional state
    baseline_deviation = 0.3,   -- how far from baseline before notable
}

-- Cognitive thresholds
thresholds.cognitive = {
    intent_confidence_min = 0.6,
    attention_decay_rate = 0.01,
    context_relevance_min = 0.3,
    metacognition_trigger = 0.7,  -- self-evaluate when confusion > this
    creativity_flow_threshold = 0.75,
    reasoning_depth_default = 3,
    reasoning_depth_complex = 5,
}

-- Drive thresholds (override config defaults)
thresholds.drives = {
    curiosity_trigger = 0.7,
    boredom_trigger = 0.6,
    attachment_trigger = 0.8,
    anxiety_trigger = 0.5,
    exploration_trigger = 0.65,
    creativity_trigger = 0.6,
    mastery_trigger = 0.7,
}

-- Trust thresholds
thresholds.trust = {
    auto_approve_above = 30,    -- auto-approve actions above this trust
    warn_below = 15,            -- warn user about low trust
    dangerous_action_min = 50,  -- minimum trust for potentially harmful actions
}

-- Memory thresholds
thresholds.memory = {
    stm_importance_promote = 0.6,
    stm_access_count_promote = 3,
    ltm_relevance_min = 0.3,
    cluster_similarity = 0.75,
    narrative_memory_count = 10,
    dream_importance_boost = 0.15,
}

-- Self-prompt thresholds
thresholds.self_prompt = {
    idle_before_thought = 60,     -- seconds of idle before self-prompting
    min_between_thoughts = 30,    -- minimum seconds between autonomous thoughts
    drive_pressure_min = 0.5,     -- cumulative drive pressure to trigger
    random_thought_chance = 0.15, -- probability of random thought per check
}

-- Goal thresholds
thresholds.goals = {
    motivation_min = 0.2,         -- abandon goal if motivation drops below
    progress_stale_days = 7,      -- auto-abandon after this many days
    max_concurrent_goals = 16,
    auto_generate_drive_min = 0.7,-- drive intensity to auto-generate goal
}

-- Ethical thresholds
thresholds.ethics = {
    harm_block = 0.9,             -- auto-block above this harm score
    harm_warn = 0.5,              -- require justification above this
    benefit_override = 0.85,      -- benefit can override harm up to block level
    consent_required_above = 0.6, -- actions above this need explicit consent
}

-- Get a threshold value by category and name
function get_threshold(category, name)
    local cat = thresholds[category]
    if cat then
        return cat[name]
    end
    return nil
end

elle.log("thresholds.lua loaded")
