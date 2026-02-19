#!/usr/bin/env node
/**
 * TealClaw SW cache version bumper.
 *
 * Purpose:
 * - Enforce the "ALWAYS bump sw.js cache version" rule when behavior changes.
 * - Make the bump 1-command and hard to mess up.
 *
 * What it does:
 * - Finds: const CACHE='tealclaw-v###'
 * - Increments ### by 1 (preserves any leading zeros)
 * - Writes sw.js back to disk (unless --dry-run)
 *
 * Usage:
 *   node scripts/bump-sw-cache.mjs
 *   node scripts/bump-sw-cache.mjs --dry-run
 *   node scripts/bump-sw-cache.mjs --file ./sw.js
 */

import fs from 'node:fs';
import path from 'node:path';

function parseArgs(argv) {
  const out = { file: 'sw.js', dryRun: false };
  for (let i = 2; i < argv.length; i++) {
    const a = argv[i];
    if (a === '--dry-run' || a === '-n') out.dryRun = true;
    else if (a === '--file' || a === '-f') out.file = argv[++i];
    else if (a === '--help' || a === '-h') out.help = true;
    else throw new Error(`Unknown arg: ${a}`);
  }
  return out;
}

function bumpCacheVersion(src) {
  // Matches exactly the shipping rule string.
  const re = /const\s+CACHE\s*=\s*(['"])tealclaw-v(\d+)\1\s*;/;
  const m = src.match(re);
  if (!m) {
    throw new Error(
      "Could not find cache constant. Expected: const CACHE='tealclaw-v###';"
    );
  }

  const quote = m[1];
  const digits = m[2];
  const nextNum = String(Number(digits) + 1);
  const nextDigits = digits.length > nextNum.length
    ? nextNum.padStart(digits.length, '0')
    : nextNum;

  const from = `tealclaw-v${digits}`;
  const to = `tealclaw-v${nextDigits}`;

  const nextSrc = src.replace(re, `const CACHE=${quote}${to}${quote};`);
  return { nextSrc, from, to };
}

async function main() {
  const args = parseArgs(process.argv);
  if (args.help) {
    console.log('Usage: node scripts/bump-sw-cache.mjs [--dry-run] [--file <path>]');
    process.exit(0);
  }

  const filePath = path.resolve(process.cwd(), args.file);
  const src = fs.readFileSync(filePath, 'utf8');
  const { nextSrc, from, to } = bumpCacheVersion(src);

  if (src === nextSrc) {
    console.log(`No change needed (already at ${to}).`);
    return;
  }

  if (args.dryRun) {
    console.log(`[dry-run] Would bump ${args.file}: ${from} -> ${to}`);
    return;
  }

  fs.writeFileSync(filePath, nextSrc, 'utf8');
  console.log(`Bumped ${args.file}: ${from} -> ${to}`);
}

main().catch((err) => {
  console.error(err?.stack || String(err));
  process.exit(1);
});
