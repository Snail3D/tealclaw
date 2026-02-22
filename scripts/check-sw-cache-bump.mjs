#!/usr/bin/env node
/**
 * TealClaw SW cache bump verifier.
 *
 * Why:
 * - TealClaw requires bumping the service worker cache version when behavior changes.
 * - This script provides a deterministic, repo-local check that can be run by humans
 *   and orchestrated by OpenClaw.
 *
 * What it checks:
 * - If any "behavior-affecting" files changed vs a base ref, then `sw.js` CACHE
 *   version must also change vs that base ref.
 *
 * Behavior-affecting changes (heuristic):
 * - Any changes outside docs/ and other obvious non-runtime files.
 *
 * Usage:
 *   node scripts/check-sw-cache-bump.mjs
 *   node scripts/check-sw-cache-bump.mjs --base origin/main
 *   node scripts/check-sw-cache-bump.mjs --base HEAD~1
 *   node scripts/check-sw-cache-bump.mjs --verbose
 */

import { execFileSync } from 'node:child_process';
import fs from 'node:fs';

function sh(cmd, args, opts = {}) {
  return execFileSync(cmd, args, { encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'], ...opts });
}

function trySh(cmd, args, opts = {}) {
  try {
    return { ok: true, out: sh(cmd, args, opts) };
  } catch (e) {
    const out = (e?.stdout?.toString?.() ?? '') + (e?.stderr?.toString?.() ?? '');
    return { ok: false, out };
  }
}

function parseArgs(argv) {
  const out = { base: null, verbose: false, help: false };
  for (let i = 2; i < argv.length; i++) {
    const a = argv[i];
    if (a === '--base') out.base = argv[++i];
    else if (a === '--verbose' || a === '-v') out.verbose = true;
    else if (a === '--help' || a === '-h') out.help = true;
    else throw new Error(`Unknown arg: ${a}`);
  }
  return out;
}

function readCacheTagFromSw(swSrc) {
  const re = /const\s+CACHE\s*=\s*(['"])tealclaw-v(\d+)\1\s*;/;
  const m = swSrc.match(re);
  if (!m) return null;
  return { tag: `tealclaw-v${m[2]}`, digits: m[2] };
}

function isBehaviorAffecting(file) {
  // Non-runtime / housekeeping
  if (file.startsWith('docs/')) return false;
  if (file === 'README.md') return false;
  if (file === 'SECURITY.md') return false;
  if (file.endsWith('.md')) return false;

  // Repo/dev tooling
  if (file.startsWith('scripts/')) return false;
  if (file.startsWith('docker/')) return false;
  if (file === 'Dockerfile') return false;
  if (file === 'docker-compose.yml') return false;

  // Everything else: assume it can affect shipped behavior.
  return true;
}

function resolveBaseRef(userBase) {
  if (userBase) return userBase;

  // Prefer origin/main if it exists.
  const hasOriginMain = trySh('git', ['rev-parse', '--verify', 'origin/main']).ok;
  if (hasOriginMain) return 'origin/main';

  // Fallback to previous commit.
  return 'HEAD~1';
}

function mergeBase(baseRef) {
  // If baseRef is a branch/ref, find merge-base with HEAD.
  const mb = trySh('git', ['merge-base', 'HEAD', baseRef]);
  if (mb.ok) return mb.out.trim();

  // If merge-base fails (e.g., baseRef is a commit-ish like HEAD~1), just use it.
  return baseRef;
}

function main() {
  const args = parseArgs(process.argv);
  if (args.help) {
    console.log('Usage: node scripts/check-sw-cache-bump.mjs [--base <ref>] [--verbose]');
    process.exit(0);
  }

  const baseRef = resolveBaseRef(args.base);
  const base = mergeBase(baseRef);

  // Compare base -> working tree (includes staged + unstaged for tracked files).
  const nameOnly = sh('git', ['diff', '--name-only', base]).trim();
  const diffFiles = nameOnly ? nameOnly.split('\n').filter(Boolean) : [];

  // Also include untracked files (they can affect behavior once committed).
  const untrackedRaw = sh('git', ['ls-files', '--others', '--exclude-standard']).trim();
  const untrackedFiles = untrackedRaw ? untrackedRaw.split('\n').filter(Boolean) : [];

  const files = Array.from(new Set([...diffFiles, ...untrackedFiles]));

  const behaviorFiles = files.filter(isBehaviorAffecting).filter(f => f !== 'sw.js');
  if (args.verbose) {
    console.log(`Base ref: ${baseRef}`);
    console.log(`Merge base: ${base}`);
    console.log(`Changed files (${files.length}):`);
    for (const f of files) console.log(` - ${f}`);
    console.log('');
  }

  if (behaviorFiles.length === 0) {
    console.log('OK: No behavior-affecting changes detected (vs base).');
    process.exit(0);
  }

  const baseSw = trySh('git', ['show', `${base}:sw.js`]);
  if (!baseSw.ok) {
    console.error('ERROR: Could not read sw.js from base ref.');
    console.error(baseSw.out);
    process.exit(2);
  }
  const headSwSrc = fs.readFileSync('sw.js', 'utf8');

  const baseTag = readCacheTagFromSw(baseSw.out);
  const headTag = readCacheTagFromSw(headSwSrc);

  if (!baseTag || !headTag) {
    console.error("ERROR: Could not parse CACHE tag in sw.js. Expected: const CACHE='tealclaw-v###';");
    process.exit(2);
  }

  if (baseTag.tag === headTag.tag) {
    console.error('FAIL: Behavior-affecting changes detected but SW cache version did not change.');
    console.error(`  sw.js CACHE unchanged: ${headTag.tag}`);
    console.error('  Behavior-affecting files changed:');
    for (const f of behaviorFiles) console.error(`   - ${f}`);
    console.error('');
    console.error('Fix: run `node scripts/bump-sw-cache.mjs` (or bump CACHE manually) and commit the change.');
    process.exit(3);
  }

  console.log(`OK: Behavior-affecting changes detected and SW cache bumped (${baseTag.tag} -> ${headTag.tag}).`);
  process.exit(0);
}

try {
  main();
} catch (err) {
  console.error(err?.stack || String(err));
  process.exit(1);
}
