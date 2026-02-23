const CACHE='tealclaw-v218';
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
    // Network-first for navigations (keeps HTML un-cached), with a safe offline fallback.
    // If the user is offline, serve the cached /guest.html shell instead of failing.
    e.respondWith(
      fetch(e.request).catch(() => caches.match('/guest.html'))
    );
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

  // Cache-first for same-origin static assets, with runtime caching.
  // - ignoreSearch lets cached assets still resolve when URL has cache-busting query params.
  // - We only cache GET + same-origin + “static-ish” request destinations.
  e.respondWith((async () => {
    if (e.request.method !== 'GET') return fetch(e.request);

    const url = new URL(e.request.url);
    const sameOrigin = url.origin === self.location.origin;
    const dest = e.request.destination; // '' | 'script' | 'style' | 'image' | 'font' | etc.
    const isStatic = dest === 'script' || dest === 'style' || dest === 'image' || dest === 'font' || dest === 'manifest';

    if (!sameOrigin || !isStatic) {
      return fetch(e.request);
    }

    const cached = await caches.match(e.request, { ignoreSearch: true });
    if (cached) return cached;

    const res = await fetch(e.request);

    // Only cache successful, same-origin, non-opaque responses.
    if (res && res.ok && res.type === 'basic') {
      const cache = await caches.open(CACHE);
      cache.put(e.request, res.clone());
    }

    return res;
  })());
});
