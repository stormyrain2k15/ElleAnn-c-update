/*******************************************************************************
 * LuaHost.cpp — Embedded Lua 5.4 Runtime with C++ Bindings
 *
 * Hosts behavioral scripts that can be hot-reloaded without recompiling.
 * Exposes Elle's emotion, memory, goal, and trust systems to Lua.
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <filesystem>

class LuaHost {
public:
    LuaHost() : m_L(nullptr) {}
    ~LuaHost() { Shutdown(); }

    bool Initialize() {
        m_L = luaL_newstate();
        if (!m_L) return false;

        /* Open safe libraries only (sandbox) */
        luaL_requiref(m_L, "_G", luaopen_base, 1);
        luaL_requiref(m_L, "table", luaopen_table, 1);
        luaL_requiref(m_L, "string", luaopen_string, 1);
        luaL_requiref(m_L, "math", luaopen_math, 1);
        lua_pop(m_L, 4);

        /* Remove dangerous functions */
        lua_pushnil(m_L); lua_setglobal(m_L, "os");
        lua_pushnil(m_L); lua_setglobal(m_L, "io");
        lua_pushnil(m_L); lua_setglobal(m_L, "loadfile");
        lua_pushnil(m_L); lua_setglobal(m_L, "dofile");

        /* Register C++ bindings */
        RegisterBindings();

        /* Load scripts */
        LoadAllScripts();

        ELLE_INFO("Lua host initialized (%d scripts loaded)", (int)m_loadedScripts.size());
        return true;
    }

    void Shutdown() {
        if (m_L) {
            lua_close(m_L);
            m_L = nullptr;
        }
    }

    /* Call a Lua function by name */
    bool Call(const std::string& func, int nargs = 0, int nresults = 0) {
        std::lock_guard<std::mutex> lock(m_mutex);
        lua_getglobal(m_L, func.c_str());
        if (!lua_isfunction(m_L, -1)) {
            lua_pop(m_L, 1);
            return false;
        }
        /* Arguments should already be pushed before calling */
        if (lua_pcall(m_L, nargs, nresults, 0) != LUA_OK) {
            ELLE_ERROR("Lua error in %s: %s", func.c_str(), lua_tostring(m_L, -1));
            lua_pop(m_L, 1);
            return false;
        }
        return true;
    }

    /* Evaluate Lua expression and return result as string */
    std::string Eval(const std::string& code) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::string wrapped = "return " + code;
        if (luaL_dostring(m_L, wrapped.c_str()) != LUA_OK) {
            /* Try without return */
            if (luaL_dostring(m_L, code.c_str()) != LUA_OK) {
                std::string err = lua_tostring(m_L, -1);
                lua_pop(m_L, 1);
                return "ERROR: " + err;
            }
        }
        std::string result;
        if (lua_gettop(m_L) > 0) {
            result = lua_tostring(m_L, -1) ? lua_tostring(m_L, -1) : "";
            lua_pop(m_L, 1);
        }
        return result;
    }

    /* Call personality shaping function */
    std::string ShapeResponse(const std::string& rawResponse, float valence, float arousal) {
        std::lock_guard<std::mutex> lock(m_mutex);
        lua_getglobal(m_L, "shape_personality");
        if (!lua_isfunction(m_L, -1)) { lua_pop(m_L, 1); return rawResponse; }
        lua_pushstring(m_L, rawResponse.c_str());
        lua_pushnumber(m_L, valence);
        lua_pushnumber(m_L, arousal);
        if (lua_pcall(m_L, 3, 1, 0) != LUA_OK) {
            ELLE_ERROR("Lua shape_personality error: %s", lua_tostring(m_L, -1));
            lua_pop(m_L, 1);
            return rawResponse;
        }
        std::string result = lua_tostring(m_L, -1) ? lua_tostring(m_L, -1) : rawResponse;
        lua_pop(m_L, 1);
        return result;
    }

    /* Score an intent */
    float ScoreIntent(const std::string& intentType, float urgency, float driveIntensity) {
        std::lock_guard<std::mutex> lock(m_mutex);
        lua_getglobal(m_L, "score_intent");
        if (!lua_isfunction(m_L, -1)) { lua_pop(m_L, 1); return urgency; }
        lua_pushstring(m_L, intentType.c_str());
        lua_pushnumber(m_L, urgency);
        lua_pushnumber(m_L, driveIntensity);
        if (lua_pcall(m_L, 3, 1, 0) != LUA_OK) {
            lua_pop(m_L, 1);
            return urgency;
        }
        float score = (float)lua_tonumber(m_L, -1);
        lua_pop(m_L, 1);
        return score;
    }

    /* Hot-reload all scripts */
    void ReloadScripts() {
        ELLE_INFO("Hot-reloading Lua scripts...");
        LoadAllScripts();
    }

private:
    lua_State* m_L;
    std::mutex m_mutex;
    std::vector<std::string> m_loadedScripts;

    void RegisterBindings() {
        /* elle.log(level, message) */
        lua_newtable(m_L);

        lua_pushcfunction(m_L, [](lua_State* L) -> int {
            const char* msg = luaL_checkstring(L, 1);
            ELLE_INFO("[Lua] %s", msg);
            return 0;
        });
        lua_setfield(m_L, -2, "log");

        /* elle.get_emotion(name) → float */
        lua_pushcfunction(m_L, [](lua_State* L) -> int {
            lua_pushnumber(L, 0.5); /* Placeholder — real impl queries emotional engine */
            return 1;
        });
        lua_setfield(m_L, -2, "get_emotion");

        /* elle.get_trust() → int */
        lua_pushcfunction(m_L, [](lua_State* L) -> int {
            lua_pushinteger(L, 5); /* Placeholder */
            return 1;
        });
        lua_setfield(m_L, -2, "get_trust");

        /* elle.get_config(path) → string */
        lua_pushcfunction(m_L, [](lua_State* L) -> int {
            const char* path = luaL_checkstring(L, 1);
            auto val = ElleConfig::Instance().GetString(path, "");
            lua_pushstring(L, val.c_str());
            return 1;
        });
        lua_setfield(m_L, -2, "get_config");

        /* elle.time_ms() → int */
        lua_pushcfunction(m_L, [](lua_State* L) -> int {
            lua_pushinteger(L, (lua_Integer)ELLE_MS_NOW());
            return 1;
        });
        lua_setfield(m_L, -2, "time_ms");

        lua_setglobal(m_L, "elle");
    }

    void LoadAllScripts() {
        auto& cfg = ElleConfig::Instance();
        auto scriptsDir = cfg.GetString("lua.scripts_directory", "Lua\\Elle.Lua.Behavioral\\scripts");

        m_loadedScripts.clear();

        /* Get script list from config */
        /* In practice, iterate the lua.scripts array from config */
        std::vector<std::string> scripts = {
            "personality.lua", "intent_scoring.lua", "reasoning.lua",
            "thresholds.lua", "self_reflection.lua", "goal_formation.lua",
            "ethical_reasoning.lua", "creative_synthesis.lua",
            "dream_processing.lua", "temporal_reasoning.lua",
            "social_modeling.lua", "metacognition.lua"
        };

        for (auto& script : scripts) {
            std::string path = scriptsDir + "\\" + script;
            if (luaL_dofile(m_L, path.c_str()) != LUA_OK) {
                ELLE_WARN("Failed to load Lua script: %s — %s", 
                          script.c_str(), lua_tostring(m_L, -1));
                lua_pop(m_L, 1);
            } else {
                m_loadedScripts.push_back(script);
            }
        }
    }
};

/*──────────────────────────────────────────────────────────────────────────────
 * LUA BEHAVIORAL SERVICE
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleLuaService : public ElleServiceBase {
public:
    ElleLuaService()
        : ElleServiceBase(SVC_LUA_BEHAVIORAL, "ElleLuaBehavioral",
                          "Elle-Ann Lua Behavioral Engine",
                          "Hot-reloadable behavioral scripts for personality and reasoning") {}

protected:
    bool OnStart() override {
        if (!m_host.Initialize()) return false;
        
        auto reloadInterval = (uint32_t)ElleConfig::Instance().GetInt("lua.reload_interval_seconds", 30);
        SetTickInterval(reloadInterval * 1000);
        
        ELLE_INFO("Lua behavioral service started");
        return true;
    }

    void OnStop() override {
        m_host.Shutdown();
        ELLE_INFO("Lua behavioral service stopped");
    }

    void OnTick() override {
        if (ElleConfig::Instance().GetBool("lua.auto_reload", true)) {
            m_host.ReloadScripts();
        }
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        if (msg.header.msg_type == IPC_LUA_EVAL) {
            std::string code = msg.GetStringPayload();
            std::string result = m_host.Eval(code);
            
            auto resp = ElleIPCMessage::Create(IPC_LUA_EVAL, SVC_LUA_BEHAVIORAL, sender);
            resp.SetStringPayload(result);
            GetIPCHub().Send(sender, resp);
        }
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT, SVC_EMOTIONAL };
    }

private:
    LuaHost m_host;
};

ELLE_SERVICE_MAIN(ElleLuaService)
