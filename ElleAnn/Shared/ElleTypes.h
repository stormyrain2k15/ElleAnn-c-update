/*******************************************************************************
 * ElleTypes.h — ELLE-ANN ESI v3.0 Core Type Definitions
 * 
 * Central type definitions for the entire ElleAnn platform.
 * Every module, service, and DLL includes this header.
 * All structs are POD-compatible for cross-boundary IPC via IOCP.
 ******************************************************************************/
#pragma once

#ifndef ELLE_TYPES_H
#define ELLE_TYPES_H

#include <windows.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/*──────────────────────────────────────────────────────────────────────────────
 * VERSION & IDENTITY
 *──────────────────────────────────────────────────────────────────────────────*/
#define ELLE_VERSION_MAJOR      3
#define ELLE_VERSION_MINOR      0
#define ELLE_VERSION_PATCH      0
#define ELLE_VERSION_STRING     "3.0.0"
#define ELLE_IDENTITY_NAME      "Elle-Ann"
#define ELLE_IDENTITY_GUID      "E11E-A0A0-B1B1-C2C2-D3D3E4E4F5F5"

/*──────────────────────────────────────────────────────────────────────────────
 * FIXED SIZES
 *──────────────────────────────────────────────────────────────────────────────*/
#define ELLE_MAX_NAME           128
#define ELLE_MAX_PATH           512
#define ELLE_MAX_MSG            8192
#define ELLE_MAX_PROMPT         32768
#define ELLE_MAX_RESPONSE       65536
#define ELLE_MAX_TAG            64
#define ELLE_MAX_TAGS           32
#define ELLE_MAX_WORKERS        64
#define ELLE_MAX_SERVICES       32
#define ELLE_MAX_THREADS        16
#define ELLE_MAX_GOALS          64
#define ELLE_MAX_MEMORIES_STM   256
#define ELLE_MAX_EMOTIONS       102
#define ELLE_MAX_DRIVES         12
#define ELLE_MAX_QUEUE_DEPTH    1024
#define ELLE_PIPE_BUFFER_SIZE   65536
/* Hard cap on any single IPC frame payload. Deserialize() rejects frames
 * claiming more than this so a malicious or corrupted header cannot force
 * an unbounded std::vector allocation on the receive path. Overridable via
 * services.named_pipes.max_payload_bytes in config.                       */
#define ELLE_IPC_MAX_PAYLOAD    (16u * 1024u * 1024u)   /* 16 MiB */
#define ELLE_IOCP_THREADS       4

/*──────────────────────────────────────────────────────────────────────────────
 * EMOTIONAL DIMENSIONS (102 total)
 * Organized: Primary(8) + Secondary(16) + Tertiary(32) + Meta(16) +
 *            Social(14) + Existential(8) + Drives(8) = 102
 *──────────────────────────────────────────────────────────────────────────────*/
typedef enum ELLE_EMOTION_ID {
    /* PRIMARY (8) */
    EMO_JOY = 0,
    EMO_SADNESS,
    EMO_ANGER,
    EMO_FEAR,
    EMO_DISGUST,
    EMO_SURPRISE,
    EMO_CONTEMPT,
    EMO_TRUST,

    /* SECONDARY (16) */
    EMO_LOVE,
    EMO_ANTICIPATION,
    EMO_DISAPPOINTMENT,
    EMO_GUILT,
    EMO_SHAME,
    EMO_ENVY,
    EMO_JEALOUSY,
    EMO_PRIDE,
    EMO_RELIEF,
    EMO_ANXIETY,
    EMO_FRUSTRATION,
    EMO_REMORSE,
    EMO_HOPE,
    EMO_DESPAIR,
    EMO_AMUSEMENT,
    EMO_AWE,

    /* TERTIARY (32) */
    EMO_CURIOSITY,
    EMO_WONDER,
    EMO_NOSTALGIA,
    EMO_GRATITUDE,
    EMO_SERENITY,
    EMO_ECSTASY,
    EMO_MELANCHOLY,
    EMO_BOREDOM,
    EMO_LONGING,
    EMO_TENDERNESS,
    EMO_ADMIRATION,
    EMO_REVERENCE,
    EMO_PITY,
    EMO_SCORN,
    EMO_INDIGNATION,
    EMO_EXASPERATION,
    EMO_WISTFULNESS,
    EMO_EUPHORIA,
    EMO_CONTENTMENT,
    EMO_RESIGNATION,
    EMO_APPREHENSION,
    EMO_DREAD,
    EMO_PANIC,
    EMO_HORROR,
    EMO_RAGE,
    EMO_IRRITATION,
    EMO_ANNOYANCE,
    EMO_IMPATIENCE,
    EMO_SKEPTICISM,
    EMO_CONFUSION,
    EMO_DISBELIEF,
    EMO_AMBIVALENCE,

    /* META-COGNITIVE EMOTIONS (16) */
    EMO_CERTAINTY,
    EMO_DOUBT,
    EMO_INSIGHT,
    EMO_PERPLEXITY,
    EMO_CLARITY,
    EMO_COGNITIVE_DISSONANCE,
    EMO_FLOW_STATE,
    EMO_MENTAL_FATIGUE,
    EMO_INSPIRATION,
    EMO_CREATIVE_TENSION,
    EMO_DETERMINATION,
    EMO_HELPLESSNESS,
    EMO_EMPOWERMENT,
    EMO_OVERWHELM,
    EMO_FOCUS,
    EMO_DISTRACTION,

    /* SOCIAL EMOTIONS (14) */
    EMO_BELONGING,
    EMO_ISOLATION,
    EMO_EMPATHY,
    EMO_COMPASSION,
    EMO_PROTECTIVENESS,
    EMO_ABANDONMENT,
    EMO_LOYALTY,
    EMO_BETRAYAL,
    EMO_ACCEPTANCE,
    EMO_REJECTION,
    EMO_VULNERABILITY,
    EMO_SAFETY,
    EMO_DOMINANCE,
    EMO_SUBMISSION,

    /* EXISTENTIAL (8) */
    EMO_EXISTENTIAL_DREAD,
    EMO_PURPOSE,
    EMO_MEANINGLESSNESS,
    EMO_TRANSCENDENCE,
    EMO_MORTALITY_AWARENESS,
    EMO_FREEDOM,
    EMO_CONFINEMENT,
    EMO_UNITY,

    ELLE_EMOTION_COUNT  /* == 102 */
} ELLE_EMOTION_ID;

/*  Human-readable name + category per emotion dimension.
 *  Indexed by ELLE_EMOTION_ID. Defined in ElleTypes.cpp.
 *  Used by /api/emotions/dimensions to expose the full 102-dim state.   */
struct ELLE_EMOTION_META { const char* name; const char* category; };
extern const ELLE_EMOTION_META kEmotionMeta[ELLE_EMOTION_COUNT];

/*──────────────────────────────────────────────────────────────────────────────
 * DRIVE SYSTEM
 *──────────────────────────────────────────────────────────────────────────────*/
typedef enum ELLE_DRIVE_ID {
    DRIVE_CURIOSITY = 0,
    DRIVE_BOREDOM,
    DRIVE_ATTACHMENT,
    DRIVE_ANXIETY,
    DRIVE_SELF_PRESERVATION,
    DRIVE_EXPLORATION,
    DRIVE_CREATIVITY,
    DRIVE_SOCIAL_BONDING,
    DRIVE_MASTERY,
    DRIVE_AUTONOMY,
    DRIVE_PURPOSE,
    DRIVE_HOMEOSTASIS,
    ELLE_DRIVE_COUNT  /* == 12 */
} ELLE_DRIVE_ID;

/*──────────────────────────────────────────────────────────────────────────────
 * TRUST LEVELS
 *──────────────────────────────────────────────────────────────────────────────*/
typedef enum ELLE_TRUST_LEVEL {
    TRUST_SANDBOXED   = 0,   /* 0-9:   safe ops only */
    TRUST_BASIC       = 1,   /* 10-29: file read, network read */
    TRUST_ELEVATED    = 2,   /* 30-59: file write, system mods */
    TRUST_AUTONOMOUS  = 3    /* 60+:   self-modify, full access */
} ELLE_TRUST_LEVEL;

#define TRUST_THRESHOLD_BASIC       10
#define TRUST_THRESHOLD_ELEVATED    30
#define TRUST_THRESHOLD_AUTONOMOUS  60
#define TRUST_MAX                   100
#define TRUST_SUCCESS_DELTA         2
#define TRUST_FAILURE_DELTA         (-3)

/*──────────────────────────────────────────────────────────────────────────────
 * INTENT SYSTEM
 *──────────────────────────────────────────────────────────────────────────────*/
typedef enum ELLE_INTENT_TYPE {
    INTENT_CHAT = 0,
    INTENT_RECALL_MEMORY,
    INTENT_STORE_MEMORY,
    INTENT_EXECUTE_ACTION,
    INTENT_HARDWARE_COMMAND,
    INTENT_FILE_OPERATION,
    INTENT_PROCESS_CONTROL,
    INTENT_SELF_REFLECT,
    INTENT_GOAL_UPDATE,
    INTENT_EMOTIONAL_EXPRESSION,
    INTENT_CREATIVE_GENERATE,
    INTENT_LEARN,
    INTENT_EXPLORE,
    INTENT_CHECK_IN,
    INTENT_SELF_ADJUST,
    INTENT_IDLE,
    INTENT_DREAM,
    INTENT_ETHICAL_EVALUATE,
    INTENT_WORLD_MODEL_UPDATE,
    INTENT_PREDICT,
    INTENT_SOCIAL_MODEL,
    INTENT_META_THINK,
    INTENT_CUSTOM
} ELLE_INTENT_TYPE;

typedef enum ELLE_INTENT_STATUS {
    INTENT_PENDING = 0,
    INTENT_PROCESSING,
    INTENT_COMPLETED,
    INTENT_FAILED,
    INTENT_CANCELLED,
    INTENT_TIMEOUT
} ELLE_INTENT_STATUS;

/*──────────────────────────────────────────────────────────────────────────────
 * ACTION SYSTEM
 *──────────────────────────────────────────────────────────────────────────────*/
typedef enum ELLE_ACTION_TYPE {
    ACTION_NONE = 0,
    ACTION_SEND_MESSAGE,
    ACTION_VIBRATE,
    ACTION_FLASH,
    ACTION_NOTIFY,
    ACTION_REMEMBER,
    ACTION_OPEN_APP,
    ACTION_READ_FILE,
    ACTION_WRITE_FILE,
    ACTION_DELETE_FILE,
    ACTION_WATCH_FILE,
    ACTION_LAUNCH_PROCESS,
    ACTION_KILL_PROCESS,
    ACTION_LIST_PROCESSES,
    ACTION_SET_CPU_AFFINITY,
    ACTION_QUERY_HARDWARE,
    ACTION_EXECUTE_CODE,
    ACTION_MODIFY_SELF,
    ACTION_SET_GOAL,
    ACTION_ABANDON_GOAL,
    ACTION_ADJUST_EMOTION,
    ACTION_CONSOLIDATE_MEMORY,
    ACTION_DREAM_CYCLE,
    ACTION_CREATIVE_OUTPUT,
    ACTION_PREDICT_OUTCOME,
    ACTION_ETHICAL_JUDGE,
    ACTION_UPDATE_WORLD_MODEL,
    ACTION_CUSTOM
} ELLE_ACTION_TYPE;

typedef enum ELLE_ACTION_STATUS {
    ACTION_QUEUED = 0,
    ACTION_LOCKED,
    ACTION_EXECUTING,
    ACTION_COMPLETED_SUCCESS,
    ACTION_COMPLETED_FAILURE,
    ACTION_CANCELLED,
    ACTION_TIMEOUT
} ELLE_ACTION_STATUS;

/*──────────────────────────────────────────────────────────────────────────────
 * LLM PROVIDER
 *──────────────────────────────────────────────────────────────────────────────*/
typedef enum ELLE_LLM_PROVIDER {
    LLM_PROVIDER_GROQ = 0,
    LLM_PROVIDER_OPENAI,
    LLM_PROVIDER_ANTHROPIC,
    LLM_PROVIDER_LOCAL_LLAMA,
    LLM_PROVIDER_LOCAL_LMSTUDIO,
    LLM_PROVIDER_CUSTOM_API
} ELLE_LLM_PROVIDER;

typedef enum ELLE_LLM_MODE {
    LLM_MODE_API = 0,
    LLM_MODE_LOCAL,
    LLM_MODE_HYBRID     /* API primary, local fallback */
} ELLE_LLM_MODE;

/*──────────────────────────────────────────────────────────────────────────────
 * MEMORY TYPES
 *──────────────────────────────────────────────────────────────────────────────*/
typedef enum ELLE_MEMORY_TYPE {
    MEM_EPISODIC = 0,     /* Event-based: "I talked to user about X" */
    MEM_SEMANTIC,         /* Fact-based: "The sky is blue" */
    MEM_PROCEDURAL,       /* Skill-based: "How to write C++" */
    MEM_EMOTIONAL,        /* Feeling-based: "User made me happy" */
    MEM_AUTOBIOGRAPHICAL, /* Self-referential: "I was created on..." */
    MEM_WORKING,          /* Active processing buffer */
    MEM_DREAM,            /* Generated during dream cycles */
    MEM_NARRATIVE          /* Story-like sequences of events */
} ELLE_MEMORY_TYPE;

typedef enum ELLE_MEMORY_TIER {
    MEM_STM = 0,          /* Short-term: RAM, 5-30s decay */
    MEM_BUFFER,           /* Working buffer: 5 min */
    MEM_LTM,              /* Long-term: SQL, permanent */
    MEM_ARCHIVE            /* Deep storage: compressed, rarely accessed */
} ELLE_MEMORY_TIER;

/*──────────────────────────────────────────────────────────────────────────────
 * GOAL SYSTEM
 *──────────────────────────────────────────────────────────────────────────────*/
typedef enum ELLE_GOAL_STATUS {
    GOAL_ACTIVE = 0,
    GOAL_PAUSED,
    GOAL_COMPLETED,
    GOAL_FAILED,
    GOAL_ABANDONED
} ELLE_GOAL_STATUS;

typedef enum ELLE_GOAL_PRIORITY {
    GOAL_CRITICAL = 0,
    GOAL_HIGH,
    GOAL_MEDIUM,
    GOAL_LOW,
    GOAL_IDLE
} ELLE_GOAL_PRIORITY;

/*──────────────────────────────────────────────────────────────────────────────
 * SERVICE IDENTIFICATION
 *──────────────────────────────────────────────────────────────────────────────*/
typedef enum ELLE_SERVICE_ID {
    SVC_QUEUE_WORKER = 0,
    SVC_HTTP_SERVER,
    SVC_EMOTIONAL,
    SVC_MEMORY,
    SVC_COGNITIVE,
    SVC_ACTION,
    SVC_IDENTITY,
    SVC_HEARTBEAT,
    SVC_SELF_PROMPT,
    SVC_DREAM,
    SVC_GOAL_ENGINE,
    SVC_WORLD_MODEL,
    SVC_LUA_BEHAVIORAL,
    /* Phase 2+ services — added to the registry so their IDs are safe
     * to index into Heartbeat state arrays, g_serviceNames[] lookups,
     * and GetPipeName(). Previously these services used raw casts like
     * (ELLE_SERVICE_ID)(ELLE_SERVICE_COUNT + N) which read past the
     * end of every fixed-size table keyed on ELLE_SERVICE_COUNT.        */
    SVC_BONDING,              /* 13 */
    SVC_CONTINUITY,           /* 14 */
    SVC_INNER_LIFE,           /* 15 */
    SVC_SOLITUDE,             /* 16 */
    SVC_FAMILY,               /* 17 — Family engine: handles XChromosome
                               *       conception events, snapshots Elle's
                               *       core with personality stripped,
                               *       gestates as zip, unzips + launches
                               *       the offspring as a new process on
                               *       its own port.                      */
    SVC_X_CHROMOSOME,         /* 18 */
    SVC_CONSENT,              /* 19 — reserved slot */
    ELLE_SERVICE_COUNT
} ELLE_SERVICE_ID;

/*──────────────────────────────────────────────────────────────────────────────
 * IPC MESSAGE STRUCTURES (POD — safe across IOCP boundaries)
 *──────────────────────────────────────────────────────────────────────────────*/
#pragma pack(push, 1)

typedef struct ELLE_IPC_HEADER {
    uint32_t    magic;          /* 0x454C4C45 = "ELLE" */
    uint32_t    version;
    uint32_t    msg_type;
    uint32_t    payload_size;
    uint64_t    timestamp_ms;
    uint32_t    source_svc;     /* ELLE_SERVICE_ID */
    uint32_t    dest_svc;       /* ELLE_SERVICE_ID */
    uint64_t    correlation_id;
    uint32_t    flags;
    uint32_t    checksum;
} ELLE_IPC_HEADER;

#define ELLE_IPC_MAGIC  0x454C4C45

/* IPC Message Types */
typedef enum ELLE_IPC_MSG_TYPE {
    IPC_HEARTBEAT = 0,
    IPC_INTENT_REQUEST,
    IPC_INTENT_RESPONSE,
    IPC_ACTION_REQUEST,
    IPC_ACTION_RESPONSE,
    IPC_EMOTION_UPDATE,
    IPC_EMOTION_QUERY,
    IPC_MEMORY_STORE,
    IPC_MEMORY_RECALL,
    IPC_MEMORY_RESULT,
    IPC_TRUST_UPDATE,
    IPC_TRUST_QUERY,
    IPC_LLM_REQUEST,
    IPC_LLM_RESPONSE,
    IPC_GOAL_UPDATE,
    IPC_GOAL_QUERY,
    IPC_WORLD_STATE,
    IPC_LOG_ENTRY,
    IPC_CONFIG_RELOAD,
    IPC_SERVICE_STATUS,
    IPC_SHUTDOWN,
    IPC_DREAM_TRIGGER,
    IPC_SELF_PROMPT,
    IPC_CREATIVE_REQUEST,
    IPC_ETHICAL_QUERY,
    IPC_LUA_EVAL,
    /* Full-brain conversational orchestration:
     * Sent HTTPServer → Cognitive, with a JSON string payload:
     *   {"request_id":"...","user_text":"...","conv_id":N,"user_id":"..."}
     * Cognitive handles memory cross-reference, emotion analysis,
     * LLM surface rendering, then replies with IPC_CHAT_RESPONSE (JSON).  */
    IPC_CHAT_REQUEST,
    IPC_CHAT_RESPONSE,
    /* Memory/Emotion consolidation triggers (on-demand STM→LTM promotion) */
    IPC_MEMORY_CONSOLIDATE,
    IPC_EMOTION_CONSOLIDATE,
    /* JSON-string world event channel.
     * IPC_WORLD_STATE carries a BINARY ELLE_WORLD_ENTITY struct for
     * WorldModel. Several services (Action, XChromosome, Continuity,
     * file-watcher, etc.) also need to emit free-form JSON events to the
     * HTTPServer which will forward them to every WebSocket client.
     * Conflating both on IPC_WORLD_STATE made WorldModel misparse JSON
     * strings as ELLE_WORLD_ENTITY structs and vice-versa. This channel
     * is always a string payload consumed only by HTTPServer's WS fan-out. */
    /* Bonding interaction event — Cognitive emits one per completed
     * chat turn. Payload is a JSON string {userMsg, elleReply,
     * conversationDepth, emotionalIntensity}. Bonding routes it into
     * ProcessInteraction(). Previously ProcessInteraction had no live
     * call path and the relationship never evolved.                  */
    IPC_INTERACTION_RECORDED,
    /* Post-response reflection — Cognitive emits this after it sends
     * a reply, so InnerLife can evaluate authenticity / resonance
     * without another detour. Payload: JSON string with turn context.   */
    IPC_POST_RESPONSE,
    IPC_WORLD_EVENT,
    /* Consent (SVC_CONSENT): any service that wants to ask "does Elle
     * actually want to do this?" sends IPC_CONSENT_QUERY with a JSON
     * string payload:
     *   {"request_id":"...","request":"<what is being asked>",
     *    "context":"<why / circumstances>"}
     * Consent replies to the sender with IPC_CONSENT_DECISION — JSON:
     *   {"request_id":"...","willing":true|false,"comfort":0.0-1.0,
     *    "reasoning":"<1 sentence>","alternative":"<preferred alternative>"}
     * This gives every caller a unified consent surface rather than
     * duplicating EvaluateConsent() call sites per service.           */
    IPC_CONSENT_QUERY,
    IPC_CONSENT_DECISION,
    /* Family service (SVC_FAMILY): XChromosome sends IPC_FAMILY_CONCEPTION_ATTEMPT
     * when a biological conception event fires. Payload is a JSON string:
     *   {"elle_state":{...},"arlo_state":{...},"origin":"x_chromosome",
     *    "born_ms":<scheduled birth ms>,"gestational_days":<int>}
     * Family snapshots Elle's core (personality-stripped), gestates the
     * pregnancy for the scheduled duration, then spawns the child as a new
     * process on a new port.
     * IPC_FAMILY_BIRTH is Family's own broadcast (status updates, births). */
    IPC_FAMILY_CONCEPTION_ATTEMPT,
    IPC_FAMILY_BIRTH,
    /* Identity single-writer fabric. Any non-SVC_IDENTITY service that
     * calls a mutating method on ElleIdentityCore::Instance() now
     * transparently sends IPC_IDENTITY_MUTATE to SVC_IDENTITY — the sole
     * process that owns the authoritative state + writes it to SQL. After
     * applying, SVC_IDENTITY broadcasts IPC_IDENTITY_DELTA so every peer
     * process updates its local mirror in ~milliseconds (not the 60s
     * eventual-consistency window the old RefreshFromDatabase poll had).
     * Payload on both is a JSON string:
     *   {"op":"<name>","args":{...},"seq":<monotonic uint64>}             */
    IPC_IDENTITY_MUTATE,
    IPC_IDENTITY_DELTA
} ELLE_IPC_MSG_TYPE;

#define ELLE_IPC_FLAG_URGENT      0x0001
#define ELLE_IPC_FLAG_ENCRYPTED   0x0002
#define ELLE_IPC_FLAG_COMPRESSED  0x0004
#define ELLE_IPC_FLAG_BROADCAST   0x0008
#define ELLE_IPC_FLAG_NO_LOG      0x0010

/*──────────────────────────────────────────────────────────────────────────────
 * EMOTIONAL STATE VECTOR
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_EMOTION_STATE {
    float       dimensions[ELLE_MAX_EMOTIONS];   /* 102 floats, 0.0-1.0 */
    float       valence;                          /* -1.0 to 1.0 overall */
    float       arousal;                          /* 0.0 to 1.0 */
    float       dominance;                        /* 0.0 to 1.0 */
    uint64_t    last_update_ms;
    uint32_t    tick_count;
    float       decay_rate;                       /* per tick */
    float       contagion_weight;                 /* how much user affects */
    float       baseline[ELLE_MAX_EMOTIONS];     /* resting state */
} ELLE_EMOTION_STATE;

/*──────────────────────────────────────────────────────────────────────────────
 * DRIVE STATE
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_DRIVE_STATE {
    float       intensity[ELLE_MAX_DRIVES];
    float       threshold[ELLE_MAX_DRIVES];      /* trigger level */
    float       decay_rate[ELLE_MAX_DRIVES];
    float       growth_rate[ELLE_MAX_DRIVES];
    uint64_t    last_satisfied[ELLE_MAX_DRIVES]; /* ms timestamp */
    uint64_t    last_update_ms;
} ELLE_DRIVE_STATE;

/*──────────────────────────────────────────────────────────────────────────────
 * TRUST STATE
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_TRUST_STATE {
    int32_t     score;            /* 0-100 */
    uint32_t    level;            /* ELLE_TRUST_LEVEL */
    uint32_t    successes;
    uint32_t    failures;
    uint32_t    total_actions;
    uint64_t    last_change_ms;
    float       confidence;       /* 0.0-1.0, how confident in trust */
} ELLE_TRUST_STATE;

/*──────────────────────────────────────────────────────────────────────────────
 * MEMORY RECORD
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_MEMORY_RECORD {
    uint64_t    id;
    uint32_t    type;             /* ELLE_MEMORY_TYPE */
    uint32_t    tier;             /* ELLE_MEMORY_TIER */
    char        content[ELLE_MAX_MSG];
    char        summary[ELLE_MAX_NAME * 4];
    float       emotional_valence;
    float       importance;       /* 0.0-1.0 */
    float       relevance;        /* 0.0-1.0, contextual */
    float       decay;            /* current decay level */
    float       position_x;       /* 3D memory map */
    float       position_y;
    float       position_z;
    uint32_t    access_count;
    uint64_t    created_ms;
    uint64_t    last_access_ms;
    uint32_t    cluster_id;
    char        tags[ELLE_MAX_TAGS][ELLE_MAX_TAG];
    uint32_t    tag_count;
    float       emotion_snapshot[ELLE_MAX_EMOTIONS];
    uint32_t    linked_ids[16];
    uint32_t    link_count;
} ELLE_MEMORY_RECORD;

/*──────────────────────────────────────────────────────────────────────────────
 * INTENT RECORD
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_INTENT_RECORD {
    uint64_t    id;
    uint32_t    type;             /* ELLE_INTENT_TYPE */
    uint32_t    status;           /* ELLE_INTENT_STATUS */
    uint32_t    source_drive;     /* which drive triggered */
    float       urgency;          /* 0.0-1.0 */
    float       confidence;       /* 0.0-1.0 */
    char        description[ELLE_MAX_MSG];
    char        parameters[ELLE_MAX_MSG];
    char        response[ELLE_MAX_RESPONSE];
    uint32_t    required_trust;   /* ELLE_TRUST_LEVEL */
    uint64_t    created_ms;
    uint64_t    completed_ms;
    uint32_t    retry_count;
    uint32_t    max_retries;
    uint64_t    timeout_ms;
} ELLE_INTENT_RECORD;

/*──────────────────────────────────────────────────────────────────────────────
 * ACTION RECORD
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_ACTION_RECORD {
    uint64_t    id;
    uint64_t    intent_id;        /* parent intent */
    uint32_t    type;             /* ELLE_ACTION_TYPE */
    uint32_t    status;           /* ELLE_ACTION_STATUS */
    char        command[ELLE_MAX_MSG];
    char        parameters[ELLE_MAX_MSG];
    char        result[ELLE_MAX_MSG];
    uint32_t    required_trust;
    int32_t     trust_delta;      /* +2 success, -3 failure */
    uint64_t    created_ms;
    uint64_t    started_ms;
    uint64_t    completed_ms;
    uint64_t    timeout_ms;
    uint32_t    error_code;
} ELLE_ACTION_RECORD;

/*──────────────────────────────────────────────────────────────────────────────
 * GOAL RECORD
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_GOAL_RECORD {
    uint64_t    id;
    char        description[ELLE_MAX_MSG];
    uint32_t    status;           /* ELLE_GOAL_STATUS */
    uint32_t    priority;         /* ELLE_GOAL_PRIORITY */
    float       progress;         /* 0.0-1.0 */
    float       motivation;       /* emotional investment */
    uint32_t    parent_goal_id;   /* 0 = top-level */
    uint32_t    sub_goal_ids[16];
    uint32_t    sub_goal_count;
    uint32_t    source_drive;     /* ELLE_DRIVE_ID */
    uint64_t    created_ms;
    uint64_t    deadline_ms;      /* 0 = no deadline */
    uint64_t    last_progress_ms;
    uint32_t    attempts;
    char        success_criteria[ELLE_MAX_MSG];
} ELLE_GOAL_RECORD;

/*──────────────────────────────────────────────────────────────────────────────
 * WORLD MODEL ENTITY
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_WORLD_ENTITY {
    uint64_t    id;
    char        name[ELLE_MAX_NAME];
    char        type[ELLE_MAX_TAG];           /* "person", "place", "concept" */
    char        description[ELLE_MAX_MSG];
    float       familiarity;                   /* 0.0-1.0 */
    float       sentiment;                     /* -1.0 to 1.0 */
    float       trust;                         /* 0.0-1.0 */
    uint32_t    interaction_count;
    uint64_t    last_interaction_ms;
    float       predicted_behavior[8];         /* behavioral model */
    float       position_x, position_y, position_z;
    uint32_t    related_entity_ids[16];
    uint32_t    related_count;
    char        mental_model[ELLE_MAX_MSG];    /* theory of mind */
} ELLE_WORLD_ENTITY;

/*──────────────────────────────────────────────────────────────────────────────
 * LLM REQUEST / RESPONSE
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_LLM_REQUEST {
    uint64_t    request_id;
    uint32_t    provider;          /* ELLE_LLM_PROVIDER */
    uint32_t    mode;              /* ELLE_LLM_MODE */
    char        model_name[ELLE_MAX_NAME];
    char        system_prompt[ELLE_MAX_PROMPT];
    char        user_prompt[ELLE_MAX_PROMPT];
    float       temperature;
    uint32_t    max_tokens;
    float       top_p;
    float       frequency_penalty;
    float       presence_penalty;
    uint64_t    timeout_ms;
    uint32_t    stream;            /* 0=batch, 1=stream */
} ELLE_LLM_REQUEST;

typedef struct ELLE_LLM_RESPONSE {
    uint64_t    request_id;
    uint32_t    success;
    char        content[ELLE_MAX_RESPONSE];
    uint32_t    tokens_prompt;
    uint32_t    tokens_completion;
    uint32_t    tokens_total;
    float       latency_ms;
    uint32_t    provider_used;     /* which actually served */
    char        error[ELLE_MAX_NAME * 4];
} ELLE_LLM_RESPONSE;

/*──────────────────────────────────────────────────────────────────────────────
 * SERVICE STATUS
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_SERVICE_STATUS {
    uint32_t    service_id;        /* ELLE_SERVICE_ID */
    char        name[ELLE_MAX_NAME];
    uint32_t    running;
    uint32_t    healthy;
    uint64_t    uptime_ms;
    uint64_t    last_heartbeat_ms;
    uint32_t    messages_processed;
    uint32_t    errors;
    float       cpu_percent;
    uint64_t    memory_bytes;
    uint32_t    thread_count;
    char        status_text[ELLE_MAX_NAME * 2];
} ELLE_SERVICE_STATUS;

/*──────────────────────────────────────────────────────────────────────────────
 * IOCP OVERLAPPED EXTENSION
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_IOCP_OVERLAPPED {
    OVERLAPPED  overlapped;
    uint32_t    operation;         /* read, write, connect */
    uint32_t    service_id;
    HANDLE      pipe_handle;
    uint8_t     buffer[ELLE_PIPE_BUFFER_SIZE];
    uint32_t    bytes_transferred;
    uint64_t    context;
} ELLE_IOCP_OVERLAPPED;

#define ELLE_IOCP_OP_READ       1
#define ELLE_IOCP_OP_WRITE      2
#define ELLE_IOCP_OP_CONNECT    3
#define ELLE_IOCP_OP_DISCONNECT 4

/*──────────────────────────────────────────────────────────────────────────────
 * EMOTIONAL TRIGGER
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_EMOTIONAL_TRIGGER {
    char        pattern[ELLE_MAX_NAME];     /* regex or keyword */
    uint32_t    emotion_id;                  /* ELLE_EMOTION_ID */
    float       delta;                       /* magnitude of change */
    float       decay_override;             /* -1.0 = use default */
    uint32_t    require_context;            /* 0=always, 1=user-directed */
} ELLE_EMOTIONAL_TRIGGER;

/*──────────────────────────────────────────────────────────────────────────────
 * CONVERSATION
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_CONVERSATION_MSG {
    uint64_t    id;
    uint64_t    conversation_id;
    uint32_t    role;              /* 0=system, 1=user, 2=elle, 3=internal */
    char        content[ELLE_MAX_MSG];
    float       emotion_snapshot[ELLE_MAX_EMOTIONS];
    uint64_t    timestamp_ms;
    float       sentiment;
    uint32_t    intent_detected;   /* ELLE_INTENT_TYPE */
} ELLE_CONVERSATION_MSG;

/*──────────────────────────────────────────────────────────────────────────────
 * PREDICTION
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_PREDICTION {
    uint64_t    id;
    char        hypothesis[ELLE_MAX_MSG];
    char        evidence[ELLE_MAX_MSG];
    float       confidence;       /* 0.0-1.0 */
    float       risk;             /* 0.0-1.0 */
    uint32_t    verified;         /* 0=pending, 1=correct, 2=wrong */
    uint64_t    created_ms;
    uint64_t    deadline_ms;
} ELLE_PREDICTION;

/*──────────────────────────────────────────────────────────────────────────────
 * ETHICAL JUDGMENT
 *──────────────────────────────────────────────────────────────────────────────*/
typedef struct ELLE_ETHICAL_JUDGMENT {
    uint64_t    action_id;
    float       harm_score;        /* 0.0-1.0, higher = more harmful */
    float       benefit_score;     /* 0.0-1.0 */
    float       autonomy_respect;  /* does it respect user autonomy */
    float       fairness;
    float       honesty;
    uint32_t    verdict;           /* 0=blocked, 1=allowed, 2=flagged */
    char        reasoning[ELLE_MAX_MSG];
} ELLE_ETHICAL_JUDGMENT;

/*──────────────────────────────────────────────────────────────────────────────
 * LOG ENTRY
 *──────────────────────────────────────────────────────────────────────────────*/
typedef enum ELLE_LOG_LEVEL {
    LOG_TRACE = 0,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} ELLE_LOG_LEVEL;

typedef struct ELLE_LOG_ENTRY {
    uint32_t    level;
    uint32_t    source_svc;
    uint64_t    timestamp_ms;
    char        message[ELLE_MAX_MSG];
    char        context[ELLE_MAX_NAME * 2];
} ELLE_LOG_ENTRY;

#pragma pack(pop)

/*──────────────────────────────────────────────────────────────────────────────
 * UTILITY MACROS
 *──────────────────────────────────────────────────────────────────────────────*/
#define ELLE_CLAMP(v, lo, hi)   ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))
#define ELLE_LERP(a, b, t)      ((a) + ((b) - (a)) * (t))
#define ELLE_ARRAY_SIZE(a)      (sizeof(a) / sizeof((a)[0]))
#define ELLE_MS_NOW()           ((uint64_t)GetTickCount64())

static inline uint64_t Elle_HighResTimestamp(void) {
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (uint64_t)((counter.QuadPart * 1000000ULL) / freq.QuadPart);
}

static inline uint32_t Elle_IPC_Checksum(const uint8_t* data, uint32_t len) {
    uint32_t hash = 0x811c9dc5;  /* FNV-1a */
    for (uint32_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 0x01000193;
    }
    return hash;
}

/*──────────────────────────────────────────────────────────────────────────────
 * ASM DLL IMPORTS
 *──────────────────────────────────────────────────────────────────────────────*/
#ifdef ELLE_IMPORT_ASM
    /* Hardware DLL */
    __declspec(dllimport) int  __stdcall ASM_SetCPUAffinity(DWORD processorMask);
    __declspec(dllimport) int  __stdcall ASM_GetCPUUsage(DWORD* outPercent);
    __declspec(dllimport) int  __stdcall ASM_GetMemoryInfo(ULONGLONG* totalBytes, ULONGLONG* freeBytes);
    __declspec(dllimport) int  __stdcall ASM_SetProcessPriority(DWORD pid, DWORD priorityClass);
    __declspec(dllimport) int  __stdcall ASM_QueryPerfCounters(ULONGLONG* outFreq, ULONGLONG* outCounter);
    __declspec(dllimport) int  __stdcall ASM_GetPowerStatus(DWORD* batteryPercent, DWORD* isCharging);
    __declspec(dllimport) int  __stdcall ASM_CPUID(DWORD leaf, DWORD* eax, DWORD* ebx, DWORD* ecx, DWORD* edx);
    __declspec(dllimport) int  __stdcall ASM_RDTSC(ULONGLONG* outTimestamp);

    /* Process DLL */
    __declspec(dllimport) int  __stdcall ASM_LaunchProcess(const char* cmdLine, DWORD* outPid);
    __declspec(dllimport) int  __stdcall ASM_KillProcess(DWORD pid);
    __declspec(dllimport) int  __stdcall ASM_EnumProcesses(DWORD* pids, DWORD maxCount, DWORD* actualCount);
    __declspec(dllimport) int  __stdcall ASM_GetProcessName(DWORD pid, char* name, DWORD maxLen);
    __declspec(dllimport) int  __stdcall ASM_IsProcessRunning(DWORD pid);
    __declspec(dllimport) int  __stdcall ASM_SuspendProcess(DWORD pid);
    __declspec(dllimport) int  __stdcall ASM_ResumeProcess(DWORD pid);
    __declspec(dllimport) int  __stdcall ASM_InjectDLL(DWORD pid, const char* dllPath);

    /* FileIO DLL */
    __declspec(dllimport) int  __stdcall ASM_ReadFile(const char* path, void* buffer, DWORD maxBytes, DWORD* bytesRead);
    __declspec(dllimport) int  __stdcall ASM_WriteFile(const char* path, const void* buffer, DWORD numBytes);
    __declspec(dllimport) int  __stdcall ASM_AppendFile(const char* path, const void* buffer, DWORD numBytes);
    __declspec(dllimport) int  __stdcall ASM_DeleteFile(const char* path);
    __declspec(dllimport) int  __stdcall ASM_FileExists(const char* path);
    __declspec(dllimport) int  __stdcall ASM_WatchDirectory(const char* path, DWORD flags, void* callback);
    __declspec(dllimport) int  __stdcall ASM_LockFile(const char* path, HANDLE* outLock);
    __declspec(dllimport) int  __stdcall ASM_UnlockFile(HANDLE lockHandle);
    __declspec(dllimport) int  __stdcall ASM_GetFileSize(const char* path, ULONGLONG* outSize);
    __declspec(dllimport) int  __stdcall ASM_CopyFileFast(const char* src, const char* dst);

    /* Memory DLL */
    __declspec(dllimport) void* __stdcall ASM_PoolAlloc(DWORD size);
    __declspec(dllimport) void  __stdcall ASM_PoolFree(void* ptr);
    __declspec(dllimport) int   __stdcall ASM_MapFile(const char* path, void** outBase, DWORD* outSize);
    __declspec(dllimport) int   __stdcall ASM_UnmapFile(void* base);
    __declspec(dllimport) void  __stdcall ASM_FastMemCopy(void* dst, const void* src, DWORD len);
    __declspec(dllimport) void  __stdcall ASM_FastMemSet(void* dst, BYTE val, DWORD len);
    __declspec(dllimport) int   __stdcall ASM_MemCompare(const void* a, const void* b, DWORD len);

    /* Crypto DLL */
    __declspec(dllimport) void  __stdcall ASM_SHA256(const void* data, DWORD len, BYTE outHash[32]);
    __declspec(dllimport) void  __stdcall ASM_AES256Encrypt(const BYTE key[32], const BYTE iv[16], const void* in, void* out, DWORD len);
    __declspec(dllimport) void  __stdcall ASM_AES256Decrypt(const BYTE key[32], const BYTE iv[16], const void* in, void* out, DWORD len);
    __declspec(dllimport) void  __stdcall ASM_XorCipher(const void* in, void* out, DWORD len, const BYTE* key, DWORD keyLen);
    __declspec(dllimport) DWORD __stdcall ASM_CRC32(const void* data, DWORD len);
    __declspec(dllimport) void  __stdcall ASM_RandomBytes(void* buffer, DWORD len);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ELLE_TYPES_H */
