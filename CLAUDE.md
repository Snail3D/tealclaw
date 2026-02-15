# TealClaw

BYOK voice dashboard PWA. Zero server. Total privacy.

## Architecture

```
User's Browser → AI Provider (OpenRouter/Groq/Anthropic) → User's Browser
User's Browser → Groq Whisper (transcription) → User's Browser
User's Browser → ElevenLabs (TTS) → User's Browser
User's Browser → OpenClaw Gateway (via Tailscale) → User's Browser
```

No backend. No database. No tracking. Static files on Cloudflare Pages.

## Key Principles

- All API keys stored in browser localStorage only
- All API calls go directly from browser to provider (verify in DevTools > Network)
- Agent connection via user's own OpenClaw gateway over Tailscale
- PWA: installable, works offline (cached assets)
- Single HTML file + manifest + service worker

## Stack

- Static HTML/CSS/JS (no build step)
- Hosted on Cloudflare Pages (tealclaw.ai)
- PWA with service worker
- BYOK: OpenRouter, Groq, Anthropic, ElevenLabs
- Agent: OpenClaw gateway via Tailscale

## Gateway + Tailscale Configuration

When configuring the OpenClaw gateway for use with TealClaw via Tailscale:

- **tailscale serve** mode: `bind` MUST be `"loopback"`. Tailscale serve proxies external traffic to localhost, so the gateway listens on loopback and tailscale handles the external exposure. Setting `bind: "tailnet"` will crash the gateway.

```json
{
  "gateway": {
    "bind": "loopback",
    "tailscale": {
      "mode": "serve"
    }
  }
}
```

- **tailscale funnel** mode: same — `bind: "loopback"` + tailscale handles external routing.
- **Direct tailnet access** (no serve/funnel): only then use `bind: "tailnet"`.

**Never set `bind: "tailnet"` when `tailscale.mode` is `"serve"` or `"funnel"`.** This is the most common misconfiguration.

## Development

Just open index.html in a browser. No build, no server, no dependencies.

For local testing with service worker: `npx serve .`
