# TealClaw App Store / Marketplace PRD

## Overview
The TealClaw App Store (Marketplace) enables users to browse, install, and share single-use standalone micro-applications directly within the chat UI. This expands the ecosystem and allows developers to distribute premium and free tools.

## Scope
- Fetching and parsing an external or locally hosted `marketplace.json` index.
- Resolving app entry points reliably across different hosting configurations (e.g. root vs `/public/` path).
- Graceful UI handling for missing apps and network errors.
- App installation into the user's flow macros.
- App sharing via base64 encoded URL fragments.

## UX States
1. **Loading State:** System bubble indicating "Fetching App Marketplace...".
2. **Success State:** Grid/list of available applications displayed as cards inside the chat.
3. **Card Enabled:** Target app file is found. Install and Share buttons are fully interactive.
4. **Card Disabled:** Target app file is missing (e.g. `lead-flow` coming soon). Card displays a "Coming soon" placeholder instead of interactive buttons to prevent a broken experience.
5. **Error State:** System bubble indicating "Failed to load marketplace" with a **Retry** button for quick recovery without needing a page refresh.
6. **Interaction Feedback:** Haptic success or error toasts during install or link sharing.

## Data Schema (`marketplace.json`)
```json
{
  "apps": [
    {
      "id": "string",          // Unique identifier for the app
      "name": "string",        // Display name
      "author": "string",      // Author or publisher name
      "description": "string", // Short description of the app's functionality
      "price": "string",       // Display string for price (e.g., "Free", "$2.00")
      "url": "string",         // Path to the HTML entry point of the app (e.g. "/apps/21ornot.html")
      "tags": ["string"],      // Categorization tags
      "buyUrl": "string"       // (Optional) URL to support or purchase the app
    }
  ]
}
```

## Rollout Checklist
- [x] Implement robust marketplace fetch with fallback (`/marketplace.json` -> `/public/marketplace.json`).
- [x] Introduce pre-check logic (using `HEAD`) to verify if the app entry URL is available.
- [x] Update UI to show "Coming soon" state if an app file is missing.
- [x] Add graceful error fallback with "Retry" button.
- [x] Ensure fallback paths correctly map to app assets (`/apps/` -> `/public/apps/`).
- [x] Bump `sw.js` cache version.
- [x] Create PRD document (this file).
- [ ] Test on iPhone PWA (see test steps below).

## Test Steps for iPhone PWA
1. Open the TealClaw PWA on iPhone.
2. Ensure you have the latest update by checking that the cache version has refreshed (wait a moment and re-open the app if necessary).
3. Type or tap the command to load the Marketplace (`/apps` or corresponding menu item).
4. Verify that the "Fetching App Marketplace..." text appears, followed by the app cards.
5. Verify that `21orNot Scanner` shows "Install" and "Share Link" buttons.
6. Verify that `ChatFormed Lead Flow` shows the "Coming soon" disabled button.
7. Disconnect from the network or intentionally change the path to simulate an error, and ensure the "Retry" button appears.
8. Tap "Retry" after reconnecting to successfully load the apps.
