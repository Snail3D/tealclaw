# Skill Spec: TealClaw SW Cache Bump Guard (OpenClaw + Repo Helper)

## One best idea
A deterministic **guardrail** that prevents the #1 TealClaw PWA shipping failure:

> If behavior-affecting files changed, `sw.js` must have a bumped cache version: `const CACHE='tealclaw-v###';`

This is both:
- a **repo-local helper** (`scripts/check-sw-cache-bump.mjs`), and
- a natural **OpenClaw skill wrapper** later (so agents can enforce the rule during ship flows).

## Why it’s useful
- The "always bump" rule is correct but easy to miss during rapid iteration.
- A *heuristic* detection is good enough to catch most misses.
- OpenClaw can run this as a non-negotiable step before allowing a push.

## Scope (v1)
### In scope
- Determine a base ref (default `origin/main`, else `HEAD~1`).
- Detect changed files vs base.
- Treat non-doc changes as behavior-affecting (exclude docs/*.md, scripts/, docker/).
- If behavior-affecting changes exist and `sw.js` CACHE tag didn’t change vs base: exit non-zero with a clear error.

### Out of scope (v1)
- Perfect behavior detection.
- Auto-bumping (that remains `scripts/bump-sw-cache.mjs`).
- Automatically running repo-security-sweep.

## UX
- `node scripts/check-sw-cache-bump.mjs` (default base)
- `node scripts/check-sw-cache-bump.mjs --base origin/main` (explicit)
- `node scripts/check-sw-cache-bump.mjs --verbose`

## Acceptance criteria
- If only docs changed, script exits 0.
- If index.html/functions/etc changed and CACHE didn’t: script exits non-zero and lists changed files.
- If cache bumped: script exits 0 and prints old -> new tag.

## Follow-ups
- Wire this into `scripts/ship-tealclaw.mjs` (or a pre-push hook) to reduce missed bumps.
- Add an OpenClaw skill wrapper that runs:
  1) check-sw-cache-bump
  2) repo-security-sweep
  3) refuses to `git push` unless both are clean
