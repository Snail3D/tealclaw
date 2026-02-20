# Nightly QA / Release Checklist — 2026-02-20

## Operating mode (Snail preference, 02:07 MST)
- Doris-style QA runs continuously in parallel with Bob implementation.
- Finisher/trailing QA keeps commit-by-commit validation active and maintains this file as rolling go/no-go source of truth.
- Coordination note sent to active `doris-night-continuous-qa` runner to keep output format aligned.

## Rolling go/no-go log

### 02:20 MST — Firmware merge-trail validation (main ahead of origin by 4 commits)

#### Merged commits validated
- `400fc6d` — `fix(firmware): secure setup flow and harden AP/config endpoints`
- `429956f` — `fix(proxy): keep upstream streams alive and bound request body size`
- `05b1086` — `build(firmware): pin compatible async/ble libs for stable esp32s3 build`
- `449b8ed` — `fix(proxy): isolate request body buffers per connection`

#### Smoke/regression checks run
- `platformio run` (clean build) ✅
- `platformio run -t buildfs` ✅
- `platformio check --skip-packages` ✅ *(known upstream lib noise; no new project-blocking errors introduced)*

#### Browser visibility/tab-hygiene/accessibility sanity (local web UI)
- Opened app in isolated browser profile and forced fresh load (cleared stale SW/cache before checks) ✅
- Settings sheet open/close behavior verified ✅
- Background inert + aria-hidden while sheet open verified ✅
- Focus trap verified (Tab cycles within sheet; Shift+Tab wraps; Escape closes + restores focus) ✅

#### Low-risk polish fixes applied during QA
- `firmware/src/api_proxy.cpp`
  - Added `request->onDisconnect(...)` cleanup for per-request body map entries to prevent abandoned-request memory accumulation.
- `firmware/src/web_server.cpp`
  - Added `request->onDisconnect(...)` cleanup for `/api/config` temporary body buffer to prevent disconnect-time leak.

#### Current go/no-go
- **Firmware merged set (4 commits): GO** ✅
- **Whole-repo release snapshot right now: CONDITIONAL** ⚠️
  - Working tree is intentionally dirty from other night work (`index.html` large local diff + untracked docs). Those changes are outside the merged-firmware set above and need their own final sign-off path.

### 02:15 MST — Bob-touch UI regression pass (working tree `index.html`, uncommitted)

#### Commit / patch under QA
- `working tree` (not yet committed), Bob-touched areas:
  - Moved `#cronAddBtn` into top toolbar near Help
  - Removed old in-row `#cronAddBtn` from `#timerAlarmRow`
  - Updated `renderCronBar()` visibility to be pill-driven (no gateway-only keepalive)

#### Smoke/regression checks run
- Local browser flow (desktop + mobile viewport) against `http://127.0.0.1:4173` ✅
- Topbar cron button renders and opens modal (mouse + keyboard Enter) ✅
- Validation path: save with no time shows `Add at least one time` ✅
- Create path: add time + save shows `Cron job created` and pill appears ✅
- Delete path: deleting pill hides row ✅
- DOM/state audit after delete:
  - `getCronJobs().length === 0`
  - row hidden
  - hidden stale cron pill node remains in `#cronPills` (cleanup mismatch) ⚠️

#### Blockers / severity
- **SEV-1 BLOCKER (A11y)** — Cron modal is not keyboard-contained.
  - Repro: open Cron modal, press `Tab` → focus moves to background toolbar (`#helpBtn`) instead of staying in modal.
  - Also observed: focus remains on trigger (`#cronAddBtn`) after modal open; not moved into modal fields.
  - Impact: keyboard/screen-reader users can interact with background UI while modal is open.
- **SEV-3 MINOR** — stale hidden cron pill DOM after deleting last cron item.
  - Not visible while row is hidden, but state cleanup is inconsistent.

#### Go / No-Go
- **Cron functional smoke (create/validate/delete): GO** ✅
- **UI accessibility gate for this patch: NO-GO** ⛔ until SEV-1 modal focus containment is fixed
- **Firmware merge sign-off status:** unaffected (still GO from earlier section)

### 02:18 MST — UI declutter directive evaluation (design QA gate)

#### Commit / scope under QA
- `working tree` design direction check (no new commit yet):
  - Fewer persistent top-chrome controls
  - More background automation
  - Stronger state visibility + safe manual override expectations

#### Smoke/regression checks run
- Baseline top chrome on current build (`:4174`) shows 6 icon controls in toolbar (`cron/help/search/settings/about/theme`) plus status pill and new-chat. ✅
- Simulated declutter by hiding low-frequency icons (`help/search/about/theme`) via runtime CSS:
  - `/help` command remains usable ✅
  - `Ctrl+F` still opens in-chat search on desktop ✅
- Simulated further declutter by hiding `settings` icon too:
  - `/setup` only opens Setup Wizard content in chat, **not** Settings sheet ❌
  - No equivalent obvious manual route to full settings/config controls found ❌
- Touch/mobile discoverability check:
  - in-chat search relies on toolbar button or desktop `Ctrl+F`; no clear touch-first command equivalent for local message search. ⚠️
- Automation visibility check:
  - cron quick button count badge updates and announces active count in aria label (`Schedules & alarms (1 active)`) ✅
  - but no global “pause all automations” / “stop all scheduled actions” control surfaced. ⚠️

#### Blockers / severity
- **SEV-1 BLOCKER (Discoverability + Override):** Removing/hiding Settings without a clear replacement path breaks manual override/config reachability for non-command users.
- **SEV-2 MAJOR (Mobile discoverability):** Removing Search icon without touch-visible replacement strands local message-search on mobile.
- **SEV-2 MAJOR (Trust/Safety UX):** Background automation lacks a single explicit global manual override (pause/stop-all) and richer always-visible run-state summary.

#### Go / No-Go
- **Declutter direction: CONDITIONAL GO** ✅ if guardrails are added first.
- **Hard top-chrome removal right now: NO-GO** ⛔ until replacement affordances ship.

#### Required guardrails before sign-off
- Keep a persistent or always-obvious entry for **Settings** (icon or labeled overflow menu item).
- Keep a touch-accessible entry for **Search messages** (icon or overflow item), not desktop-hotkey-only.
- Add a clear **global manual override** for automations (Pause all / Stop all) with reversible state.
- Add stronger automation state visibility (what is running, next fire time, last action result) in a compact status strip/panel.

### 02:24 MST — Ultra-minimal command-center pivot QA (keys + core input)

#### Commit / scope under QA
- `working tree` design pivot check (no new commit yet):
  - Reduce UI to ultra-minimal command center
  - Keep complexity in background
  - Preserve accessibility, trust cues, state observability, and manual override

#### Smoke/regression checks run
- Simulated ultra-minimal top chrome in-browser:
  - pass A: only `cron` kept visible (help/search/settings/about/theme hidden)
  - pass B: all topbar action icons hidden (command-only interaction)
- Command-first fallback behavior:
  - `/help`, `/keys`, `/setup` still work via core input ✅
  - power-user pathways remain functional without buttons ✅
- Observability checks:
  - status pill remains visible (`Setup`) ✅
  - cron active count/aria label works **only if cron button stays visible** ✅
  - when cron button is hidden, active schedule state becomes non-obvious ❌
- Manual override checks:
  - `/alarm` and `/timer` support list/inspect flows ✅
  - `cron` has no equivalent command-level list/override path (`/chron` is setup wizard only) ❌
  - no global `/pause-all` / `/stop-all automations` control found ❌
- Accessibility regression carry-over:
  - Cron modal focus still escapes to background (Tab lands on `#scrollAnchor`) ❌

#### Blockers / severity
- **SEV-1 BLOCKER (A11y):** modal focus containment still broken (existing blocker remains).
- **SEV-1 BLOCKER (Trust + Override):** in ultra-minimal mode, hiding cron/settings removes obvious human override for scheduled automation while command coverage is incomplete (no cron list/pause-all path).
- **SEV-2 MAJOR (Observability):** state visibility drops below trust threshold when cron surface is hidden (active automation not readily inspectable).
- **SEV-2 MAJOR (Discoverability):** command-only fallback is viable for experts but weak for first-time/non-technical users.

#### Go / No-Go
- **Ultra-minimal direction: CONDITIONAL GO** ✅
- **Ultra-minimal shipping state right now: NO-GO** ⛔ until override + observability + a11y gaps are closed.

#### Required guardrails before sign-off
- Add command-complete automation controls:
  - `/cron list`
  - `/cron pause-all` and `/cron resume-all`
  - `/cron disable <id>` / `/cron delete <id>`
  - `/status automations` (next run + enabled count + recent result)
- Keep one always-visible safety/control affordance in minimal UI (single “Control Center”/status chip opens overrides).
- Preserve explicit trust cues in compact form (automation count, next trigger, session/webhook risk badges).
- Fix modal keyboard containment before any aggressive chrome removal.

### 02:32 MST — Agent-first settings + skill/context loading QA (safe observability gate)

#### Commit / scope under QA
- `working tree` design/UX validation (no new commit yet):
  - Agent-first settings updates via `tc-action`
  - Skill/context loading for settings workflows (help-mode + skill context)
  - Compressed config/state observability + fallback visibility

#### Smoke/regression checks run
- Agent action execution behavior (runtime):
  - `executeTcActions([{type:'config', ...}])` applies config immediately (botName changed) with **no confirmation modal** ✅/⚠️
  - `executeTcActions([{type:'command', command:'/help'}])` runs command immediately with **no confirmation modal** ✅/⚠️
- Transcript observability:
  - `tc-action` blocks are stripped from visible chat by default; user sees only `⚡ 1 action executed` badge, not action details ⚠️
- Config readback path (`request-config`):
  - Action returns sanitized config object locally (sensitive fields as `...Set` booleans) ✅
  - No visible UI audit of returned payload, and no follow-up roundtrip path from `actionResults` back to model in current call flow (results are only counted for badge) ❌
- Skill/context loading:
  - `enterHelpMode()` shows banner and injects `HELP_MODE_CTX` into prompt/session context ✅
  - `skillContext=false` removes TealClaw env/action schema from system prompt (length ~12.5k → ~2.7k; no help-mode rule present) ⚠️
- Compressed state visibility (conversation compaction):
  - Compaction divider + “Conversation Memory” overlay (sticky facts + summary) remains accessible for human inspection ✅

#### Blockers / severity
- **SEV-1 BLOCKER (Trust/Safety):** config/command auto-apply without approval despite in-product guidance saying these actions require approval.
- **SEV-1 BLOCKER (Agent-first functional gap):** `request-config` result is not wired into a usable model feedback loop in current response cycle, so “read config then adjust” is incomplete.
- **SEV-2 MAJOR (Observability):** action details are hidden from transcript; users cannot audit exactly what changed from agent actions.
- **SEV-2 MAJOR (Fallback robustness):** disabling skill context silently removes critical action schema/help-mode guidance, weakening settings-update reliability in minimal UI modes.

#### Go / No-Go
- **Agent-first direction: CONDITIONAL GO** ✅
- **Ship gate for this pivot right now: NO-GO** ⛔ until trust/observability gaps are closed.

#### Required guardrails before sign-off
- Add explicit approval or explicit “auto-apply mode” safety UX for mutating actions (with undo).
- Surface a human-readable action audit trail (what changed, previous→new, timestamp/source).
- Plumb `request-config` into a deterministic follow-up channel the model can consume (or remove/replace action claim until implemented).
- Add visible warning/fallback when `skillContext` is off (reduced automation/help capabilities).

## Final pre-release checklist (this shift)
- [x] Validate all merged firmware commits on `main`
- [x] Run firmware compile smoke
- [x] Run filesystem image build
- [x] Run static analysis pass (non-blocking upstream warnings acknowledged)
- [x] Verify browser settings focus/visibility/tab hygiene behavior
- [x] Apply low-risk stability polish where found
- [ ] Run `node scripts/enterprise-check.mjs` (secrets scan + Doris QA + optional firmware build)
- [ ] Final sign-off on pending non-merged UI diffs (`index.html`) by UI owner
- [ ] Commit/push QA polish changes in firmware (`api_proxy.cpp`, `web_server.cpp`)
