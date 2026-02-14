# TealClaw

**BYOK voice dashboard PWA at [tealclaw.ai](https://tealclaw.ai)**. Zero server. Total privacy. 95+ configurable fields.

Your API keys never leave your browser. All traffic goes directly from your browser to the AI provider. We see nothing.

## How It Works

```
Your Browser → AI Provider (OpenRouter/Groq/Anthropic) → Your Browser
Your Browser → Groq Whisper (voice transcription) → Your Browser
Your Browser → Groq Research (deep reports) → Your Browser
Your Browser → ElevenLabs (text-to-speech) → Your Browser
Your Browser → OpenClaw Gateway (via Tailscale) → Your Browser
Your Browser → Telegram Bot API → Your Telegram Chat
Your Browser → Tenor (GIF reactions) → Your Browser
```

No backend. No database. No tracking. Static files on Cloudflare Pages.

## Features

- **Chat** — Multi-turn conversation with persistent history
- **Streaming** — Token-by-token AI response display
- **Typing Animation** — Typewriter character reveal for AI responses (slow/medium/fast)
- **Bubble Animations** — Slide, fade, scale, bounce entrance effects for chat messages
- **Voice In** — Hold-to-speak with Groq Whisper transcription
- **Voice Out** — ElevenLabs TTS with auto-play or tap-to-play
- **Vision** — Drag-and-drop, paste, camera, or attach images for AI vision
- **Research** — `/research` command for deep GROQ-powered reports with ORNOT-style segmented results
- **GIF Reactions** — Fullscreen Tenor GIF overlay on AI responses (AI can tag with `[gif:search term]`)
- **Image Generation** — `/imagine` command with configurable provider
- **LaTeX/Math** — KaTeX rendering for $inline$ and $$block$$ math
- **Quick Replies** — Configurable chip buttons for common prompts
- **Multi-Model Routing** — Auto-select fast or capable model based on query complexity
- **Style Templates** — Pre-built visual themes and bot configurations via `/template`
- **Config Profiles** — Save/load/switch between different bot configurations
- **QR Code Sharing** — Generate QR codes to configure another device instantly
- **Chat Export** — Download conversations as Markdown
- **Webhooks** — POST events to external endpoints
- **Scheduled Messages** — Timed greetings and reminders
- **PIN Code Lock** — Optional PIN to prevent unauthorized message sending
- **Widget/Embed Mode** — `?embed=true` for iframe embedding
- **Agent Mode** — Route through your OpenClaw gateway via Tailscale
- **Telegram** — Forward conversations (including full research context) to Telegram
- **95+ Config Fields** — Colors, fonts, layout, animations, markdown, buttons, accessibility, and more
- **Custom Bot Persona** — Name, icon, greeting, system prompt — transform into any chatbot
- **Accessibility** — Dyslexia font, high contrast, reduce motion, focus indicators
- **Haptic Feedback** — Vibration patterns for send/receive/error (default ON)
- **Sound Effects** — Subtle tones on interactions
- **Dark/Light Theme** — Full theme customization with CSS variable cascade
- **PWA** — Installable on iOS, Android, and desktop. Works offline.

## Quick Start (Free)

1. Get a free Groq key at [console.groq.com/keys](https://console.groq.com/keys)
2. Open [tealclaw.ai](https://tealclaw.ai)
3. Paste your `gsk_` key in the chat

That's it. One free key gives you: chat, voice input, vision, and `/research`.

## Bring Your Own Keys

| Key | Provider | What It Does |
|-----|----------|-------------|
| AI Key | OpenRouter, Groq, or Anthropic | Chat completions |
| Whisper Key | Groq | Voice transcription + /research reports |
| TTS Key | ElevenLabs | Voice responses (optional) |
| Gateway | OpenClaw via Tailscale | Agent mode with shared context |
| Telegram | Bot token + Chat ID | Message forwarding |

All keys stored in browser `localStorage`. Nothing sent to tealclaw.ai.

## For AI Agents

TealClaw is designed to be configured by AI agents. The full skill guide lives at:

- **[tealclaw.ai/llms.txt](https://tealclaw.ai/llms.txt)** — Complete agent skill (raw text)
- **[tealclaw.ai/llms.html](https://tealclaw.ai/llms.html)** — Human-readable with copy button

### Topic Guides

| Guide | URL |
|-------|-----|
| Config Reference | [/docs/config-reference.txt](https://tealclaw.ai/docs/config-reference.txt) |
| Recipes | [/docs/recipes.txt](https://tealclaw.ai/docs/recipes.txt) |
| /research Command | [/docs/research.txt](https://tealclaw.ai/docs/research.txt) |
| Visual Customization | [/docs/visual-customization.txt](https://tealclaw.ai/docs/visual-customization.txt) |
| Accessibility | [/docs/accessibility.txt](https://tealclaw.ai/docs/accessibility.txt) |
| PWA Guide | [/docs/pwa-guide.txt](https://tealclaw.ai/docs/pwa-guide.txt) |
| Chat Commands | [/docs/commands.txt](https://tealclaw.ai/docs/commands.txt) |
| Free Setup | [/docs/free-setup.txt](https://tealclaw.ai/docs/free-setup.txt) |

Agents generate a JSON config and deliver it via URL hash, file drop, or chat paste.
TealClaw does partial merge — only fields present get updated.

## Chat Commands

| Command | What It Does |
|---------|-------------|
| `/help` | Show all commands |
| `/research query` | Deep research report (GROQ-powered) |
| `/imagine prompt` | Generate an image |
| `/export` | Download chat as Markdown |
| `/profile save/load/list/delete` | Manage config profiles |
| `/qr` | Generate QR code to share config |
| `/setup` | Setup wizard |
| `/keys` | Show current config |
| `/clear` | Clear chat history |
| `/template` | Browse and apply style templates |
| `/telegram` | Telegram setup |
| `/voice` | Voice settings |

## Development

Just open `index.html` in a browser. No build step, no server, no dependencies.

```bash
# For service worker testing:
npx serve .
```

## Stack

- Single HTML file (no build step, no framework)
- Cloudflare Pages
- PWA with service worker
- CSS custom properties for dynamic theming

## Creator & Community

TealClaw is made by **Snail**.

- **YouTube**: [youtube.com/@RealSnail3D](https://www.youtube.com/@RealSnail3D) — tutorials, demos, TealClaw content
- **MakerWorld**: [makerworld.com/en/@Snail](https://makerworld.com/en/@Snail) — 3D-printable accessories

## Support

If TealClaw is useful to you:

[![Buy Me a Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-ffdd00?style=flat&logo=buy-me-a-coffee&logoColor=black)](https://buymeacoffee.com/snail3d)
