# TealClaw App Marketplace & PWA Generator

## 1. The PWA Generator (`tc-app` Expansion)
When an agent generates a `tc-app`, TealClaw offers an "Install as App" button.
Clicking it creates a Blob containing a custom `manifest.json` and a standalone `index.html` wrapper, then redirects the browser to `tealclaw.ai/standalone#...` which triggers the browser's native "Add to Home Screen" prompt.

## 2. The Marketplace (`tealclaw.ai/apps`)
A static JSON-backed gallery of community-generated apps and flows.
- **Submission:** Users click "Publish to Marketplace" from their TealClaw UI.
- **Sanitization:** The `Safe Share` protocol runs automatically, stripping `aiKey`, `matonKey`, and any hardcoded PII from the `tc-app` payload.
- **Distribution:** The app is assigned a short-link (e.g., `tealclaw.ai/app/21ornot`) that instantly unpacks the `tc-app` into the visitor's local cache.

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

Agents can programmatically fetch `https://tealclaw.ai/api/marketplace.json` to regex-search for highly-rated or most-downloaded apps when a user asks for a tool.
