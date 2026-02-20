# Skill Spec: TealClaw SW Cache Bump (Developer Utility)

## One-liner
A tiny local utility (and future OpenClaw skill wrapper) that bumps `sw.js` cache version in exactly the format TealClaw requires: `const CACHE='tealclaw-v###'`.

## Why this is the best skill idea (for TealClaw + OpenClaw)
TealClaw’s shipping rule is correct but easy to forget during rapid iteration: whenever behavior changes, the service worker cache must be bumped.

This skill makes the action:
- **Fast** (one command)
- **Consistent** (matches exact string pattern)
- **Auditable** (shows old -> new version)
- **Low-risk** (no dependencies; doesn’t touch other files)

For OpenClaw, this is also a good pattern: a “tiny deterministic repo helper” that the agent can run as a **non-negotiable step in a workflow**.

## Scope
### In scope (v1)
- Locate `const CACHE='tealclaw-v###';` in `sw.js`.
- Increment ### by 1 (preserve leading zeros).
- Support `--dry-run`.
- Support custom file via `--file`.

### Out of scope (v1)
- Auto-detecting “behavior changes”. (Humans/agents still decide *when* to bump.)
- Updating precache asset lists.
- Git commit automation.

## UX / Commands
- `node scripts/bump-sw-cache.mjs` → bumps `./sw.js`
- `node scripts/bump-sw-cache.mjs --dry-run` → prints the bump that would happen
- `node scripts/bump-sw-cache.mjs --file path/to/sw.js` → bumps specified file

## Acceptance criteria
- Running the script changes only the `CACHE` constant line.
- If the constant is missing or malformed, the script exits non-zero with a clear error.

## Future (nice-to-have)
- Wrap this into an OpenClaw “skill” entry so the agent can run:
  - `openclaw skill tealclaw:bump-sw-cache` (or equivalent)
- Add a git pre-commit check that warns if `sw.js` changed but `CACHE` didn’t.
