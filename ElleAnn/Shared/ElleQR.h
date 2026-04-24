/*══════════════════════════════════════════════════════════════════════════════
 * ElleQR.h — Minimal QR Code Model 2 encoder (byte mode, ECC levels L/M/Q/H)
 *
 *   Public-domain-style, zero-dependency (std only) QR encoder suitable
 *   for small payloads like pairing URIs. Handles versions 1–10, which
 *   cover up to 213 bytes at ECC-M — more than enough for an
 *   `ellepair://host:65535/XXXXXX` link.
 *
 *   Reference: ISO/IEC 18004:2015 (QR Code Model 2). The arithmetic
 *   constants here (generator polynomials, format/version bit patterns,
 *   mask penalty rules) are direct transcriptions from the spec; see
 *   the comments next to each table for section numbers.
 *
 *   API:
 *     ElleQR::Code c = ElleQR::Encode("text", ElleQR::Ecc::M);
 *     // c.size is the side length in modules; c.Get(x,y) → true for dark
 *     //   or false for light. Feed to your renderer of choice.
 *     std::string svg = ElleQR::ToSvg(c, 4); // 4 = scale factor per module
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ElleQR {

enum class Ecc { L = 0, M = 1, Q = 2, H = 3 };

struct Code {
    int               size = 0;              /* side length in modules       */
    std::vector<bool> modules;                /* row-major, size*size         */
    bool Get(int x, int y) const {
        return modules[(size_t)y * (size_t)size + (size_t)x];
    }
    void Set(int x, int y, bool v) {
        modules[(size_t)y * (size_t)size + (size_t)x] = v;
    }
};

/** Encode UTF-8 text (treated as byte mode) into a QR Code. Picks the
 *  smallest version 1–10 that fits. Returns an empty Code (size=0) if
 *  the payload is too large for version 10 at the requested ECC.      */
Code Encode(const std::string& data, Ecc ecc = Ecc::M);

/** Render the code as a scalable SVG string with the given per-module
 *  scale factor (in SVG user units). quietZone is the light border
 *  width in modules (QR spec requires ≥4, 4 is recommended).          */
std::string ToSvg(const Code& code, int scale = 6, int quietZone = 4);

}  /* namespace ElleQR */
