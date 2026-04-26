/*══════════════════════════════════════════════════════════════════════════════
 * FiestaBriefInfoRing.h — handle ⇆ display-name cache.
 *
 *   Populated from server-pushed BRIEFINFO_LOGINCHARACTER /
 *   BRIEFINFO_BRIEFINFODELETE packets in FiestaClient.  Used to
 *   resolve `speaker_handle` in chat broadcasts into a human-readable
 *   name for downstream Cognitive consumption.
 *
 *   This header is platform-portable on purpose — it lives apart
 *   from FiestaConnection.h (which pulls in winsock2.h) so the
 *   Linux smoke test under `backend/tests/fiesta_smoke.cpp` can
 *   exercise the ring without depending on Windows headers.
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once
#ifndef ELLE_FIESTA_BRIEFINFO_RING_H
#define ELLE_FIESTA_BRIEFINFO_RING_H

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>

namespace Fiesta {

class BriefInfoRing {
public:
    /** Memory-cap safety net. The cap evicts a stale entry rather
     *  than refusing the insert — preferring fresh data over old in
     *  griefer / spawn-flood scenarios. */
    static constexpr size_t kMaxEntries = 4096;

    void Insert(uint16_t handle, const std::string& name) {
        std::lock_guard<std::mutex> lk(m_mx);
        if (m_map.size() >= kMaxEntries && !m_map.count(handle)) {
            m_map.erase(m_map.begin());
        }
        m_map[handle] = name;
    }
    void Remove(uint16_t handle) {
        std::lock_guard<std::mutex> lk(m_mx);
        m_map.erase(handle);
    }
    /** Returns the display name for `handle` if known, else empty. */
    std::string Resolve(uint16_t handle) const {
        std::lock_guard<std::mutex> lk(m_mx);
        const auto it = m_map.find(handle);
        return (it == m_map.end()) ? std::string() : it->second;
    }
    void Clear() {
        std::lock_guard<std::mutex> lk(m_mx);
        m_map.clear();
    }
    size_t Size() const {
        std::lock_guard<std::mutex> lk(m_mx);
        return m_map.size();
    }

private:
    mutable std::mutex                        m_mx;
    std::unordered_map<uint16_t, std::string> m_map;
};

}  /* namespace Fiesta */
#endif
