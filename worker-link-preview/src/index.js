export default {
  async fetch(request, env, ctx) {
    const url = new URL(request.url);
    const targetUrl = url.searchParams.get('url');

    // CORS Headers for TealClaw clients
    const corsHeaders = {
      'Access-Control-Allow-Origin': '*',
      'Access-Control-Allow-Methods': 'GET, OPTIONS',
      'Access-Control-Allow-Headers': 'Content-Type',
    };

    if (request.method === 'OPTIONS') {
      return new Response(null, { headers: corsHeaders });
    }

    if (!targetUrl) {
      return new Response(JSON.stringify({ error: 'Missing url parameter' }), {
        status: 400,
        headers: { ...corsHeaders, 'Content-Type': 'application/json' },
      });
    }

    try {
      // Basic validation
      new URL(targetUrl);
      
      const response = await fetch(targetUrl, {
        headers: {
          'User-Agent': 'TealClawLinkPreview/1.0 (https://tealclaw.ai)',
        },
      });

      if (!response.ok) throw new Error(`Fetch failed: ${response.status}`);
      
      const html = await response.text();

      // Super basic regex parsing for OpenGraph tags
      // In a real worker, we'd use HTMLRewriter, but regex is fine for OG tags
      const getMetaContent = (name) => {
        const regex = new RegExp(`<meta\\s+(?:property|name)=["'](?:og:)?${name}["']\\s+content=["'](.*?)["']`, 'i');
        const match = html.match(regex);
        return match ? match[1] : null;
      };

      const titleMatch = html.match(/<title>(.*?)<\/title>/i);
      
      const result = {
        title: getMetaContent('title') || (titleMatch ? titleMatch[1] : null),
        description: getMetaContent('description'),
        image: getMetaContent('image'),
        url: targetUrl
      };

      return new Response(JSON.stringify(result), {
        headers: { ...corsHeaders, 'Content-Type': 'application/json' },
      });

    } catch (error) {
      return new Response(JSON.stringify({ error: error.message }), {
        status: 500,
        headers: { ...corsHeaders, 'Content-Type': 'application/json' },
      });
    }
  },
};
