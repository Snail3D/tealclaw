# TealClaw Skill

BYOK voice dashboard PWA at tealclaw.com. Zero server, total privacy, Cloudflare Pages.

## For AI Agents: How to Configure TealClaw

The full agent skill guide is at `https://tealclaw.com/llms.txt`. Any AI agent can
fetch that file to learn the complete config schema and how to generate configs for users.

Human-readable version with copy button: `https://tealclaw.com/llms.html`

### Quick Version

1. Build a JSON object with only the fields you want to set/change
2. Deliver it to TealClaw using one of the methods below
3. TealClaw merges the partial config — only fields present get updated

## Delivery Methods (pick the best one for your capabilities)

### If You Have Browser Access (best — zero user effort)

If you can open a URL in the user's browser (computer use, browser tool, MCP,
`open` command, Puppeteer, Playwright, etc.), you can inject the config directly:

1. Build the config JSON
2. Base64-encode it: `btoa(JSON.stringify(config))`
3. Open this URL in the user's browser:

```
https://tealclaw.com/#config=BASE64_ENCODED_JSON
```

TealClaw reads the hash on load, imports the config, and clears the hash from the
URL bar. The config never touches a server — the `#` fragment stays client-side.

**Example — inject a full setup via shell:**
```bash
# Build the base64 payload
CONFIG=$(echo -n '{"aiKey":"sk-or-v1-xxx","whisperKey":"gsk_xxx","ttsKey":"sk_xxx"}' | base64)

# Open in the user's default browser
open "https://tealclaw.com/#config=$CONFIG"          # macOS
xdg-open "https://tealclaw.com/#config=$CONFIG"      # Linux
start "https://tealclaw.com/#config=$CONFIG"          # Windows
```

**Example — inject via JavaScript (browser automation):**
```javascript
const config = { aiKey: "sk-or-v1-xxx", whisperKey: "gsk_xxx", ttsKey: "sk_xxx" };
const url = "https://tealclaw.com/#config=" + btoa(JSON.stringify(config));
window.open(url);
// or: page.goto(url)  // Playwright/Puppeteer
```

**Example — inject via Python:**
```python
import json, base64, webbrowser
config = {"aiKey": "sk-or-v1-xxx", "whisperKey": "gsk_xxx", "ttsKey": "sk_xxx"}
payload = base64.b64encode(json.dumps(config).encode()).decode()
webbrowser.open(f"https://tealclaw.com/#config={payload}")
```

The user's browser opens, TealClaw loads, config imports, done. No copy-paste needed.

### If You Can Generate Files

Write a `.json` file and tell the user to drag it into the TealClaw chat window.
TealClaw auto-detects JSON files on drop and imports them.

```python
import json
config = {"aiKey": "sk-or-v1-xxx", "whisperKey": "gsk_xxx"}
with open("tealclaw-config.json", "w") as f:
    json.dump(config, f)
# Tell user: "Drag tealclaw-config.json into your TealClaw chat."
```

### If You Can Only Output Text (fallback)

Give the user a single JSON code block to copy-paste into TealClaw's chat input:

```
Here's your TealClaw config — paste this into TealClaw:

{"aiKey":"sk-or-v1-xxx","whisperKey":"gsk_xxx","ttsKey":"sk_xxx","ttsVoice":"ThT5KcBeYPX7keBQBPPD"}
```

Or give them the URL to open manually:

```
Open this link to apply your config:
https://tealclaw.com/#config=eyJhaUtleSI6InNrLW9yLXYxLXh4eCJ9
```

## Config Schema

Only include fields the user wants to set or change. Omit everything else.

| Field | Type | What It Does |
|-------|------|-------------|
| aiProvider | "openrouter" / "groq" / "anthropic" | Which AI provider to use |
| aiKey | string | API key for chat (sk-or-v1-*, gsk_*, sk-ant-*) |
| aiModel | string | Model ID (default: google/gemini-2.5-flash-preview) |
| whisperKey | string | Groq key for Whisper voice transcription |
| ttsKey | string | ElevenLabs key for text-to-speech |
| ttsVoice | string | ElevenLabs voice ID (default: ThT5KcBeYPX7keBQBPPD = Rachel) |
| sysPrompt | string | System prompt sent to the AI |
| mode | "direct" / "agent" | Direct = call provider; Agent = route through OpenClaw gateway |
| tgToken | string | Telegram bot token from @BotFather |
| tgChatId | string | Telegram chat/group ID |
| tgEnabled | boolean | Telegram forwarding on/off |
| imageGenUrl | string | Image generation API endpoint |
| imageGenKey | string | Image gen API key (falls back to aiKey) |
| imageGenModel | string | Image gen model name |
| imageGenSize | string | e.g. "1024x1024" |
| agents | array | OpenClaw gateway agents (replaces entire list) |

Agent objects: `{ id, name, url, token, active }` — see llms.txt for full details.

## Common Recipes

### Full setup (all keys at once)
```json
{"aiKey":"sk-or-v1-KEY","whisperKey":"gsk_KEY","ttsKey":"sk_KEY","ttsVoice":"ThT5KcBeYPX7keBQBPPD"}
```

### Change voice
```json
{"ttsVoice": "NEW_VOICE_ID"}
```

### Switch provider
```json
{"aiProvider": "anthropic", "aiKey": "sk-ant-...", "aiModel": "claude-sonnet-4-5-20250929"}
```

### Enable Telegram
```json
{"tgToken": "123456:ABC-DEF...", "tgChatId": "-100123456", "tgEnabled": true}
```

### Custom system prompt
```json
{"sysPrompt": "You are a pirate who speaks in nautical metaphors."}
```

## Smart Paste Shortcut

For a single key, the user can just paste it bare (no JSON needed):
- `sk-or-v1-*` → auto-detected as OpenRouter AI key
- `gsk_*` → auto-detected as Groq Whisper key
- `sk_` + hex → auto-detected as ElevenLabs TTS key

## Architecture

- Single HTML file (`index.html`) — no build step
- Service worker (`sw.js`) for offline PWA support
- All API keys in browser `localStorage` — never sent to tealclaw.com
- Agent skill guide at `/llms.txt` (raw) and `/llms.html` (with copy button)
- Stats dashboard at `/stats.html` (Cloudflare Analytics via Pages Function)

## Development

Open `index.html` in a browser. For service worker testing: `npx serve .`

## Deployment

Cloudflare Pages auto-deploys on `git push`. The `functions/` directory is auto-detected for Pages Functions.

Stats API requires environment variables in Cloudflare Pages settings:
- `CF_STATS_TOKEN` — Cloudflare API token with Analytics:Read
- `CF_ZONE_ID` — Zone ID for tealclaw.com
