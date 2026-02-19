const CACHE = 'tealclaw-v182';
// IMPORTANT: Do NOT pre-cache navigation HTML (/, /index.html). If a bad build ever ships,
// caching can “brick” the app for users until they manually clear site data.
const ASSETS = ['/guest.html', '/manifest.json', '/icon-192.png', '/icon-512.png', '/apple-touch-icon.png', '/favicon-32.png'];

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
  // Always fetch navigation from the network.
  // This prevents a cached index.html from “bricking” the app.
  if (e.request.mode === 'navigate') {
    e.respondWith(fetch(e.request));
    return;
  }

  // Don't cache API calls
  if (
    e.request.url.includes('/api/') ||
    e.request.url.includes('groq.com') ||
    e.request.url.includes('api.telegram.org') ||
    e.request.url.includes('anthropic.com') ||
    e.request.url.includes('together.xyz') ||
    e.request.url.includes('fal.run') ||
    e.request.url.includes('fireworks.ai') ||
    e.request.url.includes('api.klipy.com') ||
    e.request.url.includes('generativelanguage.googleapis.com') ||
    e.request.url.includes('openrouter.ai')
  ) {
    return;
  }

  // Cache-first for static assets
  e.respondWith(caches.match(e.request).then(r => r || fetch(e.request)));
});
