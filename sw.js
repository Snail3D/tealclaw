const CACHE = 'tealclaw-v18';
const ASSETS = ['/', '/index.html', '/manifest.json', '/icon-192.png', '/icon-512.png', '/apple-touch-icon.png', '/favicon-32.png', '/stats.html', '/llms.txt', '/llms.html'];

self.addEventListener('install', e => {
  e.waitUntil(caches.open(CACHE).then(c => c.addAll(ASSETS)));
  // Don't skipWaiting here — let the app control activation via message
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
  // Network-first for API calls, cache-first for assets
  if (e.request.url.includes('/api/') || e.request.url.includes('groq.com') || e.request.url.includes('openrouter.ai') || e.request.url.includes('elevenlabs.io') || e.request.url.includes('api.telegram.org') || e.request.url.includes('anthropic.com') || e.request.url.includes('together.xyz') || e.request.url.includes('fal.run') || e.request.url.includes('fireworks.ai') || e.request.url.includes('openai.com') || e.request.url.includes('tenor.googleapis.com')) {
    return; // Don't cache API calls
  }
  e.respondWith(
    caches.match(e.request).then(r => r || fetch(e.request))
  );
});
