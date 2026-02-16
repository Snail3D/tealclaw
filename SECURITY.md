# Security Philosophy

TealClaw is built on one principle: **your data is yours**.

## Architecture

TealClaw is a static single-page application. One HTML file. No server. No database. No backend logic. The domain `tealclaw.ai` serves static files via Cloudflare Pages — it cannot execute code, store data, or intercept requests.

```
Your Browser ←→ AI Provider (OpenRouter / Groq / Anthropic)
Your Browser ←→ Groq Orpheus (text-to-speech)
Your Browser ←→ Tenor (GIF reactions)
```

Every API call goes directly from your browser to the provider. TealClaw is not in the middle.

## Key Storage

All API keys and configuration are stored in your browser's `localStorage`. This means:

- Keys exist only on your device
- Keys are never transmitted to tealclaw.ai (there is no server to transmit them to)
- Keys persist across sessions until you clear them
- Each browser/device has its own independent config
- Clearing browser data or running "Clear All Keys & Reset" in settings wipes everything

## Config Sharing

TealClaw supports sharing configurations via URL:

```
https://tealclaw.ai/#config=BASE64_JSON
```

The `#config=` portion is a **URL fragment** (hash). By design, URL fragments are never sent to the server in HTTP requests. The browser processes the fragment client-side, imports the config, and immediately clears it from the URL bar.

### Encrypted Share Links

When sharing configs that contain API keys, use the `/share` command. This creates an AES-256-GCM encrypted link:

```
https://tealclaw.ai/#config=enc:ENCRYPTED_BLOB
```

**How it works:**

1. Your config JSON is encrypted using AES-256-GCM via the Web Crypto API
2. The encryption key is derived from an 8-character alphanumeric passphrase using PBKDF2 (100,000 iterations, SHA-256)
3. A random 12-byte IV is generated for each encryption
4. The passphrase is **never included in the URL**

**To decrypt:** the recipient must enter the passphrase separately. Without it, the encrypted blob is computationally infeasible to crack.

**Passphrase strength:** 8 alphanumeric characters (a-z, A-Z, 0-9) = 62^8 = ~218 trillion combinations. Combined with PBKDF2's 100k iterations, brute-force attacks are impractical.

**Best practice:** Share the link and passphrase through different channels (e.g., link via chat, passphrase via voice or text message).

## PIN Code Lock

TealClaw supports an optional PIN code (`pinCode` + `pinRequired` config fields) that prevents unauthorized message sending. This protects against casual access on shared or unlocked devices. The PIN is stored in localStorage alongside other config.

## What We Don't Do

- **No analytics cookies.** No Google Analytics, no Facebook Pixel, no tracking scripts.
- **No fingerprinting.** We don't collect device info, IP addresses, or browser characteristics.
- **No telemetry.** No usage data, error reports, or behavioral tracking is sent anywhere.
- **No account system.** There is nothing to sign up for. No email, no password, no profile.
- **No server-side processing.** The domain serves static files. Period.

## What Third Parties See

When you use TealClaw, your AI provider (OpenRouter, Groq, Anthropic, etc.) receives your messages and API key — that's the nature of using their API. TealClaw doesn't add any additional data to these requests beyond what the provider requires.

| Service | What They See | When |
|---------|--------------|------|
| AI Provider | Your messages + API key | Every chat message |
| Groq Whisper | Your voice audio + API key | When you use voice input |
| Groq Orpheus | AI response text + API key | When TTS plays |
| Tenor | GIF search queries | When GIF reactions are enabled |
| Cloudflare Pages | Standard HTTP logs (IP, page request) | On page load only |

TealClaw adds no tracking parameters, no user IDs, and no metadata to any of these requests.

## Auditability

TealClaw is fully open source and intentionally simple to audit:

- **One HTML file.** All CSS and JavaScript are inline. No bundling, no minification, no build step.
- **No bundled dependencies.** Zero npm packages, no build step. Optional CDN libraries are loaded on demand only when their feature is used:
  - **QRCode.js** (jsdelivr) — QR code generation via `/qr`
  - **KaTeX** (jsdelivr) — LaTeX math rendering when `latexEnabled` is on
  - **Mermaid** (jsdelivr) — diagram rendering when AI responses contain mermaid blocks
  - **pdf.js** (cdnjs) — PDF file preview when attaching PDFs
  - **Mammoth** (cdnjs) — Word document preview when attaching .docx files
  - **MediaPipe Hands** (jsdelivr) — gesture control when camera gesture mode is enabled
  - **Chart.js** (jsdelivr) — usage statistics charts in `stats.html`
- **View Source works.** Right-click the page and read every line of code that runs on your device.

## Reporting Security Issues

If you find a security vulnerability, please report it responsibly:

- Open an issue at [github.com/Snail3D/tealclaw/issues](https://github.com/Snail3D/tealclaw/issues)
- Or contact Snail directly via [buymeacoffee.com/snail3d](https://buymeacoffee.com/snail3d)
