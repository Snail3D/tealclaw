<div align="center">

# TealClaw

![TealClaw](og-image.png)

**Blow your agent's mind.**

[**Try it now**](https://tealclaw.ai) &nbsp;&middot;&nbsp; [Skill Guide](https://tealclaw.ai/llms.txt) &nbsp;&middot;&nbsp; [Security](SECURITY.md)

</div>

---

TealClaw is a **bring-your-own-key** AI chat app that runs entirely in your browser. Plug in a free [Groq API key](https://console.groq.com/keys) and get voice chat, image generation, deep research, vision, and 110+ customizable settings — with zero data collection. The entire app is a single HTML file hosted as static assets on Cloudflare Pages. There is no backend, no database, and no telemetry. Your API keys never leave `localStorage`.

## What's New in V3

### ⚡ Native Local LLM Support
Connect TealClaw directly to your local **Ollama** or **vLLM** instances. No backend or intermediary server required—run powerful models entirely locally while still using the TealClaw UI.

### 🤖 MATON API Integration
Give your agents real superpowers. TealClaw now natively hooks into the MATON API, enabling seamless external tool execution and real-time environment interaction right from the chat window.

### 🧬 Active Swarm Dashboard
Keep track of everything your agents are doing with the new **Active Swarm Dashboard** (Subagent Pills UI). Instantly view, monitor, and steer multiple background subagents working concurrently without cluttering your main chat view.

## Features

| | |
|---|---|
| **Voice Chat** | Hold to speak, get spoken replies. Six Groq TTS personas. ElevenLabs support. |
| **Multi-Provider** | Groq, OpenAI, Anthropic, Fireworks, OpenRouter, xAI, Mistral, Together, DeepSeek, Cerebras, Perplexity. Switch with `/provider`. |
| **Local Models** | Native support for Ollama and vLLM without a backend. |
| **Shareable Skills** | Package your entire app config + macros into a single JSON file and distribute it instantly. |
| **tc-flow Macros** | Build and share visual, drag-and-drop prompt chains that execute locally. |
| **tc-app Mini-Apps** | Agents can inject sandboxed HTML/React widgets directly into the chat bubble. |
| **Deep Research** | `/research` for sourced reports. `/deepresearch` for multi-tool analysis. |
| **Image Gen** | `/imagine` creates images via Google Gemini. Attach a photo for image-to-image editing. |
| **The Claw** | Hand gesture control via camera. Show The Claw to start voice capture — lower your hand to send. |
| **Vision** | Snap from camera, paste, or drag images in. Multi-image support. |
| **GIF Reactions** | AI picks contextual GIFs that match the conversation. Draggable and resizable. |
| **Guest Links** | Encrypted, scoped access with an AI security filter. No keys exposed. |
| **Obsidian** | Save conversations and AI notes directly to your vault. |
| **110+ Settings** | Colors, fonts, layout, voice, persona, scheduled messages, templates — everything. |

## Get Started

```
1. Open tealclaw.ai
2. Paste a free Groq API key
3. Start talking
```

One key gives you chat, voice, vision, image generation, and deep research.

## Custom Themes

Agents (or you) can restyle TealClaw on the fly using `tc-action` config blocks. Drop one into any chat message and it applies instantly — colors, fonts, backgrounds, bot name, everything.

### Format

````
```tc-action
{"type":"config","botName":"MyBot","accentColor":"#8b5cf6","bgColor":"#0a0a0a","themeMode":"dark"}
```
````

### Example: TikiClaw

A tropical dark theme with a beach background image:

````
```tc-action
{
  "botName": "TikiClaw",
  "accentColor": "#f59e0b",
  "bgColor": "#022c22",
  "chatUserColor": "#064e3b",
  "chatAiColor": "#065f46",
  "bgImage": "https://images.unsplash.com/photo-1519046904884-53103b34b206?auto=format&fit=crop&w=1920&q=80",
  "themeMode": "dark",
  "fontFamily": "system-ui, sans-serif"
}
```
````

### Available Theme Properties

| Property | Description |
|---|---|
| `botName` | Header name (replaces "TealClaw") |
| `accentColor` | UI accent color (hex) |
| `bgColor` | Background color |
| `bgImage` | Fullscreen background image URL |
| `chatUserColor` / `chatAiColor` | Bubble background colors |
| `chatUserTextColor` / `chatAiTextColor` | Bubble text colors |
| `themeMode` | `"dark"` or `"light"` |
| `fontFamily` | Custom font stack |
| `fontSize` | `"small"` / `"medium"` / `"large"` |
| `borderRadius` | `"sharp"` / `"round"` or CSS value |
| `botGreeting` | Welcome message (supports markdown) |
| `botIcon` | Header icon URL |

Theme properties auto-apply without confirmation. See [SKILL.md](SKILL.md) for the full list of 110+ configurable fields.

## Agent Mode

Connect to an [OpenClaw](https://github.com/openclaw/openclaw) gateway for full agent mode — shared context across Telegram, Discord, Signal, and more.

```
Browser ←→ Cloudflare Tunnel ←→ OpenClaw Gateway ←→ AI Agent
```

Paste a gateway URL + token, or let your agent generate a one-click connection link. TealClaw signs every connection with Ed25519 device authentication. Remote devices require one-time approval from the gateway operator.

Setup guide: [`docs/cloudflare-gateway.html`](docs/cloudflare-gateway.html)

## Guest Link Security

Guest links let others use your agent without exposing your keys. Every guest message passes through a Groq AI security filter before reaching the agent — screening for prompt injection, social engineering, and out-of-scope requests. Attacks trigger automatic session blocks and silent owner alerts. Full details in [SECURITY.md](SECURITY.md#guest-link-security-filter).

## Commands

| Command | Description |
|---|---|
| `/research` | Sourced research report |
| `/deepresearch` | Multi-tool deep analysis |
| `/imagine` | Generate an image |
| `/photo` | Camera capture + AI analysis |
| `/screenshot` | Screen capture + AI analysis |
| `/voice` | Voice settings and persona |
| `/provider` | Switch AI provider |
| `/save` | Save to Obsidian |
| `/template` | Browse style templates |
| `/share` | Encrypted config link |
| `/session create` | Guest access link |
| `/profile save` | Save/load config profiles |
| `/help` | All commands |

All commands also work by voice.

## For AI Agents

- **[tealclaw.ai/llms.txt](https://tealclaw.ai/llms.txt)** — Skill guide (raw text, agent-ready)
- **[SKILL.md](SKILL.md)** — `tc-action` protocol reference

Agents read the skill guide, emit `tc-action` JSON blocks, and TealClaw executes them — config changes, commands, navigation, and more.

## Development

Single HTML file. No build step. No framework. No dependencies.

```bash
npx serve .
```

---

<div align="center">

Made by **Snail** &nbsp;&middot;&nbsp; [YouTube](https://www.youtube.com/@RealSnail3D) &nbsp;&middot;&nbsp; [MakerWorld](https://makerworld.com/en/@Snail)

[![Buy Me a Coffee](https://img.shields.io/badge/Buy_Me_a_Coffee-ffdd00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://buymeacoffee.com/snail3d)

[tealclaw.ai](https://tealclaw.ai)

</div>
