export async function onRequest(context) {
  const request = context.request;
  const accept = request.headers.get('Accept') || '';

  // If an AI agent requests the site and prefers markdown
  if (accept.includes('text/markdown')) {
    // Reroute them directly to our existing llms.txt guide
    const url = new URL(request.url);
    url.pathname = '/llms.txt'; 
    
    // Return the Markdown file with the correct headers
    const response = await context.env.ASSETS.fetch(url);
    const newResponse = new Response(response.body, response);
    newResponse.headers.set('Content-Type', 'text/markdown; charset=utf-8');
    
    // Add Cloudflare's new AI header standards
    newResponse.headers.set('x-markdown-tokens', 'estimated-count');
    newResponse.headers.set('content-signal', 'ai-train=yes, search=yes, ai-input=yes');
    
    return newResponse;
  }

  // Otherwise, serve the normal TealClaw app to human browsers
  return context.next();
}