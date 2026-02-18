// TealClaw Service Worker — ESP32 Hardware Vault Edition
// Routes API calls to local ESP32 proxy instead of external providers
const CACHE = 'tealclaw-esp32-v1';
const ASSETS = ['/', '/index.html', '/manifest.json', '/icon-192.png', '/icon-512.png', '/apple-touch-icon.png', '/favicon-32.png'];

self.addEventListener('install', e => {
  e.waitUntil(caches.open(CACHE).then(c => c.addAll(ASSETS)));
  self.skipWaiting();
});

self.addEventListener('activate', e => {
  e.waitUntil(
    caches.keys().then(keys =>
      Promise.all(keys.filter(k => k !== CACHE).map(k => caches.delete(k)))
    )
  );
  self.clients.claim();
});

self.addEventListener('message', e => {
  if (e.data === 'skipWaiting') self.skipWaiting();
});

self.addEventListener('fetch', e => {
  const url = e.request.url;

  // Network-first for proxy API calls (never cache these)
  if (url.includes('/proxy/') || url.includes('/api/')) {
    return; // Let fetch go directly to network
  }

  // Cache-first for static assets
  e.respondWith(
    caches.match(e.request).then(r => r || fetch(e.request))
  );
});
