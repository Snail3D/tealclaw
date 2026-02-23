# TealClaw App Marketplace & PWA Generator

## 1. The PWA Generator (`tc-app` Expansion)
When an agent generates a `tc-app`, TealClaw offers an "Install as App" button.
Clicking it creates a Blob containing a custom `manifest.json` and a standalone `index.html` wrapper, then redirects the browser to `tealclaw.ai/standalone#...` which triggers the browser's native "Add to Home Screen" prompt.

## 2. The Marketplace (`tealclaw.ai/apps`)
A static JSON-backed gallery of community-generated apps and flows based on the "Immutable Fork & Vote" model.
- **Submission:** Users click "Publish to Marketplace" from their TealClaw UI. Submissions are immutable (cannot be edited).
- **Sanitization:** The `Safe Share` protocol runs automatically, stripping `aiKey`, `matonKey`, and any hardcoded PII from the `tc-app` payload.
- **Forking:** If a user wants to improve an app, they click "Remix/Fork," edit it with their AI, and submit it as a new immutable entry.
- **Distribution:** The app is assigned a short-link (e.g., `tealclaw.ai/app/21ornot`) that instantly unpacks the `tc-app` into the visitor's local cache. The community upvotes the best versions to the top.

## 4. Security & Permissions Scoping (Critical)
When apps are published to the Marketplace, they are grouped by their required "Scope":
- **Level 0 (Static App):** Basic HTML/JS (like a Calculator or Asteroids). Has NO access to `postMessage` bridge. Cannot talk to the AI. Cannot execute webhooks. 100% safe.
- **Level 1 (Sensor App):** Uses Camera or Microphone APIs (like 21orNot Scanner). Requires explicit browser permission prompt. No AI communication.
- **Level 2 (Agentic App):** Uses the `postMessage` bridge to trigger LLM inferences or `tc-action` payloads using the *user's* local API keys. User must explicitly grant "Agent Access" when installing.

By default, all Community Apps are sandboxed at Level 0 to preserve the absolute anonymity and safety promise of TealClaw. Level 2 apps must be heavily vetted or self-signed.

## 5. Ranking, Search & Metadata
The marketplace JSON includes `upvotes`, `downloads`, and tags.
When an app is submitted to the marketplace, the AI automatically generates:
1. **A short, punchy description** of what the app does.
2. **An auto-captured screenshot** (via `html2canvas` or a headless browser thumbnail) of the app UI.
3. **The Security Level Badge** (0, 1, or 2).

Any app that is Level 1 or Level 2 triggers a strict Confirmation Modal prior to installation, explicitly detailing what sensors or API accesses the app is requesting. Level 0 installs silently.

## 6. Themes & Layouts (Skins)
The marketplace isn't just for apps and flows. Users can publish entire **Themes/Skins** (the `tc-config` object containing `accentColor`, `bgImage`, `fontFamily`, etc., stripped of API keys). 
- When an agent searches for a theme (e.g., "Make it look like Halloween"), it can fetch `marketplace.json`, find the top-voted Halloween theme, and apply the config directly via a `tc-action`.

## 7. Creator Monetization (Stripe Premium Locks)
The marketplace supports paid artifacts (Apps, Flows, or Skins) without requiring a central TealClaw e-commerce backend.
- **The Listing:** Creators include a `stripePaymentLink` and `price` in their marketplace submission. The actual `tc-app` or `tc-flow` payload is AES-256 encrypted using a secret key held by the creator.
- **The Checkout:** When a user clicks "Install", they are redirected to the creator's Stripe Checkout page.
- **The Unlock (Cloudflare Worker):** Upon successful payment, Stripe hits a lightweight Cloudflare Worker webhook. The Worker instantly returns the decryption passphrase to the user on the Stripe "Success/Thank You" page.
- **The Install:** The user clicks a deep link on the Success page (`tealclaw.ai/#unlock=PASSPHRASE_HASH`), which routes them back to TealClaw, decrypts the payload, and installs the premium artifact.
- **The Moat:** Creators keep 100% of their revenue. TealClaw acts purely as the decentralized decryption engine.
