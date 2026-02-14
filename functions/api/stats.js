/**
 * TealClaw Stats — Cloudflare Pages Function
 *
 * Queries Cloudflare Analytics GraphQL API for daily page views.
 *
 * Environment variables (set in Cloudflare Pages > Settings > Environment variables):
 *   CF_STATS_TOKEN — Cloudflare API token with Analytics:Read permission
 *   CF_ZONE_ID    — Zone ID for tealclaw.ai (Dashboard > Overview > right sidebar)
 */

const CACHE_TTL = 300; // 5 minutes
const LAUNCH_DATE = '2025-06-01';

export async function onRequest(context) {
  const { request, env } = context;

  if (request.method === 'OPTIONS') {
    return new Response(null, { status: 204 });
  }

  if (!env.CF_STATS_TOKEN || !env.CF_ZONE_ID) {
    return Response.json(
      { error: 'Analytics not configured. Set CF_STATS_TOKEN and CF_ZONE_ID in Pages environment variables.' },
      { status: 503 }
    );
  }

  // Check cache
  const cache = caches.default;
  const cacheKey = new Request(new URL('/api/stats/cached', request.url), request);
  const cached = await cache.match(cacheKey);
  if (cached) return cached;

  try {
    const now = new Date();
    const today = now.toISOString().split('T')[0];
    const thirtyAgo = new Date(now - 30 * 86400000).toISOString().split('T')[0];

    // Request daily granularity with dimensions for the chart
    const query = `{
      viewer {
        zones(filter: { zoneTag: "${env.CF_ZONE_ID}" }) {
          daily: httpRequests1dGroups(
            limit: 730
            orderBy: [date_ASC]
            filter: { date_geq: "${LAUNCH_DATE}" }
          ) {
            dimensions { date }
            sum { requests pageViews }
            uniq { uniques }
          }
          today: httpRequests1dGroups(
            limit: 1
            filter: { date_eq: "${today}" }
          ) {
            sum { requests pageViews }
            uniq { uniques }
          }
          last30d: httpRequests1dGroups(
            limit: 30
            filter: { date_geq: "${thirtyAgo}" }
          ) {
            sum { requests pageViews }
            uniq { uniques }
          }
          allTime: httpRequests1dGroups(
            limit: 730
            filter: { date_geq: "${LAUNCH_DATE}" }
          ) {
            sum { requests pageViews }
            uniq { uniques }
          }
        }
      }
    }`;

    const res = await fetch('https://api.cloudflare.com/client/v4/graphql', {
      method: 'POST',
      headers: {
        'Authorization': `Bearer ${env.CF_STATS_TOKEN}`,
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ query }),
    });

    if (!res.ok) throw new Error(`Cloudflare API: ${res.status}`);

    const data = await res.json();
    const zones = data?.data?.viewer?.zones;
    if (!zones?.length) throw new Error('No zone data returned');

    const zone = zones[0];

    function sumGroups(groups) {
      let requests = 0, pageViews = 0, uniques = 0;
      for (const g of groups) {
        requests += g.sum?.requests || 0;
        pageViews += g.sum?.pageViews || 0;
        uniques += g.uniq?.uniques || 0;
      }
      return { requests, pageViews, uniques };
    }

    // Daily data points for the chart
    const daily = (zone.daily || []).map(d => ({
      date: d.dimensions.date,
      pageViews: d.sum?.pageViews || 0,
      uniques: d.uniq?.uniques || 0,
    }));

    const stats = {
      daily,
      today: sumGroups(zone.today || []),
      last30d: sumGroups(zone.last30d || []),
      allTime: sumGroups(zone.allTime || []),
      updated: now.toISOString(),
    };

    const body = JSON.stringify(stats);
    const response = new Response(body, {
      headers: {
        'Content-Type': 'application/json',
        'Cache-Control': `public, max-age=${CACHE_TTL}`,
      },
    });

    // Store in CF edge cache
    const toCache = response.clone();
    context.waitUntil(cache.put(cacheKey, toCache));

    return response;
  } catch (err) {
    return Response.json({ error: err.message }, { status: 500 });
  }
}
