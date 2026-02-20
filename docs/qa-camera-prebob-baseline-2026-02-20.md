# Camera QA Baseline (Pre-Bob) — 2026-02-20

Scope requested:
- Front/back capture validation
- Camera source logging validation
- Low-light exposure stabilization validation
- Regression reporting

## Build/context validated
- Local branch head: `9a62484` (plus docs)
- Upstream reference checked: `origin/main @ 3edec13`
- Note: Bob-specific camera low-light/logging changes are **not present** in either checked build at time of validation.

---

## Results

### 1) Front/back capture
Status: **PASS (baseline behavior)**

Evidence:
- Camera flow supports explicit facing via `openCamera({facing:'user'|'environment'})`.
- Flip control toggles `environment <-> user` and re-requests stream.
- Slash route and tc-action route both map correctly to camera open path.

Regression observed: **None** in baseline.

---

### 2) Camera source logging
Status: **NOT IMPLEMENTED / FAIL for requested QA criteria**

Findings:
- No structured camera event logging for open/flip/capture/cancel found in current camera path.
- Current code does not emit audit-friendly records like requested-facing vs resolved-facing.

Regression observed: N/A (feature not landed yet).

---

### 3) Low-light exposure stabilization / warm-up
Status: **NOT IMPLEMENTED / CANNOT VALIDATE improvement yet**

Findings:
- No warm-up/stabilization sequence detected in camera capture path.
- No explicit exposure stabilization logic found in current capture implementation.

Regression observed: N/A (feature not landed yet).

---

## Blocking/Go gate for Bob patch
When Bob’s changes land, block release if either is true:
1. Camera logging still lacks structured source/facing audit events.
2. Low-light path does not show measurable improvement (or introduces quality/perf regressions).

Use checklist: `docs/qa-camera-bob-checklist.txt`.
