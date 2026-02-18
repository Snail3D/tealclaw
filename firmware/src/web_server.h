#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class TcWebServer {
public:
    void begin(bool setupMode);
    AsyncWebServer& getServer();

private:
    AsyncWebServer _server{WEB_PORT};
    void _setupStaticRoutes();
    void _setupProxyRoutes();
    void _setupConfigRoutes();
    void _addCORS(AsyncWebServerResponse* response);
};

extern TcWebServer tcWebServer;
