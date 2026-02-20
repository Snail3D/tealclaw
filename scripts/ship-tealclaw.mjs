#!/usr/bin/env node
/**
 * TealClaw Ship Helper
 *
 * Goal: make it hard to forget the ONE rule that bites us:
 * whenever TealClaw behavior changes, bump the SW cache version.
 *
 * This script:
 *  - shows current git status
 *  - if sw.js is staged/unstaged, runs scripts/bump-sw-cache.mjs
 *  - prints the repo-security-sweep command to run BEFORE pushing
 *
 * It does NOT push.
 */

import { execFileSync } from 'node:child_process';

function sh(cmd, args, opts = {}) {
  return execFileSync(cmd, args, { encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'], ...opts });
}

function trySh(cmd, args, opts = {}) {
  try {
    return { ok: true, out: sh(cmd, args, opts) };
  } catch (e) {
    return { ok: false, out: (e?.stdout?.toString?.() ?? '') + (e?.stderr?.toString?.() ?? '') };
  }
}

const repoRoot = sh('git', ['rev-parse', '--show-toplevel']).trim();

const status = sh('git', ['status', '--porcelain=v1']);
const swTouched = status
  .split('\n')
  .filter(Boolean)
  .some(line => line.endsWith(' sw.js') || line.endsWith('\tsw.js'));

process.stdout.write(`TealClaw ship helper (repo: ${repoRoot})\n\n`);
process.stdout.write('--- git status (porcelain) ---\n');
process.stdout.write(status || '(clean)\n');
process.stdout.write('\n');

if (swTouched) {
  process.stdout.write('sw.js appears modified/staged. Bumping SW cache version...\n');
  const bump = trySh('node', ['scripts/bump-sw-cache.mjs']);
  if (!bump.ok) {
    process.stderr.write('\nFAILED to bump SW cache version. Output:\n');
    process.stderr.write(bump.out + '\n');
    process.exit(2);
  }
  process.stdout.write(bump.out + '\n');
  process.stdout.write('Re-check your diff and ensure the CACHE version bump is appropriate.\n\n');
} else {
  process.stdout.write('sw.js not detected in git status. (If behavior changed via other files, remember to bump CACHE manually.)\n\n');
}

process.stdout.write('NEXT (NON-NEGOTIABLE BEFORE ANY PUSH): run repo-security-sweep\n');
process.stdout.write('Command (TealClaw):\n');
process.stdout.write('  cd /Users/ericwoodard/clawd/skills/repo-security-sweep && \\\n');
process.stdout.write('  ./sweep.sh --repo /Users/ericwoodard/Desktop/programs/tealclaw --name tealclaw-local \\\n');
process.stdout.write('    --out /Users/ericwoodard/clawd/security-sweeps/tealclaw-local-sweeps.md \\\n');
process.stdout.write('    --audit-out /Users/ericwoodard/clawd/security-sweeps/tealclaw-local-audit.md\n');
process.stdout.write('\nThis script intentionally does not run the sweep or push for you.\n');
