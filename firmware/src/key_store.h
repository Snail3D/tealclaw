#pragma once
#include <Arduino.h>
#include <Preferences.h>

class KeyStore {
public:
    void begin();
    void set(const char* key, const String& value);
    String get(const char* key);
    bool has(const char* key);
    bool hasKeys();   // true if at least groq_key is set
    void clear();     // factory reset — wipes entire namespace
    void end();

private:
    Preferences _prefs;
};

extern KeyStore keyStore;
