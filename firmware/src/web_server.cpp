#include "web_server.h"
#include "key_store.h"
#include "api_proxy.h"
#include "config.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <WiFi.h>

namespace {
constexpr size_t MAX_CONFIG_BODY_BYTES = 16 * 1024;
constexpr uintptr_t REQ_BLOCKED_SENTINEL = 0x1;
constexpr uintptr_t REQ_REJECTED_SENTINEL = 0x2;

String extractCookieValue(const String& cookieHeader, const char* key) {
    String needle = String(key) + "=";
    int from = 0;

    while (from >= 0 && from < (int)cookieHeader.length()) {
        int pos = cookieHeader.indexOf(needle, from);
        if (pos < 0) return "";

        // Ensure token boundary (start or '; ')
        if (pos == 0 || cookieHeader[pos - 1] == ';' || cookieHeader[pos - 1] == ' ') {
            int valueStart = pos + needle.length();
            int valueEnd = cookieHeader.indexOf(';', valueStart);
            if (valueEnd < 0) valueEnd = cookieHeader.length();
            String value = cookieHeader.substring(valueStart, valueEnd);
            value.trim();
            return value;
        }

        from = pos + 1;
    }

    return "";
}
} // namespace

TcWebServer tcWebServer;

String TcWebServer::_generateSessionToken() {
    char token[33];
    snprintf(token, sizeof(token), "%08lx%08lx%08lx%08lx",
             (unsigned long)esp_random(),
             (unsigned long)esp_random(),
             (unsigned long)esp_random(),
             (unsigned long)esp_random());
    return String(token);
}

void TcWebServer::_issueSessionCookie(AsyncWebServerResponse* response) {
    String cookie = String("tc_session=") + _sessionToken +
                    "; Path=/; HttpOnly; SameSite=Strict";
    response->addHeader("Set-Cookie", cookie);
}

void TcWebServer::_addSecurityHeaders(AsyncWebServerResponse* response) {
    response->addHeader("X-Content-Type-Options", "nosniff");
    response->addHeader("X-Frame-Options", "DENY");
    response->addHeader("Referrer-Policy", "no-referrer");
}

bool TcWebServer::_isAllowedOrigin(AsyncWebServerRequest* request, const String& origin) {
    if (origin.length() == 0) return true;

    String host = request->host();
    String expectedHttp = String("http://") + host;
    String expectedHttps = String("https://") + host;

    if (origin.equalsIgnoreCase(expectedHttp) || origin.equalsIgnoreCase(expectedHttps)) {
        return true;
    }

    String apOrigin = String("http://") + WiFi.softAPIP().toString();
    String staOrigin = String("http://") + WiFi.localIP().toString();
    if (origin.equalsIgnoreCase(apOrigin) || origin.equalsIgnoreCase(staOrigin)) {
        return true;
    }

    return false;
}

void TcWebServer::_addCORS(AsyncWebServerRequest* request, AsyncWebServerResponse* response) {
    String origin = request->header("Origin");
    if (_isAllowedOrigin(request, origin) && origin.length() > 0) {
        response->addHeader("Access-Control-Allow-Origin", origin);
        response->addHeader("Vary", "Origin");
        response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        response->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }
}

bool TcWebServer::_isAuthorized(AsyncWebServerRequest* request) {
    String origin = request->header("Origin");
    if (!_isAllowedOrigin(request, origin)) return false;

    String cookieHeader = request->header("Cookie");
    String token = extractCookieValue(cookieHeader, "tc_session");
    return token.length() > 0 && token == _sessionToken;
}

void TcWebServer::_setupStaticRoutes() {
    // Serve gzipped index.html at root
    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(
            LittleFS, "/index.html.gz", "text/html");
        response->addHeader("Content-Encoding", "gzip");
        response->addHeader("X-TC-ESP32", "true");
        _issueSessionCookie(response);
        _addSecurityHeaders(response);
        request->send(response);
    });

    // Static assets from LittleFS
    _server.on("/manifest.json", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(LittleFS, "/manifest.json", "application/json");
    });
    _server.on("/sw.js", HTTP_GET, [this](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(
            LittleFS, "/sw.js", "application/javascript");
        response->addHeader("Service-Worker-Allowed", "/");
        _addSecurityHeaders(response);
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
    _server.on("/setup", HTTP_GET, [this](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(
            LittleFS, "/setup.html", "text/html");
        _issueSessionCookie(response);
        _addSecurityHeaders(response);
        request->send(response);
    });
}

void TcWebServer::_setupProxyRoutes() {
    // CORS preflight for all proxy routes
    _server.on("/proxy/*", HTTP_OPTIONS, [this](AsyncWebServerRequest* request) {
        String origin = request->header("Origin");
        if (!_isAllowedOrigin(request, origin)) {
            request->send(403, "application/json", "{\"error\":\"origin not allowed\"}");
            return;
        }

        AsyncWebServerResponse* response = request->beginResponse(204);
        _addCORS(request, response);
        response->addHeader("Access-Control-Max-Age", "86400");
        request->send(response);
    });

    // Groq Chat Completions
    _server.on(PROXY_GROQ_CHAT, HTTP_POST,
        [this](AsyncWebServerRequest* request) {
            if (!_isAuthorized(request)) {
                request->_tempObject = reinterpret_cast<void*>(REQ_BLOCKED_SENTINEL);
                AsyncWebServerResponse* response = request->beginResponse(
                    401, "application/json", "{\"error\":\"unauthorized\"}");
                _addCORS(request, response);
                request->send(response);
                return;
            }
            request->_tempObject = nullptr;
        },
        nullptr, proxyGroqChat);

    // Groq Transcription (Whisper)
    _server.on(PROXY_GROQ_TRANSCRIBE, HTTP_POST,
        [this](AsyncWebServerRequest* request) {
            if (!_isAuthorized(request)) {
                request->_tempObject = reinterpret_cast<void*>(REQ_BLOCKED_SENTINEL);
                AsyncWebServerResponse* response = request->beginResponse(
                    401, "application/json", "{\"error\":\"unauthorized\"}");
                _addCORS(request, response);
                request->send(response);
                return;
            }
            request->_tempObject = nullptr;
        },
        nullptr, proxyGroqTranscribe);

    // Groq TTS
    _server.on(PROXY_GROQ_TTS, HTTP_POST,
        [this](AsyncWebServerRequest* request) {
            if (!_isAuthorized(request)) {
                request->_tempObject = reinterpret_cast<void*>(REQ_BLOCKED_SENTINEL);
                AsyncWebServerResponse* response = request->beginResponse(
                    401, "application/json", "{\"error\":\"unauthorized\"}");
                _addCORS(request, response);
                request->send(response);
                return;
            }
            request->_tempObject = nullptr;
        },
        nullptr, proxyGroqTTS);

    // Gemini Image Generation
    _server.on(PROXY_GEMINI_GENERATE, HTTP_POST,
        [this](AsyncWebServerRequest* request) {
            if (!_isAuthorized(request)) {
                request->_tempObject = reinterpret_cast<void*>(REQ_BLOCKED_SENTINEL);
                AsyncWebServerResponse* response = request->beginResponse(
                    401, "application/json", "{\"error\":\"unauthorized\"}");
                _addCORS(request, response);
                request->send(response);
                return;
            }
            request->_tempObject = nullptr;
        },
        nullptr, proxyGeminiGenerate);
}

void TcWebServer::_setupConfigRoutes() {
    // GET /api/config — return non-sensitive config
    _server.on("/api/config", HTTP_GET, [this](AsyncWebServerRequest* request) {
        if (!_isAuthorized(request)) {
            AsyncWebServerResponse* response = request->beginResponse(
                401, "application/json", "{\"error\":\"unauthorized\"}");
            _addCORS(request, response);
            request->send(response);
            return;
        }

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
        _addCORS(request, response);
        request->send(response);
    });

    // POST /api/config — write config values
    _server.on("/api/config", HTTP_POST,
        [](AsyncWebServerRequest* request) {},
        nullptr,
        [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
            if (!_isAuthorized(request)) {
                if (index == 0) {
                    request->_tempObject = reinterpret_cast<void*>(REQ_REJECTED_SENTINEL);
                    AsyncWebServerResponse* response = request->beginResponse(
                        401, "application/json", "{\"error\":\"unauthorized\"}");
                    _addCORS(request, response);
                    request->send(response);
                }
                return;
            }

            if (reinterpret_cast<uintptr_t>(request->_tempObject) == REQ_REJECTED_SENTINEL) {
                return;
            }

            if (index == 0) {
                if (request->_tempObject) {
                    delete static_cast<String*>(request->_tempObject);
                    request->_tempObject = nullptr;
                }
                request->_tempObject = new String();
                request->onDisconnect([request]() {
                    uintptr_t marker = reinterpret_cast<uintptr_t>(request->_tempObject);
                    if (marker == REQ_REJECTED_SENTINEL || marker == REQ_BLOCKED_SENTINEL || request->_tempObject == nullptr) {
                        return;
                    }
                    delete static_cast<String*>(request->_tempObject);
                    request->_tempObject = nullptr;
                });
            }

            auto* configBody = static_cast<String*>(request->_tempObject);
            if (!configBody) {
                request->_tempObject = reinterpret_cast<void*>(REQ_REJECTED_SENTINEL);
                AsyncWebServerResponse* response = request->beginResponse(
                    500, "application/json", "{\"error\":\"buffer alloc failed\"}");
                _addCORS(request, response);
                request->send(response);
                return;
            }

            if (configBody->length() + len > MAX_CONFIG_BODY_BYTES) {
                delete configBody;
                request->_tempObject = reinterpret_cast<void*>(REQ_REJECTED_SENTINEL);

                AsyncWebServerResponse* response = request->beginResponse(
                    413, "application/json", "{\"error\":\"config payload too large\"}");
                _addCORS(request, response);
                request->send(response);
                return;
            }

            configBody->concat(reinterpret_cast<char*>(data), len);
            if (index + len < total) return;

            String body = *configBody;
            delete configBody;
            request->_tempObject = nullptr;

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, body);
            if (err) {
                AsyncWebServerResponse* response = request->beginResponse(
                    400, "application/json", "{\"error\":\"invalid JSON\"}");
                _addCORS(request, response);
                request->send(response);
                return;
            }

            // Save each provided key
            const char* keys[] = {
                "groq_key", "gemini_key", "tg_token", "tg_chat_id",
                "sys_prompt", "ai_model", "vision_model", "tts_voice",
                "wifi_ssid", "wifi_pass_sta", "wifi_pass"
            };
            for (const char* k : keys) {
                if (doc[k].is<const char*>()) {
                    String value = doc[k].as<String>();
                    if (value.length() <= 4096) {
                        keyStore.set(k, value);
                    }
                }
            }

            AsyncWebServerResponse* response = request->beginResponse(
                200, "application/json", "{\"ok\":true}");
            _addCORS(request, response);
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
        _addCORS(request, response);
        request->send(response);
    });

    // POST /api/reboot — reboot only (no key wipe)
    _server.on("/api/reboot", HTTP_POST, [this](AsyncWebServerRequest* request) {
        if (!_isAuthorized(request)) {
            AsyncWebServerResponse* response = request->beginResponse(
                401, "application/json", "{\"error\":\"unauthorized\"}");
            _addCORS(request, response);
            request->send(response);
            return;
        }

        AsyncWebServerResponse* response = request->beginResponse(
            200, "application/json", "{\"ok\":true,\"msg\":\"rebooting\"}");
        _addCORS(request, response);
        request->send(response);

        delay(300);
        ESP.restart();
    });

    // POST /api/reset — factory reset (NVS clear + reboot)
    _server.on("/api/reset", HTTP_POST, [this](AsyncWebServerRequest* request) {
        if (!_isAuthorized(request)) {
            AsyncWebServerResponse* response = request->beginResponse(
                401, "application/json", "{\"error\":\"unauthorized\"}");
            _addCORS(request, response);
            request->send(response);
            return;
        }

        AsyncWebServerResponse* response = request->beginResponse(
            200, "application/json", "{\"ok\":true,\"msg\":\"factory reset\"}");
        _addCORS(request, response);
        request->send(response);

        delay(300);
        keyStore.clear();
        ESP.restart();
    });

    // CORS preflight for /api/*
    _server.on("/api/*", HTTP_OPTIONS, [this](AsyncWebServerRequest* request) {
        String origin = request->header("Origin");
        if (!_isAllowedOrigin(request, origin)) {
            request->send(403, "application/json", "{\"error\":\"origin not allowed\"}");
            return;
        }

        AsyncWebServerResponse* response = request->beginResponse(204);
        _addCORS(request, response);
        response->addHeader("Access-Control-Max-Age", "86400");
        request->send(response);
    });
}

void TcWebServer::begin(bool setupMode) {
    _setupMode = setupMode;
    _sessionToken = _generateSessionToken();

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
