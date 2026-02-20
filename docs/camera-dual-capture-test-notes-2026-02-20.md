# Camera Dual-Capture + Low-Light Hardening (2026-02-20)

## What changed

### 1) Dual-camera capture in existing flow (no new page)
- Extended existing camera flow to accept `facing: "both"`.
- `tc-action` `camera-capture` now supports `"user" | "environment" | "both"`.
- `/photo` now supports camera hints:
  - `/photo front [question]`
  - `/photo back [question]`
  - `/photo both [question]`
- `both` capture runs in one workflow:
  - Captures back camera
  - Captures front camera
  - Builds one labeled collage image (back + front)
  - Sends/stages as a single image payload

### 2) Logging now includes camera used
- Camera captures now attach capture metadata text (`[Capture Metadata]`) into message context.
- Metadata includes:
  - source
  - camera label (`front`, `back`, `front+back`)
  - facing mode
  - timestamp
  - brightness estimate (`luma`) when available
  - dual-capture camera list when `both`
- Photo Log now stamps each frame overlay with timestamp + camera label.
- Photo Log summary/injected logs now include camera mode (`front`, `back`, `front+back`).
- Watch Mode and Surveillance alerts now include camera used.

### 3) Dark-photo mitigation (timing/exposure)
- Added camera warm-up/stabilization before capture.
- Added short pre-capture settle delay (`requestAnimationFrame` + delay).
- Added low-light camera constraints (best effort):
  - `exposureMode: continuous`
  - `whiteBalanceMode: continuous`
  - `focusMode: continuous`
- Added low-light auto-torch for still captures when supported:
  - Only for back camera
  - Only if luma is low
  - Auto-disables after capture
- Added safer camera acquisition fallbacks with progressively simpler constraints.

## Device / browser fallback behavior

- If advanced constraints are unsupported, browser silently falls back to supported settings.
- If requested facing camera is unavailable, camera open falls back through weaker constraints.
- If torch is unsupported/unavailable:
  - Torch button hides (or errors gracefully)
  - Capture still proceeds without torch
- If `both` capture cannot access one side:
  - Uses whichever camera succeeded
  - Still completes workflow with available capture
- Watch Mode does not run simultaneous dual streams; if `watchModeCamera` somehow becomes `both`, it falls back to back camera.

## Validation run notes

### Static/sanity checks completed
- Extracted inline JS and ran syntax validation:
  - `node --check /tmp/tealclaw-inline.js` ✅

### Manual browser checks recommended (not executable in CLI-only environment)
1. `/photo front test` → verify front capture + metadata includes `camera: front`.
2. `/photo back test` → verify back capture + metadata includes `camera: back`.
3. `/photo both test` → verify one dual collage image with front/back labels.
4. In low light on phone (back camera):
   - verify capture is brighter than previous behavior
   - verify no crash when torch unavailable
5. Enable Photo Log with camera `Both`:
   - verify alternating front/back frames
   - verify summary bubble mentions `camera: front+back`
6. Trigger Watch Mode alert:
   - verify alert bubble includes `camera: front|back`.
7. Trigger Surveillance alert/snap:
   - verify alert includes camera label.

## Files touched
- `index.html`
