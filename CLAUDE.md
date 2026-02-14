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
- Hosted on Cloudflare Pages (tealclaw.com)
- PWA with service worker
- BYOK: OpenRouter, Groq, Anthropic, ElevenLabs
- Agent: OpenClaw gateway via Tailscale

## Development

Just open index.html in a browser. No build, no server, no dependencies.

For local testing with service worker: `npx serve .`
