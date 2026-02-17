# TealClaw

![TealClaw](og-image.png)

**Private AI chat with voice.** One file. No server. Your keys never leave your browser.

**[tealclaw.ai](https://tealclaw.ai)**

---

## Get Started in 30 Seconds

1. Open **[tealclaw.ai](https://tealclaw.ai)**
2. Grab a free key from [console.groq.com/keys](https://console.groq.com/keys)
3. Paste it and start talking

That's it. One key gives you chat, voice, vision, image generation, and deep research.

---

## What You Get

**Voice Chat** — Hold to speak, get spoken replies. Six voice personas with distinct personalities: Trey (hype), Axel (hacker), Dean (professional), Haven (chill), Vera (nerd), Diane (executive). Powered by Groq Whisper + Orpheus TTS.

**Deep Research** — Type `/research` and get a full sourced report. `/deepresearch` goes deeper with multi-tool analysis.

**Image Generation** — `/imagine` creates images via Google Gemini. Attach a photo and say "make this a watercolor" for image-to-image editing.

**The Claw** — Hand gesture control via your camera. Show The Claw to start voice capture. Lower your hand to send. Hands-free everything.

**Vision** — Snap from camera, paste, or drag files in. Multi-image support.

**GIF Reactions** — AI picks contextual GIFs that match your conversation. Draggable, resizable, and fun.

**110+ Settings** — Colors, fonts, layout, voice, persona, quick replies, scheduled messages, templates. Everything is customizable. AI agents can configure it all via JSON.

**Guest Links** — Create encrypted, time-limited access links for friends. PIN-protected, rate-limited, with full control over what they can do.

**Obsidian Integration** — Save conversations and AI notes directly to your vault.

---

## Privacy First

TealClaw has **no backend**. The site is static files on Cloudflare Pages.

- Your browser talks directly to AI providers. No middleman. No proxy.
- All keys stored in your browser's `localStorage`. We literally cannot see them.
- No analytics, no cookies, no fingerprinting, no telemetry.
- Encrypted sharing uses AES-256-GCM with a separate passphrase.
- The entire app is one HTML file. View Source and read every line.

Full details: **[Security Philosophy](SECURITY.md)**

---

## Commands

| Command | What It Does |
|---------|-------------|
| `/research query` | Sourced research report |
| `/deepresearch query` | Multi-tool deep analysis |
| `/imagine prompt` | Generate an image |
| `/voice` | Voice settings and personas |
| `/save` | Save to Obsidian |
| `/template` | Browse style templates |
| `/share` | Encrypted config link |
| `/session create` | Time-limited guest access |
| `/profile save name` | Save/load config profiles |
| `/help` | All commands |

Works by voice too — just say "research quantum physics" or "imagine a sunset over mars".

---

## Connect Your Own Agent

TealClaw connects to **[OpenClaw](https://github.com/openclaw/openclaw)** gateways via WebSocket for full agent mode — shared context across Telegram, Discord, Signal, and more.

```
Your Browser ←→ Cloudflare Tunnel ←→ OpenClaw Gateway ←→ Your AI Agent
```

Paste a gateway URL + token, or let your agent generate a one-click connection link.

---

## For AI Agents

Build tools that configure TealClaw automatically:

- **[tealclaw.ai/llms.txt](https://tealclaw.ai/llms.txt)** — Full skill guide (raw text, agent-ready)
- **[tealclaw.ai/llms.html](https://tealclaw.ai/llms.html)** — Human-readable version
- **[SKILL.md](SKILL.md)** — tc-action protocol reference

Agents read the skill guide, emit `tc-action` JSON blocks, and TealClaw executes them — config changes, commands, navigation, file generation, and more.

---

## Stack

Single HTML file. No build step. No framework. No dependencies.

- PWA with service worker for offline support
- CSS custom properties for theming
- Hosted on Cloudflare Pages

```bash
# Run locally
npx serve .
```

---

## Support

Made by **Snail** — [YouTube](https://www.youtube.com/@RealSnail3D) / [MakerWorld](https://makerworld.com/en/@Snail)

[![Buy Me a Coffee](https://img.shields.io/badge/Buy_Me_a_Coffee-ffdd00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://buymeacoffee.com/snail3d)

---

<p align="center">
  <a href="https://tealclaw.ai">tealclaw.ai</a>
</p>
