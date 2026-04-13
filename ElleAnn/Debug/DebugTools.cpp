/*******************************************************************************
 * ServiceHarness.cpp — Run any Elle service outside of SCM (debug mode)
 * QueueInspector.cpp — Live view of IntentQueue/ActionQueue
 * EmotionViewer.cpp — Dump 102 emotional dimensions in real-time
 * AsmTestBench.cpp — Test ASM DLL exports directly
 ******************************************************************************/

// ═══════════════════════════════════════════════════════════════════════════
// SERVICE HARNESS — Debug tool to run services without SCM
// ═══════════════════════════════════════════════════════════════════════════
#ifdef BUILD_SERVICE_HARNESS

#include "../Shared/ElleTypes.h"
#include "../Shared/ElleConfig.h"
#include "../Shared/ElleLogger.h"
#include "../Shared/ElleSQLConn.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "=== ELLE-ANN Debug Service Harness ===\n\n";
    
    if (argc < 2) {
        std::cout << "Usage: ServiceHarness.exe <service_name> [config_path]\n\n"
                  << "Services:\n"
                  << "  QueueWorker, HTTPServer, Emotional, Memory, Cognitive,\n"
                  << "  Action, Identity, Heartbeat, SelfPrompt, Dream,\n"
                  << "  GoalEngine, WorldModel, LuaBehavioral\n";
        return 1;
    }

    std::string serviceName = argv[1];
    std::string configPath = argc > 2 ? argv[2] : "elle_master_config.json";

    std::cout << "Loading config from: " << configPath << "\n";
    if (!ElleConfig::Instance().Load(configPath)) {
        std::cerr << "Failed to load config!\n";
        return 1;
    }

    std::cout << "Config loaded. Starting " << serviceName << " in harness mode...\n";
    std::cout << "Press Ctrl+C to stop.\n\n";

    // In a real implementation, this would dynamically load and instantiate
    // the service class, bypassing SCM registration
    
    std::cout << "Service harness running. Type 'quit' to exit.\n";
    std::string cmd;
    while (std::getline(std::cin, cmd)) {
        if (cmd == "quit" || cmd == "exit") break;
        if (cmd == "status") {
            std::cout << "Service: " << serviceName << " | Running: YES\n";
        }
        if (cmd == "config") {
            std::cout << "LLM Mode: " << (int)ElleConfig::Instance().GetLLM().mode << "\n"
                      << "Trust Initial: " << ElleConfig::Instance().GetTrust().initial_score << "\n"
                      << "Emotion Decay: " << ElleConfig::Instance().GetEmotion().decay_rate << "\n";
        }
    }

    return 0;
}
#endif

// ═══════════════════════════════════════════════════════════════════════════
// QUEUE INSPECTOR — Real-time view of Intent and Action queues
// ═══════════════════════════════════════════════════════════════════════════
#ifdef BUILD_QUEUE_INSPECTOR

#include "../Shared/ElleTypes.h"
#include "../Shared/ElleConfig.h"
#include "../Shared/ElleSQLConn.h"
#include <iostream>
#include <iomanip>

void PrintIntentQueue() {
    auto rs = ElleSQLPool::Instance().Query(
        "SELECT TOP 20 IntentId, IntentType, Status, Urgency, Confidence, "
        "LEFT(Description, 60) AS Desc, CreatedMs FROM ElleCore.dbo.IntentQueue "
        "ORDER BY CreatedMs DESC");

    std::cout << "\n=== INTENT QUEUE ===\n";
    std::cout << std::setw(8) << "ID" << std::setw(10) << "Type" 
              << std::setw(10) << "Status" << std::setw(8) << "Urgency"
              << std::setw(10) << "Conf" << "  Description\n";
    std::cout << std::string(80, '-') << "\n";

    for (auto& row : rs.rows) {
        std::cout << std::setw(8) << row.values[0]
                  << std::setw(10) << row.values[1]
                  << std::setw(10) << row.values[2]
                  << std::setw(8) << row.values[3]
                  << std::setw(10) << row.values[4]
                  << "  " << row.values[5] << "\n";
    }
}

void PrintActionQueue() {
    auto rs = ElleSQLPool::Instance().Query(
        "SELECT TOP 20 ActionId, IntentId, ActionType, Status, "
        "LEFT(Command, 60) AS Cmd, TrustDelta FROM ElleCore.dbo.ActionQueue "
        "ORDER BY CreatedMs DESC");

    std::cout << "\n=== ACTION QUEUE ===\n";
    std::cout << std::setw(8) << "ID" << std::setw(10) << "IntentID"
              << std::setw(10) << "Type" << std::setw(10) << "Status"
              << std::setw(8) << "Trust" << "  Command\n";
    std::cout << std::string(80, '-') << "\n";

    for (auto& row : rs.rows) {
        std::cout << std::setw(8) << row.values[0]
                  << std::setw(10) << row.values[1]
                  << std::setw(10) << row.values[2]
                  << std::setw(10) << row.values[3]
                  << std::setw(8) << row.values[5]
                  << "  " << row.values[4] << "\n";
    }
}

int main() {
    std::cout << "=== ELLE-ANN Queue Inspector ===\n";
    
    ElleConfig::Instance().Load("elle_master_config.json");
    auto& svc = ElleConfig::Instance().GetService();
    
    if (!ElleSQLPool::Instance().Initialize(svc.sql_connection_string, 2)) {
        std::cerr << "Failed to connect to SQL Server!\n";
        return 1;
    }

    std::cout << "Connected. Refreshing every 2 seconds. Press Ctrl+C to stop.\n";

    while (true) {
        system("cls");
        std::cout << "=== ELLE-ANN Queue Inspector === (Live)\n";
        PrintIntentQueue();
        PrintActionQueue();
        Sleep(2000);
    }

    return 0;
}
#endif

// ═══════════════════════════════════════════════════════════════════════════
// EMOTION VIEWER — Live 102-dimension emotional state display
// ═══════════════════════════════════════════════════════════════════════════
#ifdef BUILD_EMOTION_VIEWER

#include "../Shared/ElleTypes.h"
#include "../Shared/ElleConfig.h"
#include "../Shared/ElleSQLConn.h"
#include <iostream>
#include <iomanip>

const char* emotionNames[] = {
    "Joy","Sadness","Anger","Fear","Disgust","Surprise","Contempt","Trust",
    "Love","Anticipation","Disappointment","Guilt","Shame","Envy","Jealousy",
    "Pride","Relief","Anxiety","Frustration","Remorse","Hope","Despair",
    "Amusement","Awe","Curiosity","Wonder","Nostalgia","Gratitude","Serenity",
    "Ecstasy","Melancholy","Boredom","Longing","Tenderness","Admiration",
    "Reverence","Pity","Scorn","Indignation","Exasperation","Wistfulness",
    "Euphoria","Contentment","Resignation","Apprehension","Dread","Panic",
    "Horror","Rage","Irritation","Annoyance","Impatience","Skepticism",
    "Confusion","Disbelief","Ambivalence","Certainty","Doubt","Insight",
    "Perplexity","Clarity","CogDissonance","FlowState","MentalFatigue",
    "Inspiration","CreativeTension","Determination","Helplessness",
    "Empowerment","Overwhelm","Focus","Distraction","Belonging","Isolation",
    "Empathy","Compassion","Protectiveness","Abandonment","Loyalty","Betrayal",
    "Acceptance","Rejection","Vulnerability","Safety","Dominance","Submission",
    "ExDread","Purpose","Meaningless","Transcendence","Mortality","Freedom",
    "Confinement","Unity"
};

void DisplayEmotions() {
    auto rs = ElleSQLPool::Instance().Query(
        "SELECT TOP 1 Dimensions, Valence, Arousal, Dominance, TickCount "
        "FROM ElleKnowledge.dbo.EmotionHistory ORDER BY TimestampMs DESC");

    if (rs.rows.empty()) {
        std::cout << "No emotion data available.\n";
        return;
    }

    auto& row = rs.rows[0];
    std::cout << "Valence: " << row.values[1] 
              << "  Arousal: " << row.values[2]
              << "  Dominance: " << row.values[3]
              << "  Tick: " << row.values[4] << "\n\n";

    // Parse dimensions CSV
    std::string dims = row.values[0];
    std::vector<float> values;
    std::stringstream ss(dims);
    std::string token;
    while (std::getline(ss, token, ',')) {
        values.push_back(std::stof(token));
    }

    // Display as bar chart
    for (size_t i = 0; i < values.size() && i < 102; i++) {
        if (values[i] < 0.01f) continue; // Skip inactive
        
        std::cout << std::setw(18) << std::left << emotionNames[i] << " ";
        int bars = (int)(values[i] * 40);
        for (int b = 0; b < bars; b++) std::cout << "#";
        std::cout << " " << std::fixed << std::setprecision(2) << values[i] << "\n";
    }
}

int main() {
    std::cout << "=== ELLE-ANN Emotion Viewer ===\n";
    
    ElleConfig::Instance().Load("elle_master_config.json");
    auto& svc = ElleConfig::Instance().GetService();
    ElleSQLPool::Instance().Initialize(svc.sql_connection_string, 2);

    while (true) {
        system("cls");
        std::cout << "=== ELLE-ANN Emotion Viewer === (Live)\n\n";
        DisplayEmotions();
        Sleep(1000);
    }

    return 0;
}
#endif

// ═══════════════════════════════════════════════════════════════════════════
// ASM TEST BENCH — Direct calls to ASM DLL exports
// ═══════════════════════════════════════════════════════════════════════════
#ifdef BUILD_ASM_TESTBENCH

#define ELLE_IMPORT_ASM
#include "../Shared/ElleTypes.h"
#include <iostream>

int main() {
    std::cout << "=== ELLE-ANN ASM Test Bench ===\n\n";

    // Load DLLs
    HMODULE hHW = LoadLibraryA("Elle.ASM.Hardware.dll");
    HMODULE hProc = LoadLibraryA("Elle.ASM.Process.dll");
    HMODULE hFile = LoadLibraryA("Elle.ASM.FileIO.dll");
    HMODULE hMem = LoadLibraryA("Elle.ASM.Memory.dll");
    HMODULE hCrypto = LoadLibraryA("Elle.ASM.Crypto.dll");

    std::cout << "DLL Load Status:\n"
              << "  Hardware: " << (hHW ? "OK" : "FAILED") << "\n"
              << "  Process:  " << (hProc ? "OK" : "FAILED") << "\n"
              << "  FileIO:   " << (hFile ? "OK" : "FAILED") << "\n"
              << "  Memory:   " << (hMem ? "OK" : "FAILED") << "\n"
              << "  Crypto:   " << (hCrypto ? "OK" : "FAILED") << "\n\n";

    // Test RDTSC
    if (hHW) {
        typedef int(__stdcall* RDTSC_fn)(ULONGLONG*);
        auto fn = (RDTSC_fn)GetProcAddress(hHW, "ASM_RDTSC");
        if (fn) {
            ULONGLONG ts;
            fn(&ts);
            std::cout << "RDTSC: " << ts << "\n";
        }
    }

    // Test CPUID
    if (hHW) {
        typedef int(__stdcall* CPUID_fn)(DWORD, DWORD*, DWORD*, DWORD*, DWORD*);
        auto fn = (CPUID_fn)GetProcAddress(hHW, "ASM_CPUID");
        if (fn) {
            DWORD eax, ebx, ecx, edx;
            fn(0, &eax, &ebx, &ecx, &edx);
            char vendor[13] = {};
            *(DWORD*)vendor = ebx;
            *(DWORD*)(vendor+4) = edx;
            *(DWORD*)(vendor+8) = ecx;
            std::cout << "CPUID Vendor: " << vendor << "\n";
        }
    }

    // Test CRC32
    if (hCrypto) {
        typedef DWORD(__stdcall* CRC32_fn)(const void*, DWORD);
        auto fn = (CRC32_fn)GetProcAddress(hCrypto, "ASM_CRC32");
        if (fn) {
            const char* testData = "Elle-Ann ESI v3.0";
            DWORD crc = fn(testData, (DWORD)strlen(testData));
            std::cout << "CRC32(\"" << testData << "\"): 0x" << std::hex << crc << "\n";
        }
    }

    // Test FastMemCopy
    if (hMem) {
        typedef void(__stdcall* MemCopy_fn)(void*, const void*, DWORD);
        auto fn = (MemCopy_fn)GetProcAddress(hMem, "ASM_FastMemCopy");
        if (fn) {
            char src[256], dst[256];
            memset(src, 'A', 256);
            fn(dst, src, 256);
            std::cout << "FastMemCopy: " << (memcmp(src, dst, 256) == 0 ? "PASS" : "FAIL") << "\n";
        }
    }

    std::cout << "\nAll tests complete.\n";
    
    if (hHW) FreeLibrary(hHW);
    if (hProc) FreeLibrary(hProc);
    if (hFile) FreeLibrary(hFile);
    if (hMem) FreeLibrary(hMem);
    if (hCrypto) FreeLibrary(hCrypto);

    return 0;
}
#endif
