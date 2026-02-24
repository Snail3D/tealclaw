#include "api_proxy.h"
#include "key_store.h"
#include "config.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <memory>
#include <map>

namespace {
constexpr uintptr_t REQ_BLOCKED_SENTINEL = 0x1;
constexpr size_t PROXY_MAX_BODY_BYTES = 512 * 1024;

struct StreamContext {
    std::unique_ptr<WiFiClientSecure> tls;
    std::unique_ptr<HTTPClient> http;
    WiFiClient* stream = nullptr;

    ~StreamContext() {
        if (http) http->end();
    }
};

static std::map<AsyncWebServerRequest*, String> s_requestBodies;

bool isBlockedRequest(AsyncWebServerRequest* request) {
    return reinterpret_cast<uintptr_t>(request->_tempObject) == REQ_BLOCKED_SENTINEL;
}

bool accumulateBody(AsyncWebServerRequest* request,
                    uint8_t* data,
                    size_t len,
                    size_t index,
                    size_t total,
                    String& outBody) {
    if (isBlockedRequest(request)) {
        s_requestBodies.erase(request);
        return false;
    }

    String& accumulator = s_requestBodies[request];
    if (index == 0) {
        request->onDisconnect([request]() {
            s_requestBodies.erase(request);
        });
        accumulator = "";
    }

    if (accumulator.length() + len > PROXY_MAX_BODY_BYTES) {
        s_requestBodies.erase(request);
        request->_tempObject = reinterpret_cast<void*>(REQ_BLOCKED_SENTINEL);
        request->send(413, "application/json", "{\"error\":\"request payload too large\"}");
        return false;
    }

    accumulator.concat(reinterpret_cast<char*>(data), len);
    if (index + len < total) return false;

    outBody = accumulator;
    s_requestBodies.erase(request);
    return true;
}

size_t readUpstreamChunk(const std::shared_ptr<StreamContext>& ctx, uint8_t* buffer, size_t maxLen) {
    if (!ctx || !ctx->stream) return 0;

    unsigned long waitStart = millis();
    while (ctx->stream->available() == 0 && ctx->stream->connected()) {
        if (millis() - waitStart > PROXY_TIMEOUT_MS) {
            return 0;
        }
        delay(2);
    }

    int available = ctx->stream->available();
    if (available <= 0 && !ctx->stream->connected()) {
        return 0;
    }
    if (available <= 0) {
        return 0;
    }

    size_t toRead = min(maxLen, (size_t)PROXY_CHUNK_SIZE);
    toRead = min(toRead, (size_t)available);

    size_t bytesRead = ctx->stream->readBytes(buffer, toRead);
    if (bytesRead == 0 && !ctx->stream->connected()) {
        return 0;
    }

    return bytesRead;
}

void sendUpstreamError(AsyncWebServerRequest* request, int code, const String& message) {
    request->send(code, "application/json", String("{\"error\":\"") + message + "\"}");
}

// ── Helper: forward JSON to upstream API with Bearer auth, stream response back ──
void forwardJSON(AsyncWebServerRequest* request, const String& body,
                 const char* url, const char* apiKey, bool streaming) {
    if (!streaming) {
        WiFiClientSecure client;
        client.setInsecure();

        HTTPClient http;
        if (!http.begin(client, url)) {
            sendUpstreamError(request, 502, "upstream connect failed");
            return;
        }

        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", String("Bearer ") + apiKey);
        http.setTimeout(PROXY_TIMEOUT_MS);

        int httpCode = http.POST(body);
        if (httpCode <= 0) {
            sendUpstreamError(request, 502,
                String("upstream error: ") + http.errorToString(httpCode));
            http.end();
            return;
        }

        String contentType = http.header("Content-Type");
        if (contentType.length() == 0) contentType = "application/json";

        String responseBody = http.getString();
        request->send(httpCode, contentType, responseBody);
        http.end();
        return;
    }

    auto ctx = std::shared_ptr<StreamContext>(new StreamContext());
    ctx->tls.reset(new WiFiClientSecure());
    ctx->http.reset(new HTTPClient());

    ctx->tls->setInsecure(); // ESP32 root CA bundle is limited; TLS still encrypts

    if (!ctx->http->begin(*ctx->tls, url)) {
        sendUpstreamError(request, 502, "upstream connect failed");
        return;
    }

    ctx->http->addHeader("Content-Type", "application/json");
    ctx->http->addHeader("Authorization", String("Bearer ") + apiKey);
    ctx->http->setTimeout(PROXY_TIMEOUT_MS);

    int httpCode = ctx->http->POST(body);
    if (httpCode <= 0) {
        sendUpstreamError(request, 502,
            String("upstream error: ") + ctx->http->errorToString(httpCode));
        return;
    }

    String contentType = ctx->http->header("Content-Type");
    if (contentType.length() == 0) contentType = "application/json";

    // Fallback: if upstream didn't actually return SSE, return normal body.
    if (contentType.indexOf("text/event-stream") < 0) {
        String responseBody = ctx->http->getString();
        request->send(httpCode, contentType, responseBody);
        return;
    }

    ctx->stream = ctx->http->getStreamPtr();
    if (!ctx->stream) {
        sendUpstreamError(request, 502, "upstream stream unavailable");
        return;
    }

    AsyncWebServerResponse* response = request->beginChunkedResponse(
        "text/event-stream",
        [ctx](uint8_t* buffer, size_t maxLen, size_t index) -> size_t {
            return readUpstreamChunk(ctx, buffer, maxLen);
        }
    );
    response->addHeader("Cache-Control", "no-cache");
    request->send(response);
}

// ── Helper: forward binary response (TTS audio) ──
void forwardBinary(AsyncWebServerRequest* request, const String& body,
                   const char* url, const char* apiKey) {
    auto ctx = std::shared_ptr<StreamContext>(new StreamContext());
    ctx->tls.reset(new WiFiClientSecure());
    ctx->http.reset(new HTTPClient());

    ctx->tls->setInsecure();

    if (!ctx->http->begin(*ctx->tls, url)) {
        sendUpstreamError(request, 502, "upstream connect failed");
        return;
    }

    ctx->http->addHeader("Content-Type", "application/json");
    ctx->http->addHeader("Authorization", String("Bearer ") + apiKey);
    ctx->http->setTimeout(PROXY_TIMEOUT_MS);

    int httpCode = ctx->http->POST(body);
    if (httpCode <= 0) {
        sendUpstreamError(request, 502,
            String("upstream error: ") + ctx->http->errorToString(httpCode));
        return;
    }

    String contentType = ctx->http->header("Content-Type");
    if (contentType.length() == 0) contentType = "application/octet-stream";

    ctx->stream = ctx->http->getStreamPtr();
    if (!ctx->stream) {
        sendUpstreamError(request, 502, "upstream stream unavailable");
        return;
    }

    AsyncWebServerResponse* response = request->beginChunkedResponse(
        contentType.c_str(),
        [ctx](uint8_t* buffer, size_t maxLen, size_t index) -> size_t {
            return readUpstreamChunk(ctx, buffer, maxLen);
        }
    );
    request->send(response);
}
} // namespace

// ── Groq Chat Completions ──
void proxyGroqChat(AsyncWebServerRequest* request, uint8_t* data, size_t len,
                   size_t index, size_t total) {
    String body;
    if (!accumulateBody(request, data, len, index, total, body)) return;

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
    String body;
    if (!accumulateBody(request, data, len, index, total, body)) return;

    String apiKey = keyStore.get("groq_key");
    if (apiKey.length() == 0) {
        request->send(403, "application/json", "{\"error\":\"no API key configured\"}");
        return;
    }

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    if (!http.begin(client, GROQ_TRANSCRIBE_URL)) {
        sendUpstreamError(request, 502, "upstream connect failed");
        return;
    }

    // Forward the original content type (multipart/form-data with boundary)
    String ct = request->contentType();
    http.addHeader("Content-Type", ct);
    http.addHeader("Authorization", String("Bearer ") + apiKey);
    http.setTimeout(PROXY_TIMEOUT_MS);

    int httpCode = http.POST((uint8_t*)body.c_str(), body.length());
    if (httpCode <= 0) {
        sendUpstreamError(request, 502,
            String("upstream error: ") + http.errorToString(httpCode));
        http.end();
        return;
    }

    String contentType = http.header("Content-Type");
    if (contentType.length() == 0) contentType = "application/json";

    String resp = http.getString();
    request->send(httpCode, contentType, resp);
    http.end();
}

// ── Groq TTS (Orpheus) ──
void proxyGroqTTS(AsyncWebServerRequest* request, uint8_t* data, size_t len,
                  size_t index, size_t total) {
    String body;
    if (!accumulateBody(request, data, len, index, total, body)) return;

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
    String body;
    if (!accumulateBody(request, data, len, index, total, body)) return;

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

    auto ctx = std::shared_ptr<StreamContext>(new StreamContext());
    ctx->tls.reset(new WiFiClientSecure());
    ctx->http.reset(new HTTPClient());

    ctx->tls->setInsecure();

    if (!ctx->http->begin(*ctx->tls, url)) {
        sendUpstreamError(request, 502, "upstream connect failed");
        return;
    }

    ctx->http->addHeader("Content-Type", "application/json");
    ctx->http->setTimeout(PROXY_TIMEOUT_MS);

    int httpCode = ctx->http->POST(body);
    if (httpCode <= 0) {
        sendUpstreamError(request, 502,
            String("upstream error: ") + ctx->http->errorToString(httpCode));
        return;
    }

    String contentType = ctx->http->header("Content-Type");
    if (contentType.length() == 0) contentType = "application/json";

    ctx->stream = ctx->http->getStreamPtr();
    if (!ctx->stream) {
        sendUpstreamError(request, 502, "upstream stream unavailable");
        return;
    }

    // Gemini responses can be large (base64 images) — stream back
    AsyncWebServerResponse* response = request->beginChunkedResponse(
        contentType.c_str(),
        [ctx](uint8_t* buffer, size_t maxLen, size_t index) -> size_t {
            return readUpstreamChunk(ctx, buffer, maxLen);
        }
    );
    request->send(response);
}
