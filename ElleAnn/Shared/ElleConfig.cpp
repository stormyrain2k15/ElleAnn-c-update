/*******************************************************************************
 * ElleConfig.cpp — Central Configuration Loader Implementation
 ******************************************************************************/
#include "ElleConfig.h"
#include "ElleLogger.h"     /* ELLE_WARN / ELLE_INFO macros used below */
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

/* Static null value for safe returns */
static const JsonValue s_nullValue;

const JsonValue& JsonValue::operator[](const std::string& key) const {
    if (type != JsonType::Object) return s_nullValue;
    auto it = obj_val.find(key);
    return (it != obj_val.end()) ? it->second : s_nullValue;
}

const JsonValue& JsonValue::operator[](size_t index) const {
    if (type != JsonType::Array || index >= arr_val.size()) return s_nullValue;
    return arr_val[index];
}

bool JsonValue::has(const std::string& key) const {
    return type == JsonType::Object && obj_val.find(key) != obj_val.end();
}

/*──────────────────────────────────────────────────────────────────────────────
 * SINGLETON
 *──────────────────────────────────────────────────────────────────────────────*/
ElleConfig& ElleConfig::Instance() {
    static ElleConfig inst;
    return inst;
}

/*──────────────────────────────────────────────────────────────────────────────
 * MINIMAL JSON PARSER (no external dependencies)
 *──────────────────────────────────────────────────────────────────────────────*/
namespace {
    struct JsonParser {
        const std::string& src;
        size_t pos = 0;

        JsonParser(const std::string& s) : src(s) {}

        void SkipWhitespace() {
            while (pos < src.size() && std::isspace((unsigned char)src[pos])) pos++;
        }

        char Peek() {
            SkipWhitespace();
            return pos < src.size() ? src[pos] : '\0';
        }

        char Next() {
            SkipWhitespace();
            return pos < src.size() ? src[pos++] : '\0';
        }

        bool Match(char c) {
            SkipWhitespace();
            if (pos < src.size() && src[pos] == c) { pos++; return true; }
            return false;
        }

        std::string ParseString() {
            if (Next() != '"') return "";
            std::string result;
            while (pos < src.size() && src[pos] != '"') {
                if (src[pos] == '\\' && pos + 1 < src.size()) {
                    pos++;
                    switch (src[pos]) {
                        case '"':  result += '"'; break;
                        case '\\': result += '\\'; break;
                        case '/':  result += '/'; break;
                        case 'n':  result += '\n'; break;
                        case 't':  result += '\t'; break;
                        case 'r':  result += '\r'; break;
                        case 'b':  result += '\b'; break;
                        case 'f':  result += '\f'; break;
                        case 'u': {
                            /* \uXXXX — 4 hex digits. Decode to UTF-8. Without
                             * this, any valid JSON that contains Unicode
                             * escapes (emoji, non-ASCII identifiers, escaped
                             * quotes encoded as \u0022, etc.) was silently
                             * mis-decoded into literal "u", 4 hex chars.    */
                            if (pos + 4 >= src.size()) break;
                            unsigned cp = 0;
                            bool ok = true;
                            for (int k = 1; k <= 4; k++) {
                                char c = src[pos + k];
                                unsigned d;
                                if      (c >= '0' && c <= '9') d = c - '0';
                                else if (c >= 'a' && c <= 'f') d = 10 + c - 'a';
                                else if (c >= 'A' && c <= 'F') d = 10 + c - 'A';
                                else { ok = false; break; }
                                cp = (cp << 4) | d;
                            }
                            if (!ok) break;
                            pos += 4;
                            /* UTF-8 encode the codepoint. */
                            if (cp < 0x80) {
                                result += (char)cp;
                            } else if (cp < 0x800) {
                                result += (char)(0xC0 | (cp >> 6));
                                result += (char)(0x80 | (cp & 0x3F));
                            } else {
                                result += (char)(0xE0 | (cp >> 12));
                                result += (char)(0x80 | ((cp >> 6) & 0x3F));
                                result += (char)(0x80 | (cp & 0x3F));
                            }
                            break;
                        }
                        default:   result += src[pos]; break;
                    }
                } else {
                    result += src[pos];
                }
                pos++;
            }
            if (pos < src.size()) pos++; /* skip closing quote */
            return result;
        }

        JsonValue ParseValue() {
            SkipWhitespace();
            if (pos >= src.size()) return {};

            char c = src[pos];
            if (c == '"') {
                JsonValue v;
                v.type = JsonType::String;
                v.str_val = ParseString();
                return v;
            }
            if (c == '{') return ParseObject();
            if (c == '[') return ParseArray();
            if (c == 't' || c == 'f') return ParseBool();
            if (c == 'n') return ParseNull();
            return ParseNumber();
        }

        JsonValue ParseObject() {
            JsonValue v;
            v.type = JsonType::Object;
            Next(); /* { */
            if (Peek() == '}') { Next(); return v; }
            while (true) {
                std::string key = ParseString();
                Match(':');
                v.obj_val[key] = ParseValue();
                if (!Match(',')) break;
            }
            Match('}');
            return v;
        }

        JsonValue ParseArray() {
            JsonValue v;
            v.type = JsonType::Array;
            Next(); /* [ */
            if (Peek() == ']') { Next(); return v; }
            while (true) {
                v.arr_val.push_back(ParseValue());
                if (!Match(',')) break;
            }
            Match(']');
            return v;
        }

        JsonValue ParseNumber() {
            SkipWhitespace();
            size_t start = pos;
            bool isFloat = false;
            if (pos < src.size() && src[pos] == '-') pos++;
            while (pos < src.size() && std::isdigit((unsigned char)src[pos])) pos++;
            if (pos < src.size() && src[pos] == '.') { isFloat = true; pos++; }
            while (pos < src.size() && std::isdigit((unsigned char)src[pos])) pos++;
            if (pos < src.size() && (src[pos] == 'e' || src[pos] == 'E')) {
                isFloat = true; pos++;
                if (pos < src.size() && (src[pos] == '+' || src[pos] == '-')) pos++;
                while (pos < src.size() && std::isdigit((unsigned char)src[pos])) pos++;
            }
            std::string num = src.substr(start, pos - start);
            JsonValue v;
            /* Non-throwing number parse — previously std::stod/std::stoll
             * could raise std::invalid_argument or std::out_of_range on
             * malformed input and propagate out of ParseJSON, aborting
             * the entire config load without a clear diagnostic.         */
            try {
                if (isFloat) {
                    v.type = JsonType::Float;
                    v.float_val = std::stod(num);
                } else {
                    v.type = JsonType::Int;
                    v.int_val = std::stoll(num);
                }
            } catch (const std::exception& ex) {
                ELLE_WARN("Config JSON number parse error at offset %zu: %s "
                          "(value=`%s`) — treating as 0",
                          start, ex.what(), num.c_str());
                v.type = JsonType::Int;
                v.int_val = 0;
            }
            return v;
        }

        JsonValue ParseBool() {
            JsonValue v;
            v.type = JsonType::Bool;
            if (src.substr(pos, 4) == "true") { v.bool_val = true; pos += 4; }
            else { v.bool_val = false; pos += 5; }
            return v;
        }

        JsonValue ParseNull() {
            pos += 4;
            return {};
        }
    };
}

/*──────────────────────────────────────────────────────────────────────────────
 * LOAD & PARSE
 *──────────────────────────────────────────────────────────────────────────────*/
bool ElleConfig::Load(const std::string& configPath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_configPath = configPath;

    std::ifstream file(configPath);
    if (!file.is_open()) return false;

    std::stringstream ss;
    ss << file.rdbuf();
    std::string json = ss.str();
    file.close();

    if (!ParseJSON(json, m_root)) return false;

    /* Schema guard — fail-closed if any of the required top-level
     * sections are missing or not objects. Previously a typo like
     * `"LLM": {...}` (wrong case) silently resolved to all defaults
     * because JsonValue operator[] returns a null on missing keys —
     * which Populate* happily treats as "just use defaults", so bad
     * configs shipped without any noise.                              */
    const char* kRequired[] = { "llm", "emotion", "memory", "http", "services" };
    for (const char* sec : kRequired) {
        const auto& v = m_root[sec];
        if (v.is_null() || v.type != JsonType::Object) {
            ELLE_ERROR("Config validation failed: section '%s' is missing or not an object "
                       "in %s — refusing to start with defaults only.",
                       sec, configPath.c_str());
            return false;
        }
    }
    /* LLM sub-validation — a provider table MUST exist, and mode MUST be
     * one of api/local/hybrid. */
    {
        const auto& llm = m_root["llm"];
        std::string mode = llm["mode"].str_val;
        if (mode != "api" && mode != "local" && mode != "hybrid") {
            ELLE_ERROR("Config validation failed: llm.mode = '%s' (must be api|local|hybrid)",
                       mode.c_str());
            return false;
        }
        const auto& provs = llm["providers"];
        if (provs.is_null() || provs.type != JsonType::Object || provs.obj_val.empty()) {
            ELLE_ERROR("Config validation failed: llm.providers must be a non-empty object");
            return false;
        }
    }

    PopulateFromJSON(m_root);
    return true;
}

bool ElleConfig::Reload() {
    if (m_configPath.empty()) return false;
    if (!Load(m_configPath)) return false;
    for (auto& cb : m_reloadCallbacks) cb();
    return true;
}

void ElleConfig::RegisterReloadCallback(std::function<void()> cb) {
    m_reloadCallbacks.push_back(std::move(cb));
}

bool ElleConfig::ParseJSON(const std::string& json, JsonValue& root) {
    JsonParser parser(json);
    root = parser.ParseValue();
    if (root.type != JsonType::Object) return false;

    /* Trailing-garbage guard — after the root object there should be only
     * whitespace. Previously this accepted any leading `{...}` and ignored
     * whatever followed, which hid copy/paste errors that accidentally
     * concatenated two config files into one. */
    parser.SkipWhitespace();
    if (parser.pos < parser.src.size()) {
        ELLE_WARN("Config JSON has trailing content after root object at offset %zu",
                  parser.pos);
    }
    return true;
}

/*──────────────────────────────────────────────────────────────────────────────
 * POPULATE TYPED CONFIGS FROM JSON
 *──────────────────────────────────────────────────────────────────────────────*/
void ElleConfig::PopulateFromJSON(const JsonValue& root) {
    /* LLM Config */
    const auto& llm = root["llm"];
    if (!llm.is_null()) {
        std::string mode = llm["mode"].str_val;
        if (mode == "api") m_llm.mode = LLM_MODE_API;
        else if (mode == "local") m_llm.mode = LLM_MODE_LOCAL;
        else m_llm.mode = LLM_MODE_HYBRID;

        m_llm.stream = llm["stream_responses"].bool_val;
        m_llm.max_context_tokens = (uint32_t)llm["max_context_tokens"].int_val;
        m_llm.system_prompt_prefix = llm["system_prompt_prefix"].str_val;
        m_llm.chain_of_thought = llm["chain_of_thought"].bool_val;
        m_llm.self_reflection = llm["self_reflection_enabled"].bool_val;
        m_llm.creative_temp_boost = (float)llm["creative_temperature_boost"].float_val;
        m_llm.reasoning_temp_drop = (float)llm["reasoning_temperature_drop"].float_val;

        /* Named-provider contract: parse these from JSON so SelectProvider
         * actually honors the config. Defaults stay set on the struct so
         * if the keys are missing the engine still picks sensible providers. */
        if (llm.has("primary_provider") && !llm["primary_provider"].str_val.empty()) {
            m_llm.primary_provider = llm["primary_provider"].str_val;
        }
        if (llm.has("fallback_provider") && !llm["fallback_provider"].str_val.empty()) {
            m_llm.fallback_provider = llm["fallback_provider"].str_val;
        }

        const auto& providers = llm["providers"];
        if (!providers.is_null()) {
            for (auto& [name, prov] : providers.obj_val) {
                LLMProviderConfig pc;
                pc.enabled = prov["enabled"].bool_val;
                pc.api_url = prov["api_url"].str_val;
                pc.api_key = prov["api_key"].str_val;
                pc.model = prov["model"].str_val;
                pc.max_tokens = prov.has("max_tokens") ? (uint32_t)prov["max_tokens"].int_val : 4096;
                pc.temperature = prov.has("temperature") ? (float)prov["temperature"].float_val : 0.85f;
                pc.top_p = prov.has("top_p") ? (float)prov["top_p"].float_val : 0.95f;
                pc.timeout_ms = prov.has("timeout_ms") ? (uint32_t)prov["timeout_ms"].int_val : 30000;
                pc.model_path = prov["model_path"].str_val;
                pc.context_size = prov.has("context_size") ? (uint32_t)prov["context_size"].int_val : 8192;
                pc.gpu_layers = prov.has("gpu_layers") ? (uint32_t)prov["gpu_layers"].int_val : 0;
                pc.threads = prov.has("threads") ? (uint32_t)prov["threads"].int_val : 4;
                pc.batch_size = prov.has("batch_size") ? (uint32_t)prov["batch_size"].int_val : 512;
                pc.mlock = prov["mlock"].bool_val;
                pc.mmap = prov.has("mmap") ? prov["mmap"].bool_val : true;
                pc.use_gpu = prov["use_gpu"].bool_val;
                /* Provider tuning that was previously declared in the
                 * schema but never actually read. Landing them here makes
                 * the shipped elle_master_config.json a real contract. */
                pc.frequency_penalty = prov.has("frequency_penalty")
                    ? (float)prov["frequency_penalty"].float_val : 0.0f;
                pc.presence_penalty  = prov.has("presence_penalty")
                    ? (float)prov["presence_penalty"].float_val  : 0.0f;
                pc.rate_limit_rpm    = prov.has("rate_limit_rpm")
                    ? (uint32_t)prov["rate_limit_rpm"].int_val  : 0;
                m_llm.providers[name] = pc;
            }
        }
    }

    /* Emotion Config */
    const auto& emo = root["emotions"];
    if (!emo.is_null()) {
        m_emotion.decay_rate = (float)emo["decay_rate_per_tick"].float_val;
        m_emotion.tick_interval_ms = (uint32_t)emo["tick_interval_ms"].int_val;
        m_emotion.contagion_weight = (float)emo["contagion_weight"].float_val;
        m_emotion.baseline_return_rate = (float)emo["baseline_return_rate"].float_val;
        m_emotion.emotional_inertia = (float)emo["emotional_inertia"].float_val;
        m_emotion.mood_threshold = (float)emo["mood_formation_threshold"].float_val;
        m_emotion.mood_duration_ticks = (uint32_t)emo["mood_duration_ticks"].int_val;
        m_emotion.sentiment_analysis = emo["sentiment_analysis_enabled"].bool_val;

        /* Reload idempotency: PopulateFromJSON is called on every
         * ReloadConfig(), and these collections get rebuilt from scratch.
         * Without clearing we doubled the baselines / triggers on every
         * reload and the repeated triggers amplified emotional deltas.   */
        m_emotion.baselines.clear();
        m_emotion.triggers.clear();

        const auto& baselines = emo["baselines"];
        if (!baselines.is_null()) {
            for (auto& [k, v] : baselines.obj_val) {
                m_emotion.baselines[k] = (float)v.float_val;
            }
        }

        const auto& triggers = emo["triggers"];
        if (!triggers.is_null()) {
            for (auto& t : triggers.arr_val) {
                EmotionTriggerConfig tc;
                tc.pattern = t["pattern"].str_val;
                tc.emotion = t["emotion"].str_val;
                tc.delta = (float)t["delta"].float_val;
                m_emotion.triggers.push_back(tc);
            }
        }
    }

    /* Service Config */
    const auto& svc = root["services"];
    if (!svc.is_null()) {
        m_service.iocp_threads = (uint32_t)svc["iocp_thread_count"].int_val;
        m_service.heartbeat_ms = (uint32_t)svc["heartbeat_interval_ms"].int_val;
        m_service.heartbeat_timeout_ms = (uint32_t)svc["heartbeat_timeout_ms"].int_val;
        m_service.dead_man_timeout_ms = (uint32_t)svc["dead_man_switch_timeout_ms"].int_val;
        m_service.watchdog_ms = (uint32_t)svc["watchdog_interval_ms"].int_val;
        m_service.stale_worker_ms = (uint32_t)svc["stale_worker_timeout_ms"].int_val;
        m_service.max_restarts = (uint32_t)svc["max_restart_attempts"].int_val;

        const auto& pipes = svc["named_pipes"];
        if (!pipes.is_null()) {
            m_service.pipe_prefix = pipes["prefix"].str_val;
            m_service.pipe_buffer_size = (uint32_t)pipes["buffer_size"].int_val;
            m_service.pipe_max_instances = (uint32_t)pipes["max_instances"].int_val;
        }

        const auto& sql = svc["sql_pipes"];
        if (!sql.is_null()) {
            m_service.sql_connection_string = sql["connection_string"].str_val;
            m_service.sql_pool_size = (uint32_t)sql["pool_size"].int_val;
            m_service.sql_timeout_seconds = (uint32_t)sql["command_timeout_seconds"].int_val;
        }
    }

    /* Trust Config */
    const auto& trust = root["trust"];
    if (!trust.is_null()) {
        m_trust.initial_score = (int32_t)trust["initial_score"].int_val;
        m_trust.max_score = (int32_t)trust["max_score"].int_val;
        m_trust.success_delta = (int32_t)trust["success_delta"].int_val;
        m_trust.failure_delta = (int32_t)trust["failure_delta"].int_val;
        m_trust.decay_idle_hours = (uint32_t)trust["decay_on_idle_hours"].int_val;
        m_trust.decay_amount = (int32_t)trust["decay_amount"].int_val;
        m_trust.audit_all = trust["audit_all_actions"].bool_val;
    }

    /* Memory Config */
    const auto& mem = root["memory"];
    if (!mem.is_null()) {
        m_memory.stm_capacity = (uint32_t)mem["stm_capacity"].int_val;
        m_memory.stm_decay_seconds = (float)mem["stm_decay_seconds"].int_val;
        m_memory.promote_threshold = (float)mem["stm_promote_threshold"].float_val;
        m_memory.consolidation_interval_min = (uint32_t)mem["ltm_consolidation_interval_minutes"].int_val;
        m_memory.recall_interval_sec = (uint32_t)mem["recall_loop_interval_seconds"].int_val;
        m_memory.cluster_threshold = (float)mem["cluster_similarity_threshold"].float_val;
        m_memory.max_clusters = (uint32_t)mem["max_clusters"].int_val;
        m_memory.narrative_gen = mem["narrative_generation_enabled"].bool_val;
        m_memory.dream_consolidation = mem["dream_consolidation_enabled"].bool_val;
        m_memory.dream_interval_min = (uint32_t)mem["dream_interval_minutes"].int_val;
        m_memory.archive_after_days = (uint32_t)mem["archive_after_days"].int_val;
    }

    /* HTTP Config */
    const auto& http = root["http_server"];
    if (!http.is_null()) {
        m_http.bind_address = http["bind_address"].str_val;
        m_http.port = (uint32_t)http["port"].int_val;
        m_http.ws_path = http["websocket_path"].str_val;
        m_http.max_connections = (uint32_t)http["max_connections"].int_val;
        /* auth_enabled: honor the JSON-defined value ONLY when the key is
         * actually present. Previously we wrote `http["auth_enabled"].bool_val`
         * unconditionally, which flipped auth OFF whenever the key was
         * missing — a silent fail-open. Missing key now keeps the struct
         * default (true = fail-closed).                                   */
        if (!http["auth_enabled"].is_null())
            m_http.auth_enabled = http["auth_enabled"].bool_val;
        m_http.jwt_secret = http["jwt_secret"].str_val;
        m_http.jwt_expiry_hours = (uint32_t)http["jwt_expiry_hours"].int_val;

        /* Optional CORS + rate-limit + body-size knobs. Missing keys
         * keep the struct defaults (CORS on, wildcard, no rate limit). */
        if (!http["cors_enabled"].is_null())
            m_http.cors_enabled = http["cors_enabled"].bool_val;
        if (http["cors_origins"].type == JsonType::Array) {
            std::string joined;
            for (auto& v : http["cors_origins"].arr_val) {
                if (!joined.empty()) joined += ",";
                joined += v.str_val;
            }
            if (!joined.empty()) m_http.cors_origins = joined;
        } else if (!http["cors_origins"].is_null()) {
            m_http.cors_origins = http["cors_origins"].str_val;
        }
        if (!http["rate_limit_rpm"].is_null())
            m_http.rate_limit_rpm = (uint32_t)http["rate_limit_rpm"].int_val;
        if (!http["max_concurrent_connections"].is_null())
            m_http.max_concurrent_connections =
                (uint32_t)http["max_concurrent_connections"].int_val;
        if (!http["max_ws_frame_bytes"].is_null())
            m_http.max_ws_frame_bytes = (uint32_t)http["max_ws_frame_bytes"].int_val;
        if (!http["max_upload_bytes"].is_null())
            m_http.max_upload_bytes = (uint32_t)http["max_upload_bytes"].int_val;
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * GENERIC ACCESSORS (dot-path navigation)
 *──────────────────────────────────────────────────────────────────────────────*/
const JsonValue* ElleConfig::Navigate(const std::string& dotPath) const {
    const JsonValue* current = &m_root;
    std::istringstream iss(dotPath);
    std::string segment;
    while (std::getline(iss, segment, '.')) {
        if (current->type != JsonType::Object) return nullptr;
        auto it = current->obj_val.find(segment);
        if (it == current->obj_val.end()) return nullptr;
        current = &it->second;
    }
    return current;
}

std::string ElleConfig::GetString(const std::string& path, const std::string& def) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    const JsonValue* v = Navigate(path);
    return (v && v->type == JsonType::String) ? v->str_val : def;
}

int64_t ElleConfig::GetInt(const std::string& path, int64_t def) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    const JsonValue* v = Navigate(path);
    if (!v) return def;
    if (v->type == JsonType::Int) return v->int_val;
    if (v->type == JsonType::Float) return (int64_t)v->float_val;
    return def;
}

double ElleConfig::GetFloat(const std::string& path, double def) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    const JsonValue* v = Navigate(path);
    if (!v) return def;
    if (v->type == JsonType::Float) return v->float_val;
    if (v->type == JsonType::Int) return (double)v->int_val;
    return def;
}

bool ElleConfig::GetBool(const std::string& path, bool def) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    const JsonValue* v = Navigate(path);
    return (v && v->type == JsonType::Bool) ? v->bool_val : def;
}
