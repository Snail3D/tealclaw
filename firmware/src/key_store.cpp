#include "key_store.h"
#include "config.h"

KeyStore keyStore;

void KeyStore::begin() {
    _prefs.begin(NVS_NAMESPACE, false);
}

void KeyStore::set(const char* key, const String& value) {
    _prefs.putString(key, value);
}

String KeyStore::get(const char* key) {
    return _prefs.getString(key, "");
}

bool KeyStore::has(const char* key) {
    return _prefs.isKey(key);
}

bool KeyStore::hasKeys() {
    String k = _prefs.getString("groq_key", "");
    return k.length() > 0;
}

void KeyStore::clear() {
    _prefs.clear();
}

void KeyStore::end() {
    _prefs.end();
}
