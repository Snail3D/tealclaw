#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "config.h"

class TcWebServer {
public:
    void begin(bool setupMode);
    AsyncWebServer& getServer();

private:
    AsyncWebServer _server{WEB_PORT};
    bool _setupMode = true;
    String _sessionToken;

    void _setupStaticRoutes();
    void _setupProxyRoutes();
    void _setupConfigRoutes();

    void _addCORS(AsyncWebServerRequest* request, AsyncWebServerResponse* response);
    void _addSecurityHeaders(AsyncWebServerResponse* response);
    bool _isAllowedOrigin(AsyncWebServerRequest* request, const String& origin);
    bool _isAuthorized(AsyncWebServerRequest* request);
    void _issueSessionCookie(AsyncWebServerResponse* response);
    String _generateSessionToken();
};

extern TcWebServer tcWebServer;
