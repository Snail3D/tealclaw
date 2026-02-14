# TealClaw

BYOK voice dashboard PWA. Zero server. Total privacy.

Your API keys never leave your browser. All traffic goes directly from your browser to the AI provider. We see nothing.

## How It Works

```
Your Browser → AI Provider (OpenRouter/Groq/Anthropic) → Your Browser
Your Browser → Groq Whisper (transcription) → Your Browser
Your Browser → ElevenLabs (TTS) → Your Browser
Your Browser → Your OpenClaw Gateway (via Tailscale) → Your Browser
Your Browser → Telegram Bot API → Your Telegram Chat
```

No backend. No database. No tracking. Static files on Cloudflare Pages.

## Features

- **Chat** — Multi-turn conversation with persistent history
- **Voice In** — Hold-to-speak with Groq Whisper transcription
- **Voice Out** — ElevenLabs TTS on AI responses
- **Vision** — Drag-and-drop, paste, or attach images for AI vision
- **Agent Mode** — Route through your OpenClaw gateway via Tailscale for shared agent context
- **Telegram** — Forward conversations to your Telegram bot
- **Dark/Light Theme** — Teal accent, responsive design
- **PWA** — Installable, works offline (cached assets)

## Bring Your Own Keys

| Key | Provider | What It Does |
|-----|----------|-------------|
| AI Key | OpenRouter, Groq, or Anthropic | Chat completions |
| Whisper Key | Groq | Voice transcription (auto-uses AI key if provider is Groq) |
| TTS Key | ElevenLabs | Voice responses (optional — text-only without it) |
| Gateway URL + Token | Your Tailscale network | Agent mode with shared conversation context |
| Bot Token + Chat ID | Telegram | Forward messages to your Telegram chat |

All keys are stored in your browser's `localStorage`. Open DevTools > Network to verify — zero traffic to tealclaw.com.

## Development

Just open `index.html` in a browser. No build step, no server, no dependencies.

For service worker testing:

```bash
npx serve .
```

## Stack

- Static HTML/CSS/JS (single file, no build)
- Cloudflare Pages (tealclaw.com)
- PWA with service worker

## Support

If TealClaw is useful to you:

[![Buy Me a Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-ffdd00?style=flat&logo=buy-me-a-coffee&logoColor=black)](https://buymeacoffee.com/openclaw)
