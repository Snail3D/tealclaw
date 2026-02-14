# TealClaw Skill

BYOK voice dashboard PWA at tealclaw.ai. Zero server, total privacy, Cloudflare Pages.

## For AI Agents: How to Configure TealClaw

The full agent skill guide is at `https://tealclaw.ai/llms.txt`. Any AI agent can
fetch that file to learn the complete config schema and how to generate configs for users.

Human-readable version with copy button: `https://tealclaw.ai/llms.html`

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
https://tealclaw.ai/#config=BASE64_ENCODED_JSON
```

TealClaw reads the hash on load, imports the config, and clears the hash from the
URL bar. The config never touches a server — the `#` fragment stays client-side.

**Example — inject a full setup via shell:**
```bash
# Build the base64 payload
CONFIG=$(echo -n '{"aiKey":"sk-or-v1-xxx","whisperKey":"gsk_xxx","ttsKey":"sk_xxx"}' | base64)

# Open in the user's default browser
open "https://tealclaw.ai/#config=$CONFIG"          # macOS
xdg-open "https://tealclaw.ai/#config=$CONFIG"      # Linux
start "https://tealclaw.ai/#config=$CONFIG"          # Windows
```

**Example — inject via JavaScript (browser automation):**
```javascript
const config = { aiKey: "sk-or-v1-xxx", whisperKey: "gsk_xxx", ttsKey: "sk_xxx" };
const url = "https://tealclaw.ai/#config=" + btoa(JSON.stringify(config));
window.open(url);
// or: page.goto(url)  // Playwright/Puppeteer
```

**Example — inject via Python:**
```python
import json, base64, webbrowser
config = {"aiKey": "sk-or-v1-xxx", "whisperKey": "gsk_xxx", "ttsKey": "sk_xxx"}
payload = base64.b64encode(json.dumps(config).encode()).decode()
webbrowser.open(f"https://tealclaw.ai/#config={payload}")
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
https://tealclaw.ai/#config=eyJhaUtleSI6InNrLW9yLXYxLXh4eCJ9
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
| ttsAutoPlay | boolean | true = auto-speak; false = only call ElevenLabs on tap (saves credits). Default: true |
| sysPrompt | string | System prompt sent to the AI |
| mode | "direct" / "agent" | Direct = call provider; Agent = route through OpenClaw gateway |
| tgToken | string | Telegram bot token from @BotFather |
| tgChatId | string | Telegram chat/group ID |
| tgEnabled | boolean | Telegram forwarding on/off |
| imageGenUrl | string | Image generation API endpoint |
| imageGenKey | string | Image gen API key (falls back to aiKey) |
| imageGenModel | string | Image gen model name |
| imageGenSize | string | e.g. "1024x1024" |
| gifEnabled | boolean | Show fullscreen GIF overlay on AI responses (default: true) |
| accentColor | string | Hex color for UI accent (default: #0d9488). Cascades everywhere |
| fontSize | string | "small" / "medium" / "large" or CSS value like "16px" |
| fontFamily | string | Custom font family |
| bgColor | string | Background color override |
| bgImage | string | URL for fullscreen background image |
| textColor | string | Main text color override |
| chatUserColor | string | User bubble background color |
| chatAiColor | string | AI bubble background color |
| themeMode | string | "dark" or "light" |
| botName | string | Custom name in header (replaces "TealClaw") |
| botIcon | string | URL to custom header icon |
| botGreeting | string | Custom welcome message (Markdown supported) |
| inputFontSize | string | Chat input font size (default: "15px") |
| buttonSize | string | Mic/send button size (default: "44px") |
| borderRadius | string | "sharp" / "round" or CSS value |
| hideTopbar | boolean | Hide the top navigation bar |
| hideAttachBtn | boolean | Hide the file attach button |
| hideCameraBtn | boolean | Hide the camera button |
| sendBtnColor | string | Send button background color |
| micBtnColor | string | Mic button background color |
| sendBtnImage | string | URL to custom send button image |
| micBtnImage | string | URL to custom mic button image |
| inputPlaceholder | string | Chat input placeholder text |
| chatMaxWidth | string | Chat container max width (default: "760px") |
| topbarBg | string | Topbar background (color or gradient) |
| inputBarBg | string | Input bar background color |
| borderColor | string | Border color throughout UI |
| customCSS | string | Inject arbitrary CSS |
| mdHeadingColor | string | Markdown heading color |
| mdBoldColor | string | Markdown bold text color |
| mdLinkColor | string | Markdown link color |
| mdCodeBg | string | Inline code background color |
| mdCodeColor | string | Inline code text color |
| mdBlockquoteBorder | string | Blockquote left border color |
| mdBlockquoteBg | string | Blockquote background color |
| chatUserTextColor | string | User bubble text color |
| chatAiTextColor | string | AI bubble text color |
| chatBubbleRadius | string | Chat bubble border radius |
| chatBubblePadding | string | Chat bubble padding |
| reduceMotion | boolean | Disable animations (accessibility) |
| highContrast | boolean | Boost contrast for visibility |
| dyslexiaFont | boolean | Use OpenDyslexic font |
| lineHeight | string | Custom line height |
| letterSpacing | string | Custom letter spacing |
| wordSpacing | string | Custom word spacing |
| focusHighlight | boolean | Show visible focus outlines |
| compactMode | boolean | Reduce spacing for more content |
| autoScroll | boolean | Auto-scroll on new messages (default: true) |
| hapticFeedback | boolean | Vibrate on send (mobile) |
| soundEnabled | boolean | Play tones on send/receive |
| maxTokens | number | Max tokens per response (default: 400) |
| temperature | number | AI creativity (0-2, default: 0.7) |
| hideBmc | boolean | Hide Buy Me a Coffee link |
| cameraEnabled | boolean | Enable/disable camera access (default: true) |
| streamEnabled | boolean | Stream AI responses token by token (default: false) |
| latexEnabled | boolean | Render LaTeX math with KaTeX (default: false) |
| contextMessages | number | Conversation context messages (default: 20, range: 2-50) |
| quickReplies | array | Quick reply chip buttons (array of strings) |
| userAvatar | string | URL to user avatar image |
| aiAvatar | string | URL to AI avatar image |
| loadingText | string | Custom loading text (default: "Thinking...") |
| loadingEmoji | string | Emoji prefix for loading text |
| fastModel | string | Model for short/simple messages (routing) |
| complexModel | string | Model for long/complex messages (routing) |
| routingThreshold | number | Char threshold for model routing (default: 80) |
| maxInputLength | number | Max characters allowed in input |
| inputPrefix | string | Auto-prepend text to every message |
| webhookUrl | string | HTTP endpoint for event webhooks |
| webhookEvents | string | Comma-separated webhook events |
| scheduledMessages | array | Timed greetings [{time, text, days}] |
| pinCode | string | 4-6 digit PIN to lock message sending |
| pinRequired | boolean | Require PIN before sending messages (default: false) |
| translateTo | string | Auto-translate AI responses to this language |
| activeProfile | string | Current config profile name |
| agents | array | OpenClaw gateway agents (replaces entire list) |

Agent objects: `{ id, name, url, token, active }` — see llms.txt for full details.

## /research Command

Deep research reports powered by Groq API (same key as Whisper voice input).

- `/research your topic` — text-based research
- `/research` + attached image — visual analysis
- Results display as segmented ORNOT-style report with verdict badge, findings, and sources
- Full context (3-4x detail) forwarded to Telegram/OpenClaw agent for follow-up questions
- Only requires a Groq key (`whisperKey`)

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

### Save ElevenLabs credits (on-demand TTS)
```json
{"ttsAutoPlay": false}
```

### Custom system prompt
```json
{"sysPrompt": "You are a pirate who speaks in nautical metaphors."}
```

### Disable GIF reactions
```json
{"gifEnabled": false}
```

### Custom color theme
```json
{"accentColor": "#8b5cf6", "bgColor": "#0a0520", "chatUserColor": "#1a1040", "chatAiColor": "#0f0a2a"}
```

### Background image
```json
{"bgImage": "https://example.com/background.jpg"}
```

### Large font
```json
{"fontSize": "large"}
```

### Kid-friendly story bot
```json
{
  "botName": "StoryBot",
  "botGreeting": "# Hey there, adventurer!\nI'm **StoryBot**! What story should we tell today?",
  "sysPrompt": "You are StoryBot, a friendly storyteller for kids ages 5-10. Tell engaging, age-appropriate stories. Ask what happens next. Never use scary content.",
  "accentColor": "#f97316",
  "fontSize": "large"
}
```

### Free chatbot (zero cost)
```json
{"whisperKey": "gsk_KEY", "aiProvider": "groq", "ttsAutoPlay": false}
```
One Groq key covers chat + vision + voice input. No cost.

### Enable streaming + quick replies
```json
{"streamEnabled": true, "quickReplies": ["Tell me more", "New topic", "Summarize"]}
```

### Multi-model routing
```json
{"fastModel": "meta-llama/llama-3.1-8b-instant", "complexModel": "google/gemini-2.5-flash-preview", "routingThreshold": 80}
```

### Power user
```json
{"streamEnabled": true, "latexEnabled": true, "contextMessages": 40, "maxTokens": 2000}
```

### PIN code lock
```json
{"pinCode": "1234", "pinRequired": true}
```

## Security

### PIN Code

Set a 4-6 digit PIN to prevent unauthorized message sending:
```json
{"pinCode": "1234", "pinRequired": true}
```

Users must enter the PIN before they can send messages. Useful for shared or unlocked devices.

### QR Code Safety

QR codes generated by `/qr` do **not** contain API keys -- only visual settings and persona configuration. Keys stay in localStorage on the device that generated the QR code.

### Security Tips

- Lock your device with a passcode or biometrics
- Do not share QR codes publicly (they contain your persona and visual config)
- API keys stay in browser localStorage and are never transmitted to tealclaw.ai
- Use `/profile` to separate personal and shared configurations

## Chat Commands

| Command | What It Does |
|---------|-------------|
| `/help` | Show all commands |
| `/research query` | Deep research report |
| `/imagine prompt` | Generate an image |
| `/export` | Download chat as Markdown |
| `/profile save/load/list/delete name` | Manage config profiles |
| `/qr` | Generate QR code to share config |
| `/keys` | Show current config status |
| `/clear` | Clear chat history |
| `/telegram` | Telegram setup and toggle |
| `/voice` | Voice settings |

## Smart Paste Shortcut

For a single key, the user can just paste it bare (no JSON needed):
- `sk-or-v1-*` → auto-detected as OpenRouter AI key
- `gsk_*` → auto-detected as Groq Whisper key
- `sk_` + hex → auto-detected as ElevenLabs TTS key

## Architecture

- Single HTML file (`index.html`) — no build step
- Service worker (`sw.js`) for offline PWA support
- All API keys in browser `localStorage` — never sent to tealclaw.ai
- Agent skill guide at `/llms.txt` (raw) and `/llms.html` (with copy button)
- Stats dashboard at `/stats.html` (Cloudflare Analytics via Pages Function)

## Development

Open `index.html` in a browser. For service worker testing: `npx serve .`

## Deployment

Cloudflare Pages auto-deploys on `git push`. The `functions/` directory is auto-detected for Pages Functions.

## Creator & Community

TealClaw is made by **Snail**.

- YouTube: https://www.youtube.com/@RealSnail3D — tutorials, demos, and TealClaw content
- MakerWorld: https://makerworld.com/en/@Snail — 3D-printable accessories (speakers, button mounts, etc.)

Stats API requires environment variables in Cloudflare Pages settings:
- `CF_STATS_TOKEN` — Cloudflare API token with Analytics:Read
- `CF_ZONE_ID` — Zone ID for tealclaw.ai
