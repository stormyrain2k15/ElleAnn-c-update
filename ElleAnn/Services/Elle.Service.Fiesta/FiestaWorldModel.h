/*══════════════════════════════════════════════════════════════════════════════
 * FiestaWorldModel.h — Phase 6b-Alpha headless world state.
 *
 *   Thread-safe in-process snapshot of everything the Fiesta decoders have
 *   observed on the wire. Every handler in FiestaClient.cpp pushes deltas
 *   into this struct; Cognitive can pull the full picture over IPC with
 *   `{"op":"get_world"}`.
 *
 *   Scope (Phase 6b-Alpha):
 *     • self     — registration number, char name, map, last known (x,y),
 *                  login-state, selfHandle (once known).
 *     • entities — handle → { kind, name|mobId, last-seen ms }.
 *                  Players gain a name via BRIEFINFO, mobs carry mob_id,
 *                  NPCs are identified by disappear-only (kept as 'npc').
 *     • zone     — mapName string recovered from MAP_LOGINCOMPLETE.
 *
 *   Not in this phase (deferred to 6b-Beta): HP/MP, gold, exp, skills,
 *   inventory. Those require decoding NC_MAP_LOGIN_ACK (0x1038) and
 *   its flood packets which are shape-matched but not yet hand-decoded.
 *
 *   Serialisation is JSON via nlohmann — same envelope every other service
 *   in Elle uses, so Cognitive / Lua / HTTPServer can consume it without
 *   custom parsers.
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once
#ifndef ELLE_FIESTA_WORLDMODEL_H
#define ELLE_FIESTA_WORLDMODEL_H

#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../Shared/json.hpp"

namespace Fiesta {

enum class EntityKind : uint8_t {
    UNKNOWN = 0,
    PLAYER  = 1,
    MOB     = 2,
    NPC     = 3,
};

inline const char* EntityKindName(EntityKind k) {
    switch (k) {
        case EntityKind::PLAYER: return "player";
        case EntityKind::MOB:    return "mob";
        case EntityKind::NPC:    return "npc";
        default:                 return "unknown";
    }
}

struct WorldEntity {
    uint16_t    handle     = 0;
    EntityKind  kind       = EntityKind::UNKNOWN;
    std::string name;          /* player displayName or empty for mob/npc */
    uint16_t    mobId      = 0;/* non-zero for mobs only                  */
    uint64_t    firstSeen  = 0;/* ms since epoch — monotonic clock OK     */
    uint64_t    lastSeen   = 0;
};

class WorldModel {
public:
    WorldModel() = default;

    /* ── Self-character mutators ─────────────────────────────────── */
    void UpdateSelfBase(uint32_t chrRegNum, const std::string& charName) {
        std::lock_guard<std::mutex> lk(m_mx);
        m_self.chrRegNum = chrRegNum;
        m_self.charName  = charName;
        m_self.lastUpdate = NowMs();
    }

    void UpdateSelfHandle(uint16_t handle) {
        std::lock_guard<std::mutex> lk(m_mx);
        m_self.handle = handle;
        m_self.lastUpdate = NowMs();
    }

    void UpdateSelfPosition(uint32_t x, uint32_t y) {
        std::lock_guard<std::mutex> lk(m_mx);
        m_self.posX = x;
        m_self.posY = y;
        m_self.lastUpdate = NowMs();
    }

    void SetLoginState(const std::string& stateName) {
        std::lock_guard<std::mutex> lk(m_mx);
        m_self.stateName  = stateName;
        m_self.lastUpdate = NowMs();
    }

    /* ── Zone mutators ───────────────────────────────────────────── */
    void SetZone(const std::string& mapName) {
        std::lock_guard<std::mutex> lk(m_mx);
        m_zone.mapName    = mapName;
        m_zone.enteredMs  = NowMs();
    }

    void ClearZone() {
        std::lock_guard<std::mutex> lk(m_mx);
        m_zone.mapName.clear();
        m_zone.enteredMs = 0;
        m_entities.clear();
    }

    /* ── Entity mutators ─────────────────────────────────────────── */
    void UpsertPlayer(uint16_t handle, const std::string& name) {
        std::lock_guard<std::mutex> lk(m_mx);
        auto& e = m_entities[handle];
        if (e.firstSeen == 0) e.firstSeen = NowMs();
        e.handle   = handle;
        e.kind     = EntityKind::PLAYER;
        if (!name.empty()) e.name = name;
        e.lastSeen = NowMs();
    }

    void UpsertMob(uint16_t handle, uint16_t mobId) {
        std::lock_guard<std::mutex> lk(m_mx);
        auto& e = m_entities[handle];
        if (e.firstSeen == 0) e.firstSeen = NowMs();
        e.handle   = handle;
        e.kind     = EntityKind::MOB;
        e.mobId    = mobId;
        e.lastSeen = NowMs();
    }

    void RemoveEntity(uint16_t handle) {
        std::lock_guard<std::mutex> lk(m_mx);
        m_entities.erase(handle);
    }

    /* ── Inspectors ──────────────────────────────────────────────── */
    size_t EntityCount() const {
        std::lock_guard<std::mutex> lk(m_mx);
        return m_entities.size();
    }

    /* Full snapshot. Thread-safe, copies under lock then renders JSON
     * without holding the lock. */
    nlohmann::json SnapshotJson() const {
        SelfChar selfCopy;
        Zone     zoneCopy;
        std::vector<WorldEntity> entitiesCopy;
        {
            std::lock_guard<std::mutex> lk(m_mx);
            selfCopy = m_self;
            zoneCopy = m_zone;
            entitiesCopy.reserve(m_entities.size());
            for (const auto& kv : m_entities) entitiesCopy.push_back(kv.second);
        }

        nlohmann::json ents = nlohmann::json::array();
        for (const auto& e : entitiesCopy) {
            nlohmann::json je = {
                {"handle",     e.handle},
                {"kind",       EntityKindName(e.kind)},
                {"first_seen", e.firstSeen},
                {"last_seen",  e.lastSeen},
            };
            if (!e.name.empty()) je["name"]   = e.name;
            if (e.mobId != 0)    je["mob_id"] = e.mobId;
            ents.push_back(std::move(je));
        }

        nlohmann::json self_j = {
            {"handle",      selfCopy.handle},
            {"chrregnum",   selfCopy.chrRegNum},
            {"char_name",   selfCopy.charName},
            {"state",       selfCopy.stateName},
            {"x",           selfCopy.posX},
            {"y",           selfCopy.posY},
            {"last_update", selfCopy.lastUpdate},
        };
        nlohmann::json zone_j = {
            {"map",        zoneCopy.mapName},
            {"entered_ms", zoneCopy.enteredMs},
        };

        return nlohmann::json{
            {"self",     std::move(self_j)},
            {"zone",     std::move(zone_j)},
            {"entities", std::move(ents)},
        };
    }

    std::string SnapshotString() const {
        return SnapshotJson().dump();
    }

private:
    struct SelfChar {
        uint16_t    handle     = 0;
        uint32_t    chrRegNum  = 0;
        std::string charName;
        std::string stateName  = "disconnected";
        uint32_t    posX       = 0;
        uint32_t    posY       = 0;
        uint64_t    lastUpdate = 0;
    };

    struct Zone {
        std::string mapName;
        uint64_t    enteredMs  = 0;
    };

    static uint64_t NowMs() {
        using namespace std::chrono;
        return (uint64_t)duration_cast<milliseconds>(
                   steady_clock::now().time_since_epoch()).count();
    }

    mutable std::mutex                              m_mx;
    SelfChar                                         m_self;
    Zone                                             m_zone;
    std::unordered_map<uint16_t, WorldEntity>        m_entities;
};

}  /* namespace Fiesta */
#endif  /* ELLE_FIESTA_WORLDMODEL_H */
