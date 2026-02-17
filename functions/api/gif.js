/**
 * TealClaw GIF Proxy — Cloudflare Pages Function
 *
 * Proxies GIF search requests to Klipy using a server-side API key.
 * Returns either a GIF URL or a Klipy ad (HTML blob) for rendering.
 *
 * Environment variables:
 *   KLIPY_API_KEY — Klipy API key
 */

const CACHE_TTL = 300; // 5 minutes

function json(data, status = 200, cacheable = true) {
  return new Response(JSON.stringify(data), {
    status,
    headers: {
      'Content-Type': 'application/json',
      'Cache-Control': cacheable ? `public, max-age=${CACHE_TTL}` : 'no-store',
    },
  });
}

async function cidFromIP(ip) {
  const data = new TextEncoder().encode(ip + 'tc-klipy');
  const hash = await crypto.subtle.digest('SHA-256', data);
  return Array.from(new Uint8Array(hash)).map(b => b.toString(16).padStart(2, '0')).join('').slice(0, 32);
}

export async function onRequest(context) {
  const { request, env } = context;

  if (request.method === 'OPTIONS') return new Response(null, { status: 204 });
  if (request.method !== 'GET') return json({ error: 'Method not allowed' }, 405);
  if (!env.KLIPY_API_KEY) return json({ error: 'GIF API not configured. Set KLIPY_API_KEY.' }, 503);

  const reqUrl = new URL(request.url);
  const q = (reqUrl.searchParams.get('q') || '').trim().slice(0, 80);
  if (!q) return json({ url: null });

  // Customer ID for ad frequency capping (hashed IP — no PII sent to Klipy)
  const ip = request.headers.get('cf-connecting-ip') || 'anon';
  const cid = await cidFromIP(ip);

  // Check cache — only GIF responses are cached, never ads
  const cache = caches.default;
  const cacheKey = new Request(new URL(`/api/gif/cached?q=${encodeURIComponent(q)}`, request.url), request);
  const cached = await cache.match(cacheKey);
  if (cached) return cached;

  try {
    const upstream = new URL('https://api.klipy.com/v2/search');
    upstream.searchParams.set('q', q);
    upstream.searchParams.set('key', env.KLIPY_API_KEY);
    upstream.searchParams.set('limit', '8');
    upstream.searchParams.set('media_filter', 'gif');
    upstream.searchParams.set('contentfilter', 'medium');
    // Ad parameters — required for Klipy to include ads in results
    upstream.searchParams.set('customer_id', cid);
    upstream.searchParams.set('ad-min-width', '50');
    upstream.searchParams.set('ad-max-width', '400');
    upstream.searchParams.set('ad-min-height', '50');
    upstream.searchParams.set('ad-max-height', '250');
    upstream.searchParams.set('ad-iframe', '1');

    const res = await fetch(upstream.toString());
    if (!res.ok) throw new Error(`Klipy API: ${res.status}`);

    const data = await res.json();
    const results = Array.isArray(data?.results) ? data.results :
                    Array.isArray(data?.data?.data) ? data.data.data : [];
    if (!results.length) return json({ url: null });

    // Check for ad objects (Klipy controls frequency)
    const ad = results.find(r => r.type === 'ad');
    if (ad && ad.content) {
      // Never cache ads — impressions must fire fresh each time
      return json({
        url: null,
        ad: { content: ad.content, width: ad.width || 320, height: ad.height || 100 }
      }, 200, false);
    }

    // Regular GIF — filter out any non-content items
    const gifs = results.filter(r => r.type !== 'ad');
    if (!gifs.length) return json({ url: null });

    const pick = gifs[Math.floor(Math.random() * gifs.length)];
    const gifUrl = pick?.media_formats?.gif?.url || pick?.media_formats?.mediumgif?.url ||
                   pick?.file?.hd?.gif?.url || pick?.file?.md?.gif?.url || null;

    const response = json({ url: gifUrl });
    context.waitUntil(cache.put(cacheKey, response.clone()));
    return response;
  } catch (err) {
    return json({ error: err.message }, 500);
  }
}
