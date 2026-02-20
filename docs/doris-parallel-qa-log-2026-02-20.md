# Doris-Style Parallel QA Log — 2026-02-20

## Operational Directive
Snail directive: QA must run continuously in parallel with implementation (intertwined, not sequential).

## Parallel QA Protocol (Applied)
For each implementation slice landed:
1. Run targeted static/runtime assertions immediately.
2. Validate command surface + docs alignment for touched features.
3. Run syntax gate (`node --check` over extracted JS from `index.html`).
4. Log pass/fail and continue implementation only if green.

## QA Harness Added
- `scripts/doris-parallel-qa.mjs`
  - Verifies provider runtime additions (Cerebras/Perplexity)
  - Verifies slash command handlers (`/providers`, `/provider <name>`)
  - Verifies docs were updated and aligned
  - Verifies syntax gate for `index.html` scripts

## Validation Targeted at Landed Changes
Commits validated in this QA pass:
- `fc4424a` — provider runtime + command additions
- `fcd5dc0` — docs updates (README/config/relay architecture)
- `57fef12` — master PRD delivery

## Latest QA Run Evidence
Command:
```bash
node scripts/doris-parallel-qa.mjs
```

Result:
- **19 passed, 0 failed**
- Includes syntax pass + provider/command/doc integrity checks

## Notes
- Existing unrelated modified firmware files were intentionally excluded from this QA scope.
- This harness is designed to be rerun after each implementation micro-commit during the night shift.
