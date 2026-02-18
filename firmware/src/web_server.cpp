#include "web_server.h"
#include "key_store.h"
#include "api_proxy.h"
#include "config.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

TcWebServer tcWebServer;

void TcWebServer::_addCORS(AsyncWebServerResponse* response) {
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

void TcWebServer::_setupStaticRoutes() {
    // Serve gzipped index.html at root
    _server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(
            LittleFS, "/index.html.gz", "text/html");
        response->addHeader("Content-Encoding", "gzip");
        // Inject ESP32 mode meta tag via a wrapper approach isn't possible with gzip,
        // so we set a custom header instead — the UI checks for it
        response->addHeader("X-TC-ESP32", "true");
        request->send(response);
    });

    // Static assets from LittleFS
    _server.on("/manifest.json", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(LittleFS, "/manifest.json", "application/json");
    });
    _server.on("/sw.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(
            LittleFS, "/sw.js", "application/javascript");
        response->addHeader("Service-Worker-Allowed", "/");
        request->send(response);
    });
    _server.on("/icon-192.png", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(LittleFS, "/icon-192.png", "image/png");
    });
    _server.on("/icon-512.png", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(LittleFS, "/icon-512.png", "image/png");
    });
    _server.on("/apple-touch-icon.png", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(LittleFS, "/apple-touch-icon.png", "image/png");
    });
    _server.on("/favicon-32.png", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(LittleFS, "/favicon-32.png", "image/png");
    });

    // Setup page (captive portal)
    _server.on("/setup", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(LittleFS, "/setup.html", "text/html");
    });
}

void TcWebServer::_setupProxyRoutes() {
    // CORS preflight for all proxy routes
    _server.on("/proxy/*", HTTP_OPTIONS, [this](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(204);
        _addCORS(response);
        response->addHeader("Access-Control-Max-Age", "86400");
        request->send(response);
    });

    // Groq Chat Completions
    _server.on(PROXY_GROQ_CHAT, HTTP_POST,
        [](AsyncWebServerRequest* request) {},  // send handled in onBody
        nullptr, proxyGroqChat);

    // Groq Transcription (Whisper)
    _server.on(PROXY_GROQ_TRANSCRIBE, HTTP_POST,
        [](AsyncWebServerRequest* request) {},
        nullptr, proxyGroqTranscribe);

    // Groq TTS
    _server.on(PROXY_GROQ_TTS, HTTP_POST,
        [](AsyncWebServerRequest* request) {},
        nullptr, proxyGroqTTS);

    // Gemini Image Generation
    _server.on(PROXY_GEMINI_GENERATE, HTTP_POST,
        [](AsyncWebServerRequest* request) {},
        nullptr, proxyGeminiGenerate);
}

void TcWebServer::_setupConfigRoutes() {
    // GET /api/config — return non-sensitive config
    _server.on("/api/config", HTTP_GET, [this](AsyncWebServerRequest* request) {
        JsonDocument doc;
        doc["ai_model"] = keyStore.get("ai_model");
        doc["vision_model"] = keyStore.get("vision_model");
        doc["tts_voice"] = keyStore.get("tts_voice");
        doc["sys_prompt"] = keyStore.get("sys_prompt");
        doc["has_groq_key"] = keyStore.has("groq_key");
        doc["has_gemini_key"] = keyStore.has("gemini_key");
        doc["has_tg_token"] = keyStore.has("tg_token");

        String json;
        serializeJson(doc, json);

        AsyncWebServerResponse* response = request->beginResponse(200, "application/json", json);
        _addCORS(response);
        request->send(response);
    });

    // POST /api/config — write config values
    _server.on("/api/config", HTTP_POST,
        [this](AsyncWebServerRequest* request) {},
        nullptr,
        [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
            // Simple: just accumulate and process on final chunk
            static String configBody;
            configBody += String((char*)data, len);
            if (index + len < total) return;

            String body = configBody;
            configBody = "";

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, body);
            if (err) {
                request->send(400, "application/json", "{\"error\":\"invalid JSON\"}");
                return;
            }

            // Save each provided key
            const char* keys[] = {
                "groq_key", "gemini_key", "tg_token", "tg_chat_id",
                "sys_prompt", "ai_model", "vision_model", "tts_voice",
                "wifi_ssid", "wifi_pass"
            };
            for (const char* k : keys) {
                if (doc[k].is<const char*>()) {
                    keyStore.set(k, doc[k].as<String>());
                }
            }

            AsyncWebServerResponse* response = request->beginResponse(
                200, "application/json", "{\"ok\":true}");
            _addCORS(response);
            request->send(response);
        }
    );

    // GET /api/status — device info
    _server.on("/api/status", HTTP_GET, [this](AsyncWebServerRequest* request) {
        JsonDocument doc;
        doc["uptime"] = millis() / 1000;
        doc["heap_free"] = ESP.getFreeHeap();
        doc["psram_free"] = ESP.getFreePsram();
        doc["wifi_sta"] = WiFi.status() == WL_CONNECTED;
        doc["wifi_sta_ip"] = WiFi.localIP().toString();
        doc["keys_configured"] = keyStore.hasKeys();

        String json;
        serializeJson(doc, json);

        AsyncWebServerResponse* response = request->beginResponse(200, "application/json", json);
        _addCORS(response);
        request->send(response);
    });

    // POST /api/reset — factory reset (NVS clear + reboot)
    _server.on("/api/reset", HTTP_POST, [this](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(
            200, "application/json", "{\"ok\":true,\"msg\":\"resetting\"}");
        _addCORS(response);
        request->send(response);

        delay(500);
        keyStore.clear();
        ESP.restart();
    });

    // CORS preflight for /api/*
    _server.on("/api/*", HTTP_OPTIONS, [this](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(204);
        _addCORS(response);
        response->addHeader("Access-Control-Max-Age", "86400");
        request->send(response);
    });
}

void TcWebServer::begin(bool setupMode) {
    _setupStaticRoutes();
    _setupConfigRoutes();

    if (!setupMode) {
        _setupProxyRoutes();
    }

    // Captive portal: redirect all unknown requests to /setup or /
    _server.onNotFound([setupMode](AsyncWebServerRequest* request) {
        String target = setupMode ? "/setup" : "/";
        request->redirect(target);
    });

    _server.begin();
    Serial.printf("[Web] Server started on port %d (mode: %s)\n",
        WEB_PORT, setupMode ? "setup" : "gateway");
}

AsyncWebServer& TcWebServer::getServer() {
    return _server;
}
