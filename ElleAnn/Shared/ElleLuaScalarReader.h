/*══════════════════════════════════════════════════════════════════════════════
 * ElleLuaScalarReader.h — Stopgap regex-grade Lua reader.
 *
 *   ────────────────────────────────────────────────────────────────────
 *   PURPOSE
 *   ────────────────────────────────────────────────────────────────────
 *   The full embedded-Lua-5.4 bridge is still P2 (own focused session).
 *   A handful of operator-facing settings — currently the Fiesta cipher
 *   region — need to be readable from `elle_settings.lua` *now*.  This
 *   reader pulls scalar `dotted.path = "value"` or `dotted.path = number`
 *   pairs out of the file with string pattern matching.  No expression
 *   evaluation, no helper-function execution, no conditionals.
 *
 *   When the full Lua bridge lands, every call site here gets ported to
 *   `lua_State` lookups and this header is deleted.  Until then it lives
 *   under `Shared/` as a clearly-marked stopgap.
 *
 *   ────────────────────────────────────────────────────────────────────
 *   GRAMMAR (subset of real Lua)
 *   ────────────────────────────────────────────────────────────────────
 *     ElleAnn.fiesta.region = "usa"            -- string, double-quoted
 *     ElleAnn.fiesta.headless_tick_hz = 50     -- integer
 *     ElleAnn.fiesta.headless_client_enabled = false  -- boolean
 *
 *   The reader walks the file line-by-line, strips `--` comments and
 *   `--[[ ]]` block comments, and matches the first occurrence of the
 *   requested key.  Whitespace and `local`/`return` lines are ignored.
 *
 *   Multi-line tables and helper-function values (e.g. `seconds(5)`) are
 *   NOT supported — the full bridge will handle those.
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once
#ifndef ELLE_LUA_SCALAR_READER_H
#define ELLE_LUA_SCALAR_READER_H

#include <string>
#include <cstdint>

class ElleLuaScalarReader {
public:
    /** Construct against a file path. The file is read once into memory.
     *  If the path doesn't exist, `IsLoaded()` returns false and every
     *  Get*() returns the supplied default. */
    explicit ElleLuaScalarReader(const std::string& path);

    bool IsLoaded() const { return m_loaded; }
    const std::string& Path() const { return m_path; }

    /** Lookup a `dotted.key` and return its string value, or `def` if not
     *  found / wrong type. Strips the surrounding quotes. */
    std::string GetString(const std::string& dottedKey,
                          const std::string& def = "") const;

    int64_t  GetInt   (const std::string& dottedKey, int64_t  def = 0) const;
    double   GetDouble(const std::string& dottedKey, double   def = 0.0) const;
    bool     GetBool  (const std::string& dottedKey, bool     def = false) const;

private:
    std::string m_path;
    std::string m_text;       /* Full file contents with comments stripped. */
    bool        m_loaded = false;

    /** Find the line containing the matching `key = ...` assignment.
     *  Returns the substring AFTER the `=`, with leading whitespace
     *  stripped, or empty string if no match.                          */
    std::string FindRhs(const std::string& dottedKey) const;
};

#endif /* ELLE_LUA_SCALAR_READER_H */
