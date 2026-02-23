# TealClaw App Marketplace & PWA Generator

## 1. The PWA Generator (`tc-app` Expansion)
When an agent generates a `tc-app`, TealClaw offers an "Install as App" button.
Clicking it creates a Blob containing a custom `manifest.json` and a standalone `index.html` wrapper, then redirects the browser to `tealclaw.ai/standalone#...` which triggers the browser's native "Add to Home Screen" prompt.

## 2. The Marketplace (`tealclaw.ai/apps`)
A static JSON-backed gallery of community-generated apps and flows.
- **Submission:** Users click "Publish to Marketplace" from their TealClaw UI.
- **Sanitization:** The `Safe Share` protocol runs automatically, stripping `aiKey`, `matonKey`, and any hardcoded PII from the `tc-app` payload.
- **Distribution:** The app is assigned a short-link (e.g., `tealclaw.ai/app/21ornot`) that instantly unpacks the `tc-app` into the visitor's local cache.

## 3. Agent Memory Sync
Whenever a user "Installs" or "Pins" an app from the marketplace, TealClaw fires a system message:
`[System] User installed app: "Asteroids"`
The agent logs this into `MEMORY.md` so it remembers what tools the user has available in their local OS drawer.
