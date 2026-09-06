// TealClaw rebrand: the old PWA service worker is retired.
// This stub unregisters itself and clears all caches so visitors get the
// new marketing site instead of the cached legacy app.
self.addEventListener('install', () => self.skipWaiting());
self.addEventListener('activate', async (event) => {
  event.waitUntil((async () => {
    const keys = await caches.keys();
    await Promise.all(keys.map(k => caches.delete(k)));
    await self.registration.unregister();
    const clientList = await self.clients.matchAll({ type: 'window' });
    for (const client of clientList) client.navigate(client.url);
  })());
});
