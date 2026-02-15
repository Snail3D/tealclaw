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

### Encrypted Share Links (REQUIRED when keys are included)

**IMPORTANT:** If the config contains ANY API keys (`aiKey`, `whisperKey`, `ttsKey`,
`tgToken`, `imageGenKey`, or gateway tokens), you MUST use encrypted links. Never
send plain base64 links with API keys through any messaging channel.

TealClaw encrypts configs with AES-256-GCM. The encrypted link format:

```
https://tealclaw.ai/#config=enc:ENCRYPTED_BLOB
```

The recipient must enter an 8-character alphanumeric passphrase to decrypt.
The passphrase is NEVER included in the URL — it must be shared separately.

**Easiest approach — tell the user to type `/share` in TealClaw:**

After setting up a user's TealClaw, instruct them to type `/share` in the chat.
TealClaw will generate the encrypted link + passphrase. Tell them to send the link
to the recipient, then share the passphrase through a different channel (voice, text, DM).

**For programmatic encryption** (if you have code execution), see the Node.js
example in the full skill guide at `https://tealclaw.ai/llms.txt` under
"Encrypted Share Links".

**When sharing via Telegram, Discord, or any platform:**
1. ALWAYS encrypt the link when keys are present
2. Send the link and passphrase through DIFFERENT channels
3. Never put both in the same message

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
| gifTenorKey | string | Tenor/Google API key for GIF reactions (AIza* prefix) |
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
| typingAnimation | boolean | Enable typewriter text reveal for AI responses (default: true) |
| typingSpeed | "slow" / "medium" / "fast" | Typing animation speed (default: medium) |
| bubbleAnimation | "slide" / "fade" / "scale" / "bounce" / "none" | Chat bubble entrance animation (default: slide) |
| activeProfile | string | Current config profile name |
| gestureEnabled | boolean | Enable camera-based hand gesture control (MediaPipe Hands) |
| gestureBindings | string | JSON string of gesture bindings array [{gesture, action, label}]. Gestures: victory, fist, open_palm, thumbs_up, thumbs_down, pointing_up. Actions: voice_start, voice_stop, stop_agent, send_message, toggle_mute, send_thumbsup, send:custom text |
| rateLimitPerMin | number | Max messages per minute (default: not set = unlimited). Prevents runaway API costs |
| obsidianVault | string | Obsidian vault name for /save integration (default: "ObsidianVault"). Uses obsidian:// URI protocol |
| agents | array | OpenClaw gateway agents (replaces entire list) |

Agent objects: `{ id, name, url, token, active }` — see llms.txt for full details.

## Interactive Components (tc-ui)

AI responses can include ` ```tc-ui ` fenced code blocks to render interactive UI
components directly in the chat. This is the same pattern as config auto-apply
(` ```json `) but for UI elements.

### Component Types

**buttons** — Row or grid of clickable buttons:
```tc-ui
{"components":[{"type":"buttons","label":"Choose one","items":[
  {"id":"yes","text":"Approve","style":"primary"},
  {"id":"no","text":"Deny","style":"danger"}
]}]}
```
Button styles: `primary`, `secondary`, `danger`, `success`, `ghost`. Add `"layout":"grid"` for grid layout.

**card** — Rich card with title, description, optional image, and action buttons:
```tc-ui
{"components":[{"type":"card","title":"Weather Report","description":"Sunny, 72F","image":"https://example.com/sun.jpg","actions":[
  {"id":"details","text":"Details","style":"primary"},
  {"id":"dismiss","text":"Dismiss","style":"ghost"}
]}]}
```

**chips** — Selectable pill group:
```tc-ui
{"components":[{"type":"chips","label":"Topics","multi":false,"items":[
  {"id":"tech","text":"Technology"},
  {"id":"science","text":"Science"},
  {"id":"art","text":"Art"}
]}]}
```
Set `"multi":true` for multi-select.

**status** — Status indicator:
```tc-ui
{"components":[{"type":"status","state":"success","text":"Task completed"}]}
```
States: `pending`, `success`, `error`, `loading`.

**collapse** — Expandable details section:
```tc-ui
{"components":[{"type":"collapse","title":"Technical Details","body":"The implementation uses **WebSocket** connections with automatic reconnection."}]}
```

### Callback Flow

When a user clicks a button or single-select chip:
1. Visual pressed state (checkmark + dimmed siblings)
2. Button text auto-sends as the next chat message
3. Selection persists across page reloads via `chatHistory[i].tcSelections`

### Streaming Compatibility

`tc-ui` blocks only activate when the closing ` ``` ` fence is present. During
streaming, partial blocks render as plain text until complete.

### Collapsible Sections in Markdown

AI responses can also use HTML-style collapsible sections:
```
<details><summary>Click to expand</summary>
Hidden content here with **markdown** support.
</details>
```

### Font Choices

TealClaw includes 6 built-in font options (selectable in settings or via config):
- System Default (Inter/system-ui)
- Inter
- Georgia (serif)
- JetBrains Mono (monospace)
- Nunito
- Space Grotesk

Set via `fontFamily` config field using the CSS value, e.g.:
```json
{"fontFamily": "'Space Grotesk',system-ui,sans-serif"}
```

## File Handling

TealClaw processes files entirely client-side:

- **Drag & drop anywhere** — drop files on any part of the page, full-window overlay with visual feedback
- **Image compression** — auto-shrinks to max 1568px, JPEG 80% quality. Good for OCR/screenshots. Shows original vs compressed size
- **PDF to markdown** — extracts text from up to 50 pages using pdf.js (lazy-loaded). Formatted as markdown with page headers
- **DOCX to markdown** — converts Word docs to markdown preserving headings, bold, lists using mammoth.js (lazy-loaded)
- **CSV/TSV to markdown table** — auto-detects delimiter, outputs proper markdown table
- **Code files** — reads 30+ extensions (js, ts, py, go, rs, etc.) as text documents
- **Multi-file drop** — drop multiple files at once, all appear in file stack with thumbnails and progress indicators
- **Documents prepended** — document text is automatically prepended to the user's message when sent

## Gesture Control

Camera-based hand gesture recognition using MediaPipe Hands. Agent-configurable bindings.

- **Enable**: `{"gestureEnabled": true}`
- **Custom bindings**: Set `gestureBindings` to a JSON string array of `{gesture, action, label}` objects
- **Available gestures**: victory, fist, open_palm, thumbs_up, thumbs_down, pointing_up
- **Available actions**: voice_start, voice_stop, stop_agent, send_message, toggle_mute, send_thumbsup, `send:custom text`
- Floating draggable pip shows camera feed with hand wireframe overlay
- Gestures must be held 400ms to trigger (prevents accidental activation)

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

### Bouncy animated chat
```json
{"typingAnimation": true, "typingSpeed": "fast", "bubbleAnimation": "bounce", "soundEnabled": true}
```

### Performance mode (no animations)
```json
{"typingAnimation": false, "bubbleAnimation": "none", "reduceMotion": true}
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
| `/export` | Export current chat (encrypted JSON, re-importable) |
| `/export all` | Export all conversations (encrypted JSON) |
| `/export md` | Download chat as Markdown (for reading) |
| `/profile save/load/list/delete name` | Manage config profiles |
| `/qr` | Generate QR code to share config |
| `/keys` | Show current config status |
| `/clear` | Clear current conversation |
| `/clear all` | Delete all conversations |
| `/telegram` | Telegram setup and toggle |
| `/voice` | Voice settings |
| `/template` | Browse and apply style templates (12 built-in themes) |
| `/save` | Save current chat to Obsidian |
| `/save idea text` | Quick-capture idea to Obsidian |
| `/session create 7d` | Create a time-limited session link (1h, 7d, 2w, etc.) |
| `/session create 1h Label` | Session link with a custom label |
| `/session info` | Show current session status and expiry |
| `/session end` | End session early, wipe gateway credentials |

## Agent Action Protocol (tc-action)

Agents can control TealClaw in real-time by embedding `tc-action` blocks in responses. These are parsed, executed, and stripped from the displayed message.

### Format
````
```tc-action
{"type": "config", "data": {"tealColor": "#8b5cf6", "botName": "Reef"}}
```
````

### Action Types

| Type | Fields | What It Does |
|------|--------|-------------|
| `config` | `data: {...}` | Update any TealClaw config field in real-time |
| `command` | `command: "/save idea ..."` | Trigger any slash command |
| `save` | `name, folder, content, tags` | Save content directly to Obsidian vault |
| `research` | `query: "topic"` | Trigger the research pipeline |
| `toast` | `message, style` (ok/err) | Show a notification toast |
| `style` | `vars: {"--teal": "#f00"}` | Live-update CSS custom properties |
| `navigate` | `target` (settings/new-chat) | Navigate the UI |
| `bubble` | `html` or `text` | Inject a system bubble |

### Multiple Actions
Include multiple blocks in one response — they execute sequentially:
````
Here's your new theme!

```tc-action
{"type": "config", "data": {"tealColor": "#8b5cf6", "botName": "Reef", "bgColor": "#0a0520"}}
```

```tc-action
{"type": "toast", "message": "Theme applied!", "style": "ok"}
```

```tc-action
{"type": "save", "name": "Theme Change Log", "folder": "Daily", "content": "# Theme Update\nSwitched to purple theme at request.", "tags": ["tealclaw", "theme"]}
```
````

### Saving to Obsidian
```tc-action
{"type": "save", "name": "Meeting Notes - Feb 14", "folder": "Daily", "content": "# Meeting Notes\n\n- Discussed launch timeline\n- Need to fix auth flow", "tags": ["meeting", "tealclaw"]}
```

### Config Examples
```tc-action
{"type": "config", "data": {"systemPrompt": "You are a helpful coding assistant", "temperature": 0.3, "maxTokens": 2000}}
```

Actions show a subtle badge on the message: "⚡ 2 actions executed"

## Smart Paste Shortcut

For a single key, the user can just paste it bare (no JSON needed):
- `sk-or-v1-*` → auto-detected as OpenRouter AI key
- `gsk_*` → auto-detected as Groq Whisper key
- `sk_` + hex → auto-detected as ElevenLabs TTS key
- `AIza*` → auto-detected as Tenor/Google API key (GIF reactions)

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
