# Camera QA Update (permissions-approved) — 2026-02-20

After Snail confirmed browser/device permissions were approved, I re-ran camera checks live in browser.

## Environment
- URL: `http://127.0.0.1:4173/index.html`
- Device camera seen by browser: `HD Pro Webcam C920 (046d:0892)`
- Branch: local main

## Results

### 1) Front/back capture behavior

Status: **PASS** after fix.

Validated:
- `openCamera({facing:'environment'})` opens and captures.
- `openCamera({facing:'user'})` opens and captures.
- Flip path now reliably keeps preview live and capture works after flip.

Notes:
- On this single-camera webcam, user/environment resolve to same physical device label (expected), but stream remains functional.

### 2) Camera source logging

Status: **FAIL** vs Bob QA requirement (structured audit logging missing).

Observed:
- No dedicated structured camera event logs for open/flip/capture/cancel/facing-resolution.
- Browser console only showed generic storage + manifest warnings.

### 3) Low-light exposure stabilization / warm-up

Status: **NOT PRESENT / NOT VALIDATED**.

Observed:
- No code path for low-light warm-up/exposure stabilization found in camera capture flow.
- No measurable stabilization feature available to validate yet.

## Regression/fix note discovered during rerun

I found and fixed a camera flip regression path:
- Before fix, flip could stop current track and leave an ended/dead preview on reacquire failure.
- Fix adds resilient reacquire + retry and fallback to prior facing so preview does not die.

Code fix in `index.html`:
- Added `startCameraStream()` helper with brief retry for reacquire errors.
- Updated `openCamera()` to set facing only after successful stream open.
- Updated flip handler to fallback to previous facing on failure (instead of dead stream).

## Remaining blocking items for Bob acceptance
1. Add structured camera source/facing logs.
2. Implement and validate low-light stabilization/warm-up improvement with no regressions.
