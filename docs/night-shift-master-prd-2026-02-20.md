# TealClaw Night-Shift Master Build PRD (2026-02-20)

## Document Control
- **Owner:** Night shift implementation run
- **Scope window:** 8-hour execution block
- **Primary repo:** `/Users/ericwoodard/Desktop/programs/tealclaw`
- **Version:** v1.0 (execution + delivery)

---

## 1) Executive Summary

This PRD defines a high-impact overnight plan to advance TealClaw across four tracks:

1. **Provider integrations** (especially Fireworks + major AI labs)
2. **Gateway interoperability** (Telegram relay + method/contract drift tolerance)
3. **Reliability and security hardening** (timeouts, endpoint hygiene, resilient fallbacks)
4. **Audit gap closure** (missing operator controls + documentation alignment)

### Night-Shift Outcome Target
By end-of-shift, TealClaw should:
- Support **more direct providers with lower friction**
- Handle **gateway variant contracts** more reliably in relay mode
- Reduce outage sensitivity via **fallback + timeout behaviors**
- Expose operator controls for provider routing in both **Settings and slash commands**
- Ship updated docs for field-level configuration and relay architecture
- Enforce **continuous Doris-style QA in parallel** with each landed implementation slice

---

## 2) Context & Evidence Base

### Current-state evidence reviewed
- `index.html` provider routing, relay runtime, command handling, model search
- `docs/config-reference.txt`
- `docs/telegram-relay-mode.md`
- `README.md`
- Existing settings and relay diagnostics paths (`/keys`, `/telegram status`, settings panels)

### Observed pain points before this run
- Provider coverage was broad but still missing notable labs (Cerebras, Perplexity)
- No fast command UX to inspect/switch provider (`/providers`, `/provider` missing)
- Relay interoperability benefits from continuous adapter expansion due gateway method variance
- Some docs lagged implementation details (provider keys, routing fields, stream defaults)

---

## 3) Product Goals (P0)

### G1 — Provider Surface Expansion
Add feasible major-lab direct providers that are OpenAI-compatible enough for immediate support.

### G2 — Operator Routing Control
Let operators inspect and switch providers quickly without entering settings menus.

### G3 — Gateway Relay Interop Stability
Strengthen Telegram relay behavior across gateway method/event contract variants.

### G4 — Config/Docs Integrity
Bring docs in line with real runtime behavior and new fields.

### G5 — Desktop Dashboard + AI Orchestration Shift
Move toward a powerful desktop-dashboard experience where AI handles most orchestration in the background, with fewer forward-facing controls but strong status/override visibility.

---

## 4) Non-Goals (Tonight)

- No backend service introduction (TealClaw remains browser-first)
- No vendor-specific SDK bundles (keep fetch-based compatibility strategy)
- No full one-shot removal of existing controls tonight (top bar/settings remain until phased trust gates are met)
- No major UI redesign of settings architecture in a single cutover
- No persistent queue service for relay guarantees (best-effort remains)

---

## 5) Architecture Direction

### 5.1 Provider Strategy
- Keep a **provider preset registry**
- Continue OpenAI-compatible transport where feasible
- Use provider inference by key prefix and/or base URL
- Preserve Groq as premium fallback path for voice/research and resiliency

### 5.2 Relay Strategy
- Use a **compatibility adapter layer**:
  - Expanded inbound event aliases
  - Outbound method negotiation
  - History-method negotiation for startup reconcile

### 5.3 Reliability/Security Strategy
- Normalize + validate base URLs
- Keep failover behavior explicit and operator-visible
- Favor user-visible diagnostics over silent failure

### 5.4 Desktop-Dashboard Orchestration Strategy (Snail Direction)

#### Current UI surface inventory (evidence from `index.html`)
- Sticky **top bar** currently carries conversation switch/new chat/channel/settings actions.
- **Cron/alarms/timers** are exposed in primary chrome (`cron-quick-btn`, cron pills, cron modal).
- Large **settings sheet** remains the main operator control surface for many capabilities.

#### Product direction target
- Reduce up-front UI complexity.
- Move routine automation authoring/maintenance to AI background orchestration.
- Keep trust by preserving transparent status, logs, and emergency override controls.
- Progress toward optional/conditional **no-top-bar desktop mode** when navigation friction is solved.

#### Phased transition plan

**Phase A — Soft-hide + AI-first orchestration entry (near-term)**
- Keep existing cron/timer primitives intact internally.
- Shift creation/edit flows to AI commands first (`/chron`, natural language scheduling, tc-action).
- De-emphasize cron creation UI in primary chrome; retain status indicator.
- Add "Automation Status" read panel (next run) showing active jobs, next run, last run, failures.

**Hide/automate first (Phase A):**
1. Cron job *creation* controls in primary top-level chrome.
2. Advanced recurrence editing from default path (AI handles conversion/validation).
3. Bulk schedule editing screens for non-power users.

**Phase B — Dashboard-first operations (mid-term)**
- Replace multiple small controls with one desktop dashboard area:
  - Job health summary
  - Upcoming runs
  - Recent action log
  - AI orchestration state (idle/running/degraded)
- Keep deep editing behind "Advanced" affordance rather than default UI.
- Keep slash-command parity for all automation operations.

**Phase C — Minimal chrome / optional no-top-bar mode (later)**
- Introduce desktop layout profile where top bar is collapsed/hidden by default.
- Provide keyboard command palette and persistent status rail so functionality is not lost.
- Keep fast restore switch ("Classic chrome") for fallback.

#### What must remain user-visible for trust/control (non-negotiable)
1. **System state:** connected/disconnected, active mode/provider, relay health.
2. **Automation status:** job count, next run, last run result, failure count.
3. **Recent actions/audit trail:** what AI changed, when, and why.
4. **Hard stop controls:** pause automation, disable relay, cancel in-flight orchestration.
5. **Permissions/safety state:** camera/screen consent mode, sensitive capability gates.

#### Safe transition guardrails (do not regress power-user control)
- Keep all existing command-based controls functional throughout migration.
- Add an explicit **Power User / Advanced Controls** toggle.
- Preserve JSON/tc-action compatibility and import/export behaviors.
- Roll out by feature flags with instant rollback path.
- Require Doris QA pass + regression suite per phase gate.

#### Power-user continuity plan
- Advanced users retain:
  - slash commands (`/chron`, `/alarm`, `/timer`, `/keys`, `/provider`)
  - direct config editing/import paths
  - detailed diagnostics/status views
- UI simplification applies to default surface, not capability removal.

---

## 6) Prioritized Roadmap

## P0 (Tonight)

### P0.1 Add Cerebras + Perplexity Direct Provider Support
**Why:** closes major-lab gap and improves model/provider optionality.

**Implementation checklist**
- [x] Add provider presets for `cerebras`, `perplexity`
- [x] Add detection support for `csk-` / `pplx-` prefixes
- [x] Add base-URL inference for new providers
- [x] Extend provider ordering for Settings/selection lists
- [x] Extend model fallback catalog for both providers

**Acceptance criteria**
- Provider appears in Settings direct-provider selector
- `/provider cerebras` and `/provider perplexity` set routing mode successfully
- `detectKey()` maps known prefixes to `aiKey` + provider
- No syntax/runtime regression in core JS bundle check

---

### P0.2 Add Provider Operator Commands
**Why:** reduce routing friction, faster triage during outages/rate limits.

**Implementation checklist**
- [x] Add `/providers` command (status list + key presence + quick usage hints)
- [x] Add `/provider <name>` command (switch provider quickly)
- [x] Update help command surface text to include new commands

**Acceptance criteria**
- `/providers` returns full list from provider order
- Current provider is visibly marked
- `/provider <name>` updates config + status without page reload
- Unknown providers produce explicit guidance

---

### P0.3 Gateway Relay Interop Hardeners (Doc + Runtime Alignment)
**Why:** gateway contract names drift over versions; adapter strategy must be explicit.

**Implementation checklist**
- [x] Expand relay docs to reflect outbound method negotiation
- [x] Expand relay docs to include history method negotiation
- [x] Keep adapter behavior surfaced via relay diagnostics

**Acceptance criteria**
- `docs/telegram-relay-mode.md` matches runtime negotiation strategy
- Operator can understand fallback sequence from docs alone

---

### P0.4 Config Documentation Hardening
**Why:** docs are often source-of-truth for operators and automation.

**Implementation checklist**
- [x] Update API provider fields with expanded provider matrix
- [x] Document `aiBaseUrl`, `directFallbackToGroq`, `gatewayHttpFallback`
- [x] Correct stream default mismatch in docs

**Acceptance criteria**
- `docs/config-reference.txt` reflects runtime keys and defaults
- New providers discoverable from config docs

---

## P1 (Next block)

### P1.1 Provider Capability Matrix in UI
- Show per-provider capability badges (vision/tooling/stream/search)
- Disable unsupported options per provider

### P1.2 Provider Health Probing
- Add lightweight “Test Provider” action in Settings
- Cache last known health/status by provider

### P1.3 Relay Delivery Guarantees
- Optional local outbound retry queue with idempotency replay
- Better offline queue diagnostics

### P1.4 Automation Status Panel (Trust-first)
- Add dashboard-visible status module for cron/jobs/alarms/timers
- Show next run, last run, recent failure reason, and AI-orchestrated change markers
- Keep one-click pause/resume/disable controls visible

### P1.5 AI-Managed Cron Authoring by Default
- Make AI scheduling flow the default entry path
- Keep manual cron modal behind Advanced toggle
- Add explicit confirmation summaries before applying AI-generated schedule changes

### P1.6 Chrome Simplification Track
- Merge scattered top controls into a compact desktop status strip
- De-emphasize settings entry points in favor of context-driven AI actions
- Prototype top-bar-off desktop layout behind feature flag

---

## P2 (Deferred Strategic)

### P2.1 Anthropic-first Native Path Enhancements
- Extend beyond compatibility basics (tool-use native semantics)

### P2.2 Signed Config Export/Import Profiles
- Tamper-evident profile bundles

### P2.3 Relay Contract Test Harness
- Fixture-driven simulation for gateway event/method permutations

### P2.4 Orchestration Audit Center
- Unified timeline of AI-initiated background actions with revert links
- Cross-channel correlation (UI, Telegram relay, gateway agent actions)

### P2.5 Policy-Driven UI Reduction Engine
- Role/profile-based UI simplification (default, advanced, kiosk)
- Allows no-top-bar + reduced-settings layouts without hard forking code paths

---

## 7) Parallel QA Operating Model (Doris Lane)

QA is executed **continuously in parallel** with implementation, not at the end.

Per micro-commit:
1. Run targeted assertions for changed surface area.
2. Run command/help/docs alignment checks for user-facing changes.
3. Run syntax gate (`node --check` over extracted script blocks).
4. Log results before proceeding to next implementation item.

Night-shift harness:
- `scripts/doris-parallel-qa.mjs`
- Run log: `docs/doris-parallel-qa-log-2026-02-20.md`

## 8) Acceptance Test Plan

## 8.1 Functional Checks (Tonight)

1. **Syntax validation**
   - Extract script blocks and run `node --check`
2. **Provider command checks**
   - `/providers` returns complete list
   - `/provider perplexity` switches routing
   - `/provider cerebras` switches routing
3. **Detection checks**
   - Prefix mapping for `csk-...` and `pplx-...`
4. **Docs checks**
   - Provider list + routing keys visible in config docs
   - Relay negotiation sequence documented

## 8.2 Regression checks (manual smoke)
- Existing `/keys` output still renders and includes provider/model lines
- Settings provider selector still populated and functional
- Model search fallback behavior still returns results

## 8.3 Desktop-dashboard transition checks (phase gates)
- Automation status remains visible even when creation UI is de-emphasized.
- Power-user controls remain reachable (commands + advanced toggle).
- Top-bar-off mode (when enabled) still exposes:
  - conversation switching
  - settings/advanced access
  - safety kill switch / automation pause.
- AI-orchestrated schedule changes produce auditable user-visible records.

---

## 9) Risks & Mitigations

### Risk: provider key prefix ambiguity (`sk-` family)
- **Mitigation:** explicit high-confidence prefixes first (`sk-ant-`, `sk-or-v1-`, `csk-`, `pplx-`) and conservative fallback behavior

### Risk: gateway method drift beyond known aliases
- **Mitigation:** keep adapter layer documented and diagnostics surfaced; extend alias list incrementally

### Risk: docs drifting again
- **Mitigation:** include docs update in provider/relay PR checklist

### Risk: over-hiding controls reduces trust
- **Mitigation:** preserve non-negotiable visible status + kill-switch panel in every simplified layout

### Risk: power users lose precision control
- **Mitigation:** maintain command parity, advanced toggle, and JSON import/edit paths through every phase

---

## 10) Deployment & Rollback

### Deploy
- Merge as small scoped commits (provider runtime, commands, docs)
- Smoke test direct mode + command parser on staging/local browser

### Rollback
- Revert latest provider/command commits without touching unrelated firmware work
- Keep docs sync rollback paired with runtime rollback

---

## 11) Tonight Execution Log (Delivered)

### Delivered in this run
- Added **Cerebras** and **Perplexity** providers to runtime preset registry
- Added key-prefix detection for `csk-` and `pplx-`
- Added base-url inference for new providers
- Added model fallback catalog entries for both new providers
- Added `/providers` and `/provider <name>` command UX
- Updated README to expose provider-switch commands
- Updated config reference for provider/routing fields and stream default
- Updated Telegram relay architecture doc for negotiated outbound/history methods

### Validation completed
- JS syntax check passed (`node --check` over extracted script blocks)

### Direction updates captured
- Added phased desktop-dashboard transition plan per Snail directive:
  - AI-managed background orchestration first
  - reduced forward-facing settings/cron surfaces
  - trust-critical visibility + power-user continuity guardrails

---

## 12) Deferred Items + Blockers (Explicit)

1. **Live provider integration tests against each upstream API**
   - **Blocker:** no dedicated multi-provider credential set in this run context
2. **Automated relay compatibility test harness**
   - **Blocker:** no fixture runner/CI harness currently wired for gateway protocol matrix
3. **Provider capability auto-discovery UI**
   - **Blocker:** requires stable capability metadata across providers (not standardized)
4. **End-to-end mobile UX validation pass for new commands/settings copy**
   - **Blocker:** no physical device test loop included in this night run

---

## 13) Next-Step Plan (First 2 hours of next shift)

1. Draft and ship **Automation Status panel** spec (data model + minimal UI contract).
2. Move cron creation affordance behind Advanced flag while preserving command-first flow.
3. Build relay contract fixture matrix and run adapter tests.
4. Capture one-page runbook for no-top-bar transition safety and rollback.

---

## 14) Definition of Done (This PRD)

- [x] Comprehensive roadmap with priorities and acceptance criteria
- [x] Concrete implementation completed for feasible high-impact items
- [x] Explicit deferred list with blockers
- [x] Evidence-based notes tied to actual repo files and behavior
