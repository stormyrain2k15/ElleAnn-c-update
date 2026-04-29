/*══════════════════════════════════════════════════════════════════════════════
 * api.js — single API client for the Elle dev panel.
 *
 *   Base URL comes from REACT_APP_BACKEND_URL so the panel works whether
 *   served from the same host as the C++ HTTP service or pointed at a
 *   remote machine. Auth state lives in localStorage:
 *     - elle.jwt        → Authorization: Bearer <jwt>
 *     - elle.deviceId   → audit / device identification
 *     - elle.username   → cosmetic, also returned by /api/me
 *
 *   Every request returns { ok, status, data, error } so pages can show
 *   useful error states instead of `Cannot read properties of undefined`
 *   on a failed fetch.  This is the "every page loads with proper 500
 *   handling" property the operator complained was missing.
 *══════════════════════════════════════════════════════════════════════════════*/
import axios from "axios";

const BASE = process.env.REACT_APP_BACKEND_URL || "";

function authHeaders() {
  const jwt = localStorage.getItem("elle.jwt");
  const h = { "Content-Type": "application/json" };
  if (jwt) h["Authorization"] = `Bearer ${jwt}`;
  return h;
}

async function call(method, path, body) {
  const url = `${BASE}${path}`;
  try {
    const res = await axios({
      method,
      url,
      headers: authHeaders(),
      data: body,
      timeout: 15000,
      validateStatus: () => true,
    });
    if (res.status >= 200 && res.status < 300) {
      return { ok: true, status: res.status, data: res.data, error: null };
    }
    /* Surface the server's error string when the C++ side packed one. */
    const err =
      (res.data && (res.data.error || res.data.message)) ||
      `HTTP ${res.status}`;
    return { ok: false, status: res.status, data: res.data, error: err };
  } catch (e) {
    /* Connection refused / timeout / DNS — expose the real cause so the
     * user knows whether the C++ HTTP service is even running. */
    return {
      ok: false,
      status: 0,
      data: null,
      error: e.message || "network error",
    };
  }
}

export const api = {
  get:  (p)    => call("GET",    p),
  post: (p, b) => call("POST",   p, b),
  put:  (p, b) => call("PUT",    p, b),
  del:  (p)    => call("DELETE", p),
};

/*── Auth helpers ───────────────────────────────────────────────────*/
export const auth = {
  isAuthed: () => !!localStorage.getItem("elle.jwt"),
  username: () => localStorage.getItem("elle.username") || "",
  deviceId: () => localStorage.getItem("elle.deviceId") || "",

  async login(username, password) {
    const deviceId =
      localStorage.getItem("elle.deviceId") ||
      `panel-${crypto.randomUUID()}`;
    const r = await api.post("/api/auth/login", {
      username,
      password,
      device_id: deviceId,
      device_name: "Elle Dev Panel",
    });
    if (!r.ok) return r;
    const jwt = r.data.jwt || r.data.token;
    if (!jwt) return { ok: false, status: 500, error: "no jwt in response" };
    localStorage.setItem("elle.jwt", jwt);
    localStorage.setItem("elle.deviceId", deviceId);
    localStorage.setItem("elle.username", username);
    return r;
  },

  logout() {
    localStorage.removeItem("elle.jwt");
    localStorage.removeItem("elle.username");
    /* deviceId persists — it's the device's identity, not the session's. */
  },
};

export const BACKEND = BASE;
