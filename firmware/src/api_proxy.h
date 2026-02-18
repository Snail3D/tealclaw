#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>

void proxyGroqChat(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);
void proxyGroqTranscribe(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);
void proxyGroqTTS(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);
void proxyGeminiGenerate(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total);
