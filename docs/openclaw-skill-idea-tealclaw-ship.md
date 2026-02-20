# Skill Idea: `tealclaw-ship` (OpenClaw) / `scripts/ship-tealclaw.mjs` (repo helper)

## Why this skill
TealClaw is a static PWA with a service worker. The most common "it works on my machine" failure is forgetting to bump the SW cache version when behavior changes.

Also, the repo has a strict shipping policy: **run repo-security-sweep before any push**.

This skill/script is meant to enforce both, with minimal ceremony.

## One best idea
A single, repeatable **shipping assistant** that:
1) makes SW cache bumps hard to forget,
2) makes security-sweep-before-push unavoidable.

## Spec (short)

### Name
- **Repo helper (this repo):** `scripts/ship-tealclaw.mjs`
- **Future OpenClaw skill wrapper:** `tealclaw-ship`

### Inputs
- None required.
- Optional flags (future): `--dry-run`, `--no-bump`, `--sweep`.

### Behavior
1) Print `git status --porcelain`.
2) If `sw.js` is modified or staged:
   - Run `node scripts/bump-sw-cache.mjs`.
   - Remind the developer to confirm `const CACHE='tealclaw-v###'` bump is correct.
3) Print the exact **repo-security-sweep** command to run before pushing.
4) Never push automatically.

### Success criteria
- When a dev changes `sw.js`, the cache bump happens immediately.
- The security sweep command is always the last step shown before any push.

### Non-goals
- Do not attempt to auto-detect "behavior change" outside of `sw.js` (too subjective).
- Do not run the security sweep automatically inside TealClaw repo scripts (keeps coupling low; OpenClaw can orchestrate it).

## Notes / follow-ups
- If we want stronger enforcement later: add a pre-push hook that blocks when `sw.js` changed but CACHE didn’t.
- OpenClaw wrapper skill can call this script, then call repo-security-sweep, then refuse to run `git push` unless sweep is clean.
