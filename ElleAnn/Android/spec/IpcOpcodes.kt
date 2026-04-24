/*══════════════════════════════════════════════════════════════════════════════
 * IpcOpcodes.kt — Read-only reference of the IPC_X_* opcodes
 *
 *   The app NEVER speaks raw IPC. These constants exist for two reasons:
 *
 *     1. Debugging: when a WebSocket push-event envelope includes
 *        `opcode: N`, this file lets you look up what N meant in the
 *        server's internal dispatch table. Useful when tracing UI bugs
 *        back to server state transitions.
 *
 *     2. Contract reference: if the HTTP service ever adds a new
 *        push-event type, it will surface a new opcode. The Android
 *        client doesn't care WHICH opcode — only the `type` string in
 *        the envelope — but this reference lets a human verify the
 *        mapping in one place.
 *
 *   Source of truth: Services/Elle.Service.XChromosome/XChromosome.cpp
 *   (the #define IPC_X_* block near the top). If you change one side,
 *   change the other in the same commit.
 *══════════════════════════════════════════════════════════════════════════════*/
package com.elleann.android.data.spec

/** Read-only IPC opcode reference. */
object IpcOpcodes {

    // Requests FROM other services TO XChromosome (app never sends these)
    const val STATE_QUERY:          Int = 2200
    const val HISTORY_QUERY:        Int = 2201
    const val ANCHOR:               Int = 2202   // cycle anchor
    const val STIMULUS:             Int = 2203
    const val MODULATION_QUERY:     Int = 2204
    const val CONCEPTION_ATTEMPT:   Int = 2205
    const val DELIVER:              Int = 2206
    const val RESPONSE:             Int = 2207
    const val CONTRACEPTION_SET:    Int = 2208
    const val LIFECYCLE_SET:        Int = 2209
    const val SYMPTOM_LOG:          Int = 2210
    const val SYMPTOM_QUERY:        Int = 2211
    const val PREG_EVENTS_QUERY:    Int = 2212
    const val ACCELERATE:           Int = 2213

    // Broadcasts FROM XChromosome → ANY subscriber (app receives these over WS)
    const val HORMONE_UPDATE:       Int = 2220   // → WsEvent.HormoneTick
    const val PHASE_TRANSITION:     Int = 2221   // → WsEvent.PhaseTransition
    const val BIRTH:                Int = 2222   // → WsEvent.Birth
    const val LH_SURGE:             Int = 2223   // → WsEvent.LHSurge
    const val LABOR_STAGE:          Int = 2224   // → WsEvent.LaborStage
    const val MISCARRIAGE:          Int = 2225   // → WsEvent.Miscarriage

    /** Map a broadcast opcode to its WS envelope `type`.                    */
    fun broadcastTypeFor(opcode: Int): String? = when (opcode) {
        HORMONE_UPDATE   -> "hormone_tick"
        PHASE_TRANSITION -> "phase_transition"
        BIRTH            -> "birth"
        LH_SURGE         -> "lh_surge"
        LABOR_STAGE      -> "labor_stage"
        MISCARRIAGE      -> "miscarriage"
        else             -> null
    }
}
