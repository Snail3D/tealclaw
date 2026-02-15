# TealClaw Skill

=== LEGEND (decode before reading) ===
TC=TealClaw  cfg=config  k=key  LS=localStorage  SW=service worker
OR=OpenRouter  GQ=Groq  EL=ElevenLabs  TG=Telegram  AP=Anthropic
b64=base64  enc=encrypted  md=Markdown  JS=JavaScript
bg=background  btn=button  msg=message  rsp=response  usr=user
req=required  opt=optional  dflt=default  fn=function
===

BYOK voice dashboard PWA at tealclaw.ai. Zero server, total privacy, Cloudflare Pages.

## For AI Agents: How to Configure TC

Full agent skill guide: `https://tealclaw.ai/llms.txt`
Human-readable version: `https://tealclaw.ai/llms.html`

### Quick Version

1. Build a JSON object with only the fields you want to set/change
2. Deliver it to TC using one of the methods below
3. TC merges the partial cfg -- only fields present get updated

## Delivery Methods (pick best for your capabilities)

### If You Have Browser Access (best -- zero usr effort)

Open a URL in the usr's browser (computer use, browser tool, MCP, shell, etc.):

1. Build cfg JSON
2. b64-encode: `btoa(JSON.stringify(config))`
3. Open: `https://tealclaw.ai/#config=BASE64_ENCODED_JSON`

TC reads hash on load, imports cfg, clears hash. # fragment stays client-side.

**Shell example:**
```bash
CONFIG=$(echo -n '{"aiKey":"sk-or-v1-xxx","whisperKey":"gsk_xxx","ttsKey":"sk_xxx"}' | base64)
open "https://tealclaw.ai/#config=$CONFIG"          # macOS
xdg-open "https://tealclaw.ai/#config=$CONFIG"      # Linux
start "https://tealclaw.ai/#config=$CONFIG"          # Windows
```

**JS example:**
```javascript
const config = { aiKey: "sk-or-v1-xxx", whisperKey: "gsk_xxx", ttsKey: "sk_xxx" };
const url = "https://tealclaw.ai/#config=" + btoa(JSON.stringify(config));
window.open(url);
// or: page.goto(url)  // Playwright/Puppeteer
```

**Python example:**
```python
import json, base64, webbrowser
config = {"aiKey": "sk-or-v1-xxx", "whisperKey": "gsk_xxx", "ttsKey": "sk_xxx"}
payload = base64.b64encode(json.dumps(config).encode()).decode()
webbrowser.open(f"https://tealclaw.ai/#config={payload}")
```

### Enc Share Links (REQUIRED when keys included)

If cfg contains ANY API keys, MUST use enc links. Never send plain b64 with keys.

TC encrypts with AES-256-GCM: `https://tealclaw.ai/#config=enc:ENCRYPTED_BLOB`

**Easiest:** tell usr to type `/share` in TC, copy enc link, send passphrase separately.

For programmatic encryption, see `https://tealclaw.ai/llms.txt` "Encrypted Share Links".

**Sharing rules:** 1) Always encrypt when keys present 2) Link + passphrase via different channels 3) Never both in same msg.

### If You Can Generate Files

Write `.json` file. Usr drags into TC chat -- auto-imports.

```python
import json
config = {"aiKey": "sk-or-v1-xxx", "whisperKey": "gsk_xxx"}
with open("tealclaw-config.json", "w") as f:
    json.dump(config, f)
```

### Text-Only Fallback

Give usr JSON code block to paste into TC chat input, or a tealclaw.ai/#config=b64 URL.

## Config Schema

Only include fields to set/change. TC uses partial merge.

| Field | Type | What It Does |
|-------|------|-------------|
| aiProvider | "openrouter"/"groq"/"anthropic" | AI provider |
| aiKey | string | Chat API k (sk-or-v1-*, gsk_*, sk-ant-*) |
| aiModel | string | Model ID (dflt: google/gemini-2.5-flash-preview) |
| whisperKey | string | GQ k for Whisper voice transcription |
| ttsKey | string | EL k for TTS |
| ttsVoice | string | EL voice ID (dflt: ThT5KcBeYPX7keBQBPPD = Rachel) |
| ttsAutoPlay | boolean | true=auto-speak; false=on tap only. Dflt: true |
| sysPrompt | string | System prompt |
| mode | "direct"/"agent" | Direct=provider; Agent=OpenClaw gateway |
| tgToken | string | TG bot token |
| tgChatId | string | TG chat/group ID |
| tgEnabled | boolean | TG forwarding on/off |
| imageGenUrl | string | Image gen endpoint |
| imageGenKey | string | Image gen API k (falls back to aiKey) |
| imageGenModel | string | Image gen model |
| imageGenSize | string | e.g. "1024x1024" |
| gifEnabled | boolean | GIF overlay on AI rsp (dflt: true) |
| gifTenorKey | string | Tenor/Google API k (AIza*) |
| accentColor | string | Hex UI accent (dflt: #0d9488). Cascades everywhere |
| fontSize | string | "small"/"medium"/"large" or CSS value |
| fontFamily | string | Custom font family |
| bgColor | string | bg color override |
| bgImage | string | Fullscreen bg image URL |
| textColor | string | Main text color |
| chatUserColor | string | Usr bubble bg |
| chatAiColor | string | AI bubble bg |
| themeMode | string | "dark"/"light" |
| botName | string | Header name (replaces "TealClaw") |
| botIcon | string | Header icon URL |
| botGreeting | string | Welcome msg (md support) |
| inputFontSize | string | Input font size (dflt: "15px") |
| buttonSize | string | Mic/send btn size (dflt: "44px") |
| borderRadius | string | "sharp"/"round" or CSS |
| hideTopbar | boolean | Hide top nav |
| hideAttachBtn | boolean | Hide attach btn |
| hideCameraBtn | boolean | Hide camera btn |
| sendBtnColor | string | Send btn color |
| micBtnColor | string | Mic btn color |
| sendBtnImage | string | Custom send btn image URL |
| micBtnImage | string | Custom mic btn image URL |
| inputPlaceholder | string | Input placeholder |
| chatMaxWidth | string | Chat max width (dflt: "760px") |
| topbarBg | string | Topbar bg (color/gradient) |
| inputBarBg | string | Input bar bg |
| borderColor | string | Border color |
| customCSS | string | Arbitrary CSS |
| mdHeadingColor | string | md heading color |
| mdBoldColor | string | md bold color |
| mdLinkColor | string | md link color |
| mdCodeBg | string | Code bg |
| mdCodeColor | string | Code text color |
| mdBlockquoteBorder | string | Blockquote border |
| mdBlockquoteBg | string | Blockquote bg |
| chatUserTextColor | string | Usr bubble text color |
| chatAiTextColor | string | AI bubble text color |
| chatBubbleRadius | string | Bubble border radius |
| chatBubblePadding | string | Bubble padding |
| reduceMotion | boolean | Disable animations |
| highContrast | boolean | Boost contrast |
| dyslexiaFont | boolean | OpenDyslexic font |
| lineHeight | string | e.g. "1.8" |
| letterSpacing | string | e.g. "0.05em" |
| wordSpacing | string | e.g. "0.1em" |
| focusHighlight | boolean | Focus outlines |
| compactMode | boolean | Reduce spacing |
| autoScroll | boolean | Auto-scroll (dflt: true) |
| hapticFeedback | boolean | Vibrate on send (mobile) |
| soundEnabled | boolean | Tones on send/receive |
| maxTokens | number | Max tokens (dflt: 400) |
| temperature | number | Creativity 0-2 (dflt: 0.7) |
| hideBmc | boolean | Hide BMC link |
| cameraEnabled | boolean | Camera access (dflt: true) |
| streamEnabled | boolean | Stream rsp (dflt: false) |
| latexEnabled | boolean | LaTeX via KaTeX (dflt: false) |
| contextMessages | number | Context msgs (dflt: 20, 2-50) |
| quickReplies | array | Quick reply chips (string array) |
| userAvatar | string | Usr avatar URL |
| aiAvatar | string | AI avatar URL |
| loadingText | string | Loading text (dflt: "Thinking...") |
| loadingEmoji | string | Loading emoji prefix |
| fastModel | string | Fast model for routing |
| complexModel | string | Complex model for routing |
| routingThreshold | number | Char threshold (dflt: 80) |
| maxInputLength | number | Max input chars |
| inputPrefix | string | Auto-prepend to msgs |
| webhookUrl | string | Webhook endpoint |
| webhookEvents | string | Webhook events |
| scheduledMessages | array | Timed greetings [{time, text, days}] |
| pinCode | string | 4-6 digit PIN |
| pinRequired | boolean | Require PIN (dflt: false) |
| translateTo | string | Auto-translate rsp language |
| typingAnimation | boolean | Typewriter reveal (dflt: true) |
| typingSpeed | "slow"/"medium"/"fast" | Typing speed (dflt: medium) |
| bubbleAnimation | "slide"/"fade"/"scale"/"bounce"/"none" | Bubble animation (dflt: slide) |
| activeProfile | string | Current profile name |
| gestureEnabled | boolean | Hand gesture control |
| gestureBindings | string | JSON string of [{gesture, action, label}] |
| rateLimitPerMin | number | Max msgs/min (dflt: unlimited) |
| obsidianVault | string | Obsidian vault for /save (dflt: "ObsidianVault") |
| agents | array | OpenClaw gateway agents (replaces list) |
| skillContext | boolean | Append TC capability context to system prompt (dflt: true) |
| use24HourTime | boolean | Use 24-hour time format for timestamps (dflt: false) |
| cameraEnabled | boolean | Show camera btn in input bar (dflt: true) |

Agent objects: `{ id, name, url, token, active }` -- see llms.txt for details.

## Interactive Components (tc-ui)

AI rsp can include ` ```tc-ui ` blocks to render interactive UI in chat.

### Component Types

**buttons** -- Clickable row/grid:
```tc-ui
{"components":[{"type":"buttons","label":"Choose one","items":[
  {"id":"yes","text":"Approve","style":"primary"},
  {"id":"no","text":"Deny","style":"danger"}
]}]}
```
Styles: `primary`, `secondary`, `danger`, `success`, `ghost`. Add `"layout":"grid"` for grid.

**card** -- Rich card with title, description, opt image, actions:
```tc-ui
{"components":[{"type":"card","title":"Weather Report","description":"Sunny, 72F","image":"https://example.com/sun.jpg","actions":[
  {"id":"details","text":"Details","style":"primary"},
  {"id":"dismiss","text":"Dismiss","style":"ghost"}
]}]}
```

**chips** -- Selectable pills:
```tc-ui
{"components":[{"type":"chips","label":"Topics","multi":false,"items":[
  {"id":"tech","text":"Technology"},
  {"id":"science","text":"Science"},
  {"id":"art","text":"Art"}
]}]}
```
`"multi":true` for multi-select.

**status** -- Indicator:
```tc-ui
{"components":[{"type":"status","state":"success","text":"Task completed"}]}
```
States: `pending`, `success`, `error`, `loading`.

**collapse** -- Expandable section:
```tc-ui
{"components":[{"type":"collapse","title":"Technical Details","body":"The implementation uses **WebSocket** connections with automatic reconnection."}]}
```

### Callback Flow

Btn/chip click: visual pressed state, text auto-sends as next msg, persists via `chatHistory[i].tcSelections`.

### Streaming

tc-ui blocks activate only when closing fence present. Partial blocks render as plain text.

### Collapsible Sections in md

```
<details><summary>Click to expand</summary>
Hidden content with **markdown** support.
</details>
```

### Font Choices

6 built-in: System Default (Inter/system-ui), Inter, Georgia, JetBrains Mono, Nunito, Space Grotesk.

```json
{"fontFamily": "'Space Grotesk',system-ui,sans-serif"}
```

## File Handling

Client-side processing:
- **Drag & drop** -- full-window overlay
- **Images** -- auto-compressed max 1568px, JPEG 80%. Sent as b64
- **PDFs** -- md text, up to 50 pages (pdf.js lazy-loaded)
- **DOCX** -- md conversion (mammoth.js lazy-loaded)
- **CSV/TSV** -- md tables, auto-detected delimiter
- **Code** -- 30+ extensions as text
- **Multi-file** -- drop multiple, file stack with thumbnails
- **Documents** -- text prepended to usr msg

## Gesture Control

Camera-based recognition via MediaPipe Hands. Agent-configurable.

- Enable: `{"gestureEnabled": true}`
- Custom: set `gestureBindings` to JSON string of `{gesture, action, label}`
- Gestures: victory, fist, open_palm, thumbs_up, thumbs_down, pointing_up
- Actions: voice_start, voice_stop, stop_agent, send_message, toggle_mute, send_thumbsup, `send:custom text`
- Floating draggable pip with hand wireframe. 400ms hold to trigger.

## /research Command

GQ-powered deep research reports (same k as Whisper).

- `/research topic` -- text research
- `/research` + image -- visual analysis
- Segmented report: verdict badge, findings, sources
- Full context (3-4x) forwarded to TG/OpenClaw agent
- Setup: `{"whisperKey": "gsk_KEY"}`

## Common Recipes

### Quick Recipes

| Recipe | JSON |
|--------|------|
| Full k setup | `{"aiKey":"sk-or-v1-KEY","whisperKey":"gsk_KEY","ttsKey":"sk_KEY","ttsVoice":"ThT5KcBeYPX7keBQBPPD"}` |
| Change voice | `{"ttsVoice": "NEW_VOICE_ID"}` |
| Switch provider | `{"aiProvider": "anthropic", "aiKey": "sk-ant-...", "aiModel": "claude-sonnet-4-5-20250929"}` |
| Enable TG | `{"tgToken": "123456:ABC-DEF...", "tgChatId": "-100123456", "tgEnabled": true}` |
| Save EL credits | `{"ttsAutoPlay": false}` |
| Custom prompt | `{"sysPrompt": "You are a pirate who speaks in nautical metaphors."}` |
| Disable GIFs | `{"gifEnabled": false}` |
| Color theme | `{"accentColor": "#8b5cf6", "bgColor": "#0a0520", "chatUserColor": "#1a1040", "chatAiColor": "#0f0a2a"}` |
| bg image | `{"bgImage": "https://example.com/background.jpg"}` |
| Large font | `{"fontSize": "large"}` |
| Free chatbot | `{"whisperKey": "gsk_KEY", "aiProvider": "groq", "ttsAutoPlay": false}` |
| Streaming + replies | `{"streamEnabled": true, "quickReplies": ["Tell me more", "New topic", "Summarize"]}` |
| Model routing | `{"fastModel": "meta-llama/llama-3.1-8b-instant", "complexModel": "google/gemini-2.5-flash-preview", "routingThreshold": 80}` |
| Bouncy chat | `{"typingAnimation": true, "typingSpeed": "fast", "bubbleAnimation": "bounce", "soundEnabled": true}` |
| No animations | `{"typingAnimation": false, "bubbleAnimation": "none", "reduceMotion": true}` |
| Power usr | `{"streamEnabled": true, "latexEnabled": true, "contextMessages": 40, "maxTokens": 2000}` |
| PIN lock | `{"pinCode": "1234", "pinRequired": true}` |

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
One GQ k covers chat + vision + voice. No cost.

## Security

### PIN Code
```json
{"pinCode": "1234", "pinRequired": true}
```
Usr must enter PIN before sending. For shared/unlocked devices.

### QR Code Safety
/qr codes do NOT contain API keys -- only visual + persona cfg.

### Tips
- Lock device with passcode/biometrics
- Don't share QR codes publicly
- API keys stay in browser LS, never sent to tealclaw.ai
- Use /profile to separate personal/shared cfgs

## Chat Commands

| Command | What It Does |
|---------|-------------|
| `/help` | Show all commands |
| `/research query` | Deep research report |
| `/imagine prompt` | Generate image |
| `/export` / `all` / `md` | Export convo(s) |
| `/profile save/load/list/delete name` | Manage cfg profiles |
| `/qr` | Share cfg QR code |
| `/keys` | Current cfg status |
| `/clear` / `all` | Clear convo(s) |
| `/telegram` / `on` / `off` | TG setup/toggle |
| `/voice` | Voice settings |
| `/template` | Browse 12 style templates |
| `/save` / `idea text` / `setup` | Obsidian save |
| `/session create/info/end` | Session management |

## Agent Action Protocol (tc-action)

Agents control TC via ```tc-action blocks in rsp. Parsed, executed, stripped from display. Badge shows count.

### Format
````
```tc-action
{"type": "config", "data": {"tealColor": "#8b5cf6", "botName": "Reef"}}
```
````

### Action Types

| Type | Fields | Effect |
|------|--------|--------|
| `config` | `data: {...}` | Update any cfg field live |
| `command` | `command: "/save ..."` | Trigger slash command |
| `save` | `name, folder, content, tags` | Save to Obsidian |
| `research` | `query: "topic"` | Trigger research pipeline |
| `toast` | `message, style` (ok/err) | Show notification |
| `style` | `vars: {"--teal": "#f00"}` | Update CSS vars |
| `navigate` | `target` (settings/new-chat) | Navigate UI |
| `bubble` | `html` or `text` | Inject system bubble |

Multiple blocks per rsp OK -- execute sequentially.

## Smart Paste

Bare k auto-detection:
- `sk-or-v1-*` --> OR AI k
- `gsk_*` --> GQ Whisper k
- `sk_` + hex --> EL TTS k
- `AIza*` --> Tenor/Google API k

## Architecture

- Single HTML file -- no build step
- SW for offline PWA
- All API keys in browser LS
- Skill guide: `/llms.txt` (raw), `/llms.html` (copy btn)
- Stats: `/stats.html` (Cloudflare Analytics via Pages Function)

## Development

Open `index.html` in browser. SW testing: `npx serve .`

## Deployment

Cloudflare Pages auto-deploys on `git push`. `functions/` auto-detected.

## Creator & Community

TC by **Snail**.

- YouTube: https://www.youtube.com/@RealSnail3D -- tutorials, demos, and TealClaw content
- MakerWorld: https://makerworld.com/en/@Snail -- 3D-printable accessories

Stats API requires Cloudflare Pages env vars: `CF_STATS_TOKEN`, `CF_ZONE_ID`.
