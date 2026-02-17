# TealClaw

BYOK voice dashboard PWA. Zero server. Total privacy.

## Architecture

```
User's Browser → AI Provider (Groq) → User's Browser
User's Browser → Groq Whisper (transcription) → User's Browser
User's Browser → Groq Orpheus (TTS) → User's Browser
User's Browser → Google Gemini (image generation) → User's Browser
User's Browser → Klipy via /api/gif (GIF reactions) → User's Browser
User's Browser → OpenClaw Gateway (via Cloudflare Tunnel) → User's Browser
```

No backend. No database. No tracking. Static files on Cloudflare Pages.

## Key Principles

- All API keys stored in browser localStorage only
- All API calls go directly from browser to provider (verify in DevTools > Network)
- Agent connection via user's own OpenClaw gateway over Cloudflare Tunnel
- PWA: installable, works offline (cached assets)
- Single HTML file + manifest + service worker

## Stack

- Static HTML/CSS/JS (no build step)
- Hosted on Cloudflare Pages (tealclaw.ai)
- PWA with service worker
- BYOK: Groq, Google AI
- Agent: OpenClaw gateway via Cloudflare Tunnel

## Gateway + Cloudflare Tunnel Configuration

Expose your OpenClaw gateway to TealClaw using a Cloudflare Tunnel:

1. Gateway binds to `loopback` (127.0.0.1)
2. `cloudflared tunnel` proxies your domain (e.g. `gw.yourdomain.com`) to `http://127.0.0.1:18789`
3. TealClaw connects to `https://gw.yourdomain.com` (HTTPS, no mixed content issues)

```bash
# Create and run a Cloudflare Tunnel
cloudflared tunnel create openclaw-gw
cloudflared tunnel route dns openclaw-gw gw.yourdomain.com
cloudflared tunnel run --url http://127.0.0.1:18789 openclaw-gw
```

**Agent URLs in TealClaw MUST be HTTPS.** Cloudflare Tunnel handles TLS automatically.

## Development

Just open index.html in a browser. No build, no server, no dependencies.

For local testing with service worker: `npx serve .`
