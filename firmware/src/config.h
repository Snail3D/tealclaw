#pragma once

// ── WiFi AP defaults ──
#define AP_SSID_PREFIX    "TealClaw-"
#define AP_DEFAULT_PASS   "tc-default" // Legacy fallback only; runtime uses per-device default
#define AP_CHANNEL        6
#define AP_MAX_CLIENTS    4

// ── Web server ──
#define WEB_PORT          80

// ── NVS namespace ──
#define NVS_NAMESPACE     "tealclaw"

// ── GPIO ──
#define PIN_RESET_BTN     0    // BOOT button on most ESP32-S3 boards
#define PIN_LED           2    // Onboard LED (varies by board)
#define FACTORY_RESET_MS  5000 // Hold time for factory reset

// ── BLE ──
#define BLE_DEVICE_NAME   "TealClaw Vault"

// Custom service UUID: "4d6f7365-6375-7265-2d54-43564c540001"
#define BLE_SERVICE_UUID         "4d6f7365-6375-7265-2d54-43564c540001"
#define BLE_CHAR_STATUS_UUID     "4d6f7365-6375-7265-2d54-43564c540002"
#define BLE_CHAR_CONFIG_UUID     "4d6f7365-6375-7265-2d54-43564c540003"
#define BLE_CHAR_PROXIMITY_UUID  "4d6f7365-6375-7265-2d54-43564c540004"

// ── API endpoints (upstream) ──
#define GROQ_CHAT_URL      "https://api.groq.com/openai/v1/chat/completions"
#define GROQ_TRANSCRIBE_URL "https://api.groq.com/openai/v1/audio/transcriptions"
#define GROQ_TTS_URL       "https://api.groq.com/openai/v1/audio/speech"
#define GEMINI_API_HOST    "generativelanguage.googleapis.com"

// ── Proxy buffer sizes ──
#define PROXY_CHUNK_SIZE   1024
#define PROXY_TIMEOUT_MS   30000

// ── API proxy paths ──
#define PROXY_GROQ_CHAT       "/proxy/groq/chat"
#define PROXY_GROQ_TRANSCRIBE "/proxy/groq/transcribe"
#define PROXY_GROQ_TTS        "/proxy/groq/tts"
#define PROXY_GEMINI_GENERATE "/proxy/gemini/generate"
