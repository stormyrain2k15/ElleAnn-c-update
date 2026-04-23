/*******************************************************************************
 * ElleLLM.h — Dual-Mode LLM Integration (API + Local)
 * 
 * Unified interface for:
 * - Cloud APIs: Groq, OpenAI, Anthropic, Custom
 * - Local inference: llama.cpp direct, LM Studio (OpenAI-compatible local API)
 * 
 * Supports streaming, token counting, conversation history management,
 * automatic failover between providers.
 ******************************************************************************/
#pragma once
#ifndef ELLE_LLM_H
#define ELLE_LLM_H

#include "ElleTypes.h"
#include "ElleConfig.h"
#include <string>
#include <vector>
#include <functional>
#include <mutex>
#include <memory>
#include <atomic>
#include <thread>

/*──────────────────────────────────────────────────────────────────────────────
 * CONVERSATION MESSAGE
 *──────────────────────────────────────────────────────────────────────────────*/
struct LLMMessage {
    std::string role;     /* "system", "user", "assistant" */
    std::string content;
};

/*──────────────────────────────────────────────────────────────────────────────
 * STREAM CALLBACK
 *──────────────────────────────────────────────────────────────────────────────*/
typedef std::function<void(const std::string& token, bool done)> LLMStreamCallback;

/*──────────────────────────────────────────────────────────────────────────────
 * LLM PROVIDER INTERFACE
 *──────────────────────────────────────────────────────────────────────────────*/
class ILLMProvider {
public:
    virtual ~ILLMProvider() = default;

    virtual bool Initialize(const LLMProviderConfig& config) = 0;
    virtual void Shutdown() = 0;
    virtual bool IsAvailable() const = 0;

    /* Synchronous completion */
    virtual ELLE_LLM_RESPONSE Complete(const std::vector<LLMMessage>& messages,
                                        float temperature = -1.0f,
                                        uint32_t maxTokens = 0) = 0;

    /* Streaming completion */
    virtual bool StreamComplete(const std::vector<LLMMessage>& messages,
                                LLMStreamCallback callback,
                                float temperature = -1.0f,
                                uint32_t maxTokens = 0) = 0;

    virtual ELLE_LLM_PROVIDER GetProviderId() const = 0;
    virtual std::string GetModelName() const = 0;
    virtual uint32_t EstimateTokens(const std::string& text) const = 0;

    /* Baseline temperature from provider config — used by ElleLLMEngine::Chat
     * when a caller leaves temperature unset (-1) and the config's creative/
     * reasoning boosts need a real starting point. Without this, the boost
     * math runs on top of a sentinel (-1.0f) and the provider silently
     * ignores the resulting negative value and falls back to its own
     * default, rendering the config knobs dead. Defaults to 0.7 when the
     * provider cannot expose a configured baseline.                       */
    virtual float GetBaselineTemperature() const = 0;
};

/*──────────────────────────────────────────────────────────────────────────────
 * HTTP-BASED PROVIDER (Groq, OpenAI, Anthropic, LM Studio, Custom)
 *──────────────────────────────────────────────────────────────────────────────*/
class LLMAPIProvider : public ILLMProvider {
public:
    LLMAPIProvider(ELLE_LLM_PROVIDER id);
    ~LLMAPIProvider() override;

    bool Initialize(const LLMProviderConfig& config) override;
    void Shutdown() override;
    bool IsAvailable() const override;

    ELLE_LLM_RESPONSE Complete(const std::vector<LLMMessage>& messages,
                                float temperature, uint32_t maxTokens) override;
    bool StreamComplete(const std::vector<LLMMessage>& messages,
                        LLMStreamCallback callback,
                        float temperature, uint32_t maxTokens) override;

    ELLE_LLM_PROVIDER GetProviderId() const override { return m_providerId; }
    std::string GetModelName() const override { return m_config.model; }
    uint32_t EstimateTokens(const std::string& text) const override;
    float GetBaselineTemperature() const override { return m_config.temperature; }

private:
    ELLE_LLM_PROVIDER m_providerId;
    LLMProviderConfig m_config;
    bool              m_available = false;
    std::mutex        m_rateMutex;
    uint64_t          m_lastRequestMs = 0;
    uint32_t          m_requestsThisMinute = 0;

    /* WinHTTP handles */
    void*   m_hSession = nullptr;   /* HINTERNET */
    void*   m_hConnect = nullptr;   /* HINTERNET */
    bool    m_useTls   = false;     /* set from URL scheme at InitWinHTTP() */

    bool InitWinHTTP();
    void CleanupWinHTTP();

    /* Request building */
    std::string BuildRequestBody(const std::vector<LLMMessage>& messages,
                                  float temperature, uint32_t maxTokens, bool stream);
    std::string BuildAnthropicBody(const std::vector<LLMMessage>& messages,
                                    float temperature, uint32_t maxTokens, bool stream);

    /* HTTP execution. `HTTPPost` returns the body as a string and writes
     * the HTTP status code to `outStatus`. Non-2xx responses still fill
     * the body (so we can surface the provider's error text) but let
     * the caller refuse to parse garbage as a chat completion.          */
    std::string HTTPPost(const std::string& path, const std::string& body,
                         const std::vector<std::pair<std::string, std::string>>& headers,
                         int* outStatus = nullptr);
    bool HTTPPostStream(const std::string& path, const std::string& body,
                        const std::vector<std::pair<std::string, std::string>>& headers,
                        std::function<void(const std::string& chunk)> onChunk);

    /* Response parsing */
    ELLE_LLM_RESPONSE ParseOpenAIResponse(const std::string& json);
    ELLE_LLM_RESPONSE ParseAnthropicResponse(const std::string& json);
    std::string ParseStreamChunk(const std::string& chunk, bool isAnthropic);

    /* Rate limiting */
    bool CheckRateLimit();
    void RecordRequest();
};

/*──────────────────────────────────────────────────────────────────────────────
 * LOCAL LLAMA.CPP PROVIDER
 * Loads GGUF model directly, no external server needed.
 *──────────────────────────────────────────────────────────────────────────────*/

/* Forward declare llama.cpp types */
struct llama_model;
struct llama_context;

class LLMLocalProvider : public ILLMProvider {
public:
    LLMLocalProvider();
    ~LLMLocalProvider() override;

    bool Initialize(const LLMProviderConfig& config) override;
    void Shutdown() override;
    bool IsAvailable() const override;

    ELLE_LLM_RESPONSE Complete(const std::vector<LLMMessage>& messages,
                                float temperature, uint32_t maxTokens) override;
    bool StreamComplete(const std::vector<LLMMessage>& messages,
                        LLMStreamCallback callback,
                        float temperature, uint32_t maxTokens) override;

    ELLE_LLM_PROVIDER GetProviderId() const override { return LLM_PROVIDER_LOCAL_LLAMA; }
    std::string GetModelName() const override { return m_modelPath; }
    uint32_t EstimateTokens(const std::string& text) const override;
    float GetBaselineTemperature() const override { return m_config.temperature; }

    /* Local-specific */
    bool IsModelLoaded() const { return m_model != nullptr; }
    uint32_t GetContextSize() const { return m_contextSize; }
    float GetLoadProgress() const { return m_loadProgress; }

private:
    LLMProviderConfig m_config;
    std::string       m_modelPath;
    uint32_t          m_contextSize = 0;
    float             m_loadProgress = 0.0f;

    llama_model*      m_model = nullptr;
    llama_context*    m_ctx = nullptr;
    std::mutex        m_inferenceMutex;

    /* Chat template formatting — shared by subprocess invocation. */
    std::string FormatChatPrompt(const std::vector<LLMMessage>& messages);

    /* Generation loop — spawns llama-cli.exe as a subprocess (see Generate()
     * in ElleLLM.cpp for the full protocol). In-process llama linkage was
     * considered and rejected; the subprocess path is what Complete/Stream
     * actually use today.                                                  */
    std::string Generate(const std::string& prompt, uint32_t maxTokens,
                         float temperature, LLMStreamCallback callback = nullptr);
};

/*──────────────────────────────────────────────────────────────────────────────
 * LLM ENGINE (Unified interface with failover)
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleLLMEngine {
public:
    static ElleLLMEngine& Instance();

    bool Initialize();
    void Shutdown();

    /* High-level chat */
    ELLE_LLM_RESPONSE Chat(const std::vector<LLMMessage>& messages,
                            float temperature = -1.0f,
                            uint32_t maxTokens = 0);

    bool StreamChat(const std::vector<LLMMessage>& messages,
                    LLMStreamCallback callback,
                    float temperature = -1.0f,
                    uint32_t maxTokens = 0);

    /* Single-shot convenience */
    std::string Ask(const std::string& prompt, const std::string& systemPrompt = "");

    /* With Elle's personality and emotional context */
    ELLE_LLM_RESPONSE ElleChat(const std::string& userMessage,
                                const std::vector<LLMMessage>& history,
                                const ELLE_EMOTION_STATE& emotions,
                                const std::string& memoryContext = "",
                                const std::string& goalContext = "");

    /* Specialized prompts */
    std::string AnalyzeSentiment(const std::string& text);
    std::string ParseIntent(const std::string& text, const std::string& context);
    std::string GenerateCreative(const std::string& theme, float creativity = 1.0f);
    std::string SelfReflect(const std::string& context, const ELLE_EMOTION_STATE& emotions);
    std::string EthicalEvaluate(const std::string& action, const std::string& context);
    std::string FormGoal(const std::string& driveContext, const std::string& emotionContext);
    std::string DreamNarrate(const std::vector<std::string>& memories);

    /* Provider management */
    bool IsAPIAvailable() const;
    bool IsLocalAvailable() const;
    ELLE_LLM_PROVIDER GetActiveProvider() const { return m_activeProvider; }
    void ForceProvider(ELLE_LLM_PROVIDER provider);
    void ResetProviderSelection();

    /* Token estimation */
    uint32_t EstimateTokens(const std::string& text) const;

    /* Stats */
    uint64_t TotalRequests() const { return m_totalRequests; }
    uint64_t TotalTokens() const { return m_totalTokens; }
    float    AverageLatencyMs() const;

private:
    ElleLLMEngine() = default;
    ~ElleLLMEngine() = default;

    std::vector<std::unique_ptr<ILLMProvider>> m_providers;
    ELLE_LLM_MODE     m_mode = LLM_MODE_HYBRID;
    ELLE_LLM_PROVIDER m_activeProvider = LLM_PROVIDER_GROQ;
    ELLE_LLM_PROVIDER m_forcedProvider = (ELLE_LLM_PROVIDER)-1;
    bool              m_initialized = false;

    /* Failover logic */
    ILLMProvider* SelectProvider(bool preferLocal = false);
    ILLMProvider* GetProviderById(ELLE_LLM_PROVIDER id);

    /* System prompt builder */
    std::string BuildElleSystemPrompt(const ELLE_EMOTION_STATE& emotions,
                                       const std::string& memoryContext,
                                       const std::string& goalContext);

    /* Stats tracking */
    std::atomic<uint64_t> m_totalRequests{0};
    std::atomic<uint64_t> m_totalTokens{0};
    std::atomic<uint64_t> m_totalLatencyMs{0};
    std::mutex m_statsMutex;
};

#endif /* ELLE_LLM_H */
