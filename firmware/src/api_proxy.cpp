#include "api_proxy.h"
#include "key_store.h"
#include "config.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// ── Accumulation buffer for chunked body reception ──
// ESPAsyncWebServer delivers POST bodies in chunks via onBody callback.
// We accumulate until index+len == total, then forward the complete request.

static String _chatBody;
static String _transcribeBody;
static String _ttsBody;
static String _geminiBody;

// ── Helper: forward JSON to upstream API with Bearer auth, stream response back ──
static void forwardJSON(AsyncWebServerRequest* request, const String& body,
                        const char* url, const char* apiKey, bool streaming) {
    WiFiClientSecure* client = new WiFiClientSecure();
    client->setInsecure(); // ESP32 root CA bundle is limited; TLS still encrypts

    HTTPClient http;
    if (!http.begin(*client, url)) {
        request->send(502, "application/json", "{\"error\":\"upstream connect failed\"}");
        delete client;
        return;
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String("Bearer ") + apiKey);
    http.setTimeout(PROXY_TIMEOUT_MS);

    int httpCode = http.POST(body);
    if (httpCode <= 0) {
        request->send(502, "application/json",
            String("{\"error\":\"upstream error: ") + http.errorToString(httpCode) + "\"}");
        http.end();
        delete client;
        return;
    }

    // Get response stream
    int contentLen = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    // Determine content type from upstream
    String contentType = http.header("Content-Type");
    if (contentType.length() == 0) contentType = "application/json";

    if (streaming && contentType.indexOf("text/event-stream") >= 0) {
        // SSE streaming response
        AsyncWebServerResponse* response = request->beginChunkedResponse(
            "text/event-stream",
            [stream, client, &http](uint8_t* buffer, size_t maxLen, size_t index) -> size_t {
                if (!stream->available() && !stream->connected()) {
                    return 0; // Done
                }
                size_t toRead = min(maxLen, (size_t)PROXY_CHUNK_SIZE);
                size_t bytesRead = stream->readBytes(buffer, toRead);
                if (bytesRead == 0 && !stream->connected()) {
                    return 0; // Done
                }
                return bytesRead;
            }
        );
        response->addHeader("Cache-Control", "no-cache");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    } else {
        // Non-streaming: read full response and forward
        String responseBody = http.getString();
        AsyncWebServerResponse* response = request->beginResponse(
            httpCode, contentType, responseBody);
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    }

    http.end();
    delete client;
}

// ── Helper: forward binary response (TTS audio) ──
static void forwardBinary(AsyncWebServerRequest* request, const String& body,
                          const char* url, const char* apiKey) {
    WiFiClientSecure* client = new WiFiClientSecure();
    client->setInsecure();

    HTTPClient http;
    if (!http.begin(*client, url)) {
        request->send(502, "application/json", "{\"error\":\"upstream connect failed\"}");
        delete client;
        return;
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String("Bearer ") + apiKey);
    http.setTimeout(PROXY_TIMEOUT_MS);

    int httpCode = http.POST(body);
    if (httpCode <= 0) {
        request->send(502, "application/json",
            String("{\"error\":\"upstream error: ") + http.errorToString(httpCode) + "\"}");
        http.end();
        delete client;
        return;
    }

    String contentType = http.header("Content-Type");
    if (contentType.length() == 0) contentType = "application/octet-stream";

    WiFiClient* stream = http.getStreamPtr();

    AsyncWebServerResponse* response = request->beginChunkedResponse(
        contentType.c_str(),
        [stream, client, &http](uint8_t* buffer, size_t maxLen, size_t index) -> size_t {
            if (!stream->available() && !stream->connected()) return 0;
            size_t toRead = min(maxLen, (size_t)PROXY_CHUNK_SIZE);
            return stream->readBytes(buffer, toRead);
        }
    );
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);

    http.end();
    delete client;
}

// ── Groq Chat Completions ──
void proxyGroqChat(AsyncWebServerRequest* request, uint8_t* data, size_t len,
                   size_t index, size_t total) {
    _chatBody += String((char*)data, len);
    if (index + len < total) return; // Still receiving body

    String body = _chatBody;
    _chatBody = "";

    String apiKey = keyStore.get("groq_key");
    if (apiKey.length() == 0) {
        request->send(403, "application/json", "{\"error\":\"no API key configured\"}");
        return;
    }

    // Check if request wants streaming
    bool streaming = body.indexOf("\"stream\":true") >= 0 ||
                     body.indexOf("\"stream\": true") >= 0;

    forwardJSON(request, body, GROQ_CHAT_URL, apiKey.c_str(), streaming);
}

// ── Groq Transcription (Whisper) ──
void proxyGroqTranscribe(AsyncWebServerRequest* request, uint8_t* data, size_t len,
                         size_t index, size_t total) {
    // Whisper expects multipart/form-data — forward as-is with auth header
    _transcribeBody += String((char*)data, len);
    if (index + len < total) return;

    String body = _transcribeBody;
    _transcribeBody = "";

    String apiKey = keyStore.get("groq_key");
    if (apiKey.length() == 0) {
        request->send(403, "application/json", "{\"error\":\"no API key configured\"}");
        return;
    }

    WiFiClientSecure* client = new WiFiClientSecure();
    client->setInsecure();

    HTTPClient http;
    if (!http.begin(*client, GROQ_TRANSCRIBE_URL)) {
        request->send(502, "application/json", "{\"error\":\"upstream connect failed\"}");
        delete client;
        return;
    }

    // Forward the original content type (multipart/form-data with boundary)
    String ct = request->contentType();
    http.addHeader("Content-Type", ct);
    http.addHeader("Authorization", String("Bearer ") + apiKey);
    http.setTimeout(PROXY_TIMEOUT_MS);

    int httpCode = http.POST((uint8_t*)body.c_str(), body.length());
    if (httpCode <= 0) {
        request->send(502, "application/json",
            String("{\"error\":\"upstream error: ") + http.errorToString(httpCode) + "\"}");
    } else {
        String resp = http.getString();
        AsyncWebServerResponse* response = request->beginResponse(httpCode, "application/json", resp);
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    }

    http.end();
    delete client;
}

// ── Groq TTS (Orpheus) ──
void proxyGroqTTS(AsyncWebServerRequest* request, uint8_t* data, size_t len,
                  size_t index, size_t total) {
    _ttsBody += String((char*)data, len);
    if (index + len < total) return;

    String body = _ttsBody;
    _ttsBody = "";

    String apiKey = keyStore.get("groq_key");
    if (apiKey.length() == 0) {
        request->send(403, "application/json", "{\"error\":\"no API key configured\"}");
        return;
    }

    forwardBinary(request, body, GROQ_TTS_URL, apiKey.c_str());
}

// ── Gemini Image Generation ──
void proxyGeminiGenerate(AsyncWebServerRequest* request, uint8_t* data, size_t len,
                         size_t index, size_t total) {
    _geminiBody += String((char*)data, len);
    if (index + len < total) return;

    String body = _geminiBody;
    _geminiBody = "";

    String apiKey = keyStore.get("gemini_key");
    if (apiKey.length() == 0) {
        request->send(403, "application/json", "{\"error\":\"no Gemini key configured\"}");
        return;
    }

    // Build Gemini URL — extract model from body or use default
    String model = keyStore.get("vision_model");
    if (model.length() == 0) model = "gemini-2.0-flash-exp";

    String url = String("https://") + GEMINI_API_HOST +
                 "/v1beta/models/" + model + ":generateContent?key=" + apiKey;

    WiFiClientSecure* client = new WiFiClientSecure();
    client->setInsecure();

    HTTPClient http;
    if (!http.begin(*client, url)) {
        request->send(502, "application/json", "{\"error\":\"upstream connect failed\"}");
        delete client;
        return;
    }

    http.addHeader("Content-Type", "application/json");
    http.setTimeout(PROXY_TIMEOUT_MS);

    int httpCode = http.POST(body);
    if (httpCode <= 0) {
        request->send(502, "application/json",
            String("{\"error\":\"upstream error: ") + http.errorToString(httpCode) + "\"}");
    } else {
        // Gemini responses can be large (base64 images) — stream back
        WiFiClient* stream = http.getStreamPtr();
        String contentType = http.header("Content-Type");
        if (contentType.length() == 0) contentType = "application/json";

        AsyncWebServerResponse* response = request->beginChunkedResponse(
            contentType.c_str(),
            [stream, client, &http](uint8_t* buffer, size_t maxLen, size_t index) -> size_t {
                if (!stream->available() && !stream->connected()) return 0;
                size_t toRead = min(maxLen, (size_t)PROXY_CHUNK_SIZE);
                return stream->readBytes(buffer, toRead);
            }
        );
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    }

    http.end();
    delete client;
}
