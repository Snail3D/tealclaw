# TealClaw

![TealClaw — Private AI Control](og-image.png)

**BYOK voice dashboard at [tealclaw.ai](https://tealclaw.ai).** Your keys never leave your browser. We see nothing.

---

## Highlights

**Zero Server, Total Privacy** — Static files on Cloudflare Pages. No backend, no database, no tracking. Your browser talks directly to AI providers.

**Voice Chat** — Hold-to-speak with Groq Whisper transcription. ElevenLabs TTS for voice responses. Full push-to-talk with live subtitles.

**The Claw** — MediaPipe hand-tracking gesture control. Pinch to talk, wave to clear, point to scroll. Hands-free everything.

**Deep Research** — `/research` generates segmented, source-cited reports powered by Groq. Entire research pipelines in one command.

**Vision** — Drag, paste, snap from camera, or attach. Multi-image support across providers.

**Single HTML File** — No build step, no framework, no dependencies. View Source and read every line. `npx serve .` and you're developing.

**95+ Config Fields** — Colors, fonts, layout, animations, persona, system prompt, accessibility, quick replies, scheduled messages, and more. Style templates let you transform the entire UI in one click.

**Agent-Configurable** — AI agents read [llms.txt](https://tealclaw.ai/llms.txt), generate a JSON config, and deliver it via URL hash, file drop, or chat paste. Partial merge — only fields present get updated. See the [tc-action protocol](SKILL.md) for full agent control.

---

## Quick Start (Free)

1. Get a free Groq key at [console.groq.com/keys](https://console.groq.com/keys)
2. Open [tealclaw.ai](https://tealclaw.ai)
3. Paste your `gsk_` key in the chat

One free key gives you chat, voice input, vision, and `/research`.

## Bring Your Own Keys

| Key | Provider | Unlocks |
|-----|----------|---------|
| AI Key | OpenRouter, Groq, or Anthropic | Chat completions |
| Whisper Key | Groq | Voice transcription + research |
| TTS Key | ElevenLabs | Voice responses |
| Gateway | OpenClaw via Cloudflare Tunnel | Agent mode |
| Telegram | Bot token + Chat ID | Message forwarding |

All keys stored in `localStorage`. Nothing sent to tealclaw.ai. Full details in **[Security Philosophy](SECURITY.md)**.

## Commands

| Command | What It Does |
|---------|-------------|
| `/research query` | Deep research report |
| `/imagine prompt` | Generate an image |
| `/save` | Save chat to Obsidian |
| `/template` | Browse and apply style templates |
| `/session create` | Time-limited encrypted share link |
| `/share` | Encrypted config link (passphrase-protected) |
| `/profile save/load` | Manage config profiles |
| `/help` | Show all commands |

## Privacy

- **No backend** — Static files only. No server, no database, no way to see your keys.
- **Direct API calls** — Browser to provider. No middleman.
- **No tracking** — No analytics, no cookies, no fingerprinting.
- **Encrypted sharing** — AES-256-GCM config links with separate passphrase.
- **Fully auditable** — One HTML file. No build step.

## For AI Agents

- **[tealclaw.ai/llms.txt](https://tealclaw.ai/llms.txt)** — Complete agent skill (raw text)
- **[tealclaw.ai/llms.html](https://tealclaw.ai/llms.html)** — Human-readable with copy button
- **[SKILL.md](SKILL.md)** — tc-action protocol docs

## Stack

Single HTML file on Cloudflare Pages. PWA with service worker. CSS custom properties for theming.

## Creator

Made by **Snail** — [YouTube](https://www.youtube.com/@RealSnail3D) / [MakerWorld](https://makerworld.com/en/@Snail)

[![Buy Me a Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-ffdd00?style=flat&logo=buy-me-a-coffee&logoColor=black)](https://buymeacoffee.com/snail3d)
