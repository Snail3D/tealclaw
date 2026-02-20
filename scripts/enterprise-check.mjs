#!/usr/bin/env node
/**
 * Enterprise check runner (best-effort).
 * Runs:
 *  - scripts/security-scan.mjs
 *  - scripts/doris-parallel-qa.mjs
 *  - firmware build (PlatformIO) if `pio` is available
 */
import { spawnSync } from 'node:child_process';

function run(cmd, args, opts = {}) {
  const res = spawnSync(cmd, args, { stdio: 'inherit', ...opts });
  return res.status === 0;
}

function hasCmd(cmd) {
  const r = spawnSync('bash', ['-lc', `command -v ${cmd} >/dev/null 2>&1`], { stdio: 'ignore' });
  return r.status === 0;
}

let ok = true;

console.log('\n== TealClaw Enterprise Checks ==');

console.log('\n[1/3] Secrets scan');
ok = run('node', ['scripts/security-scan.mjs']) && ok;

console.log('\n[2/3] Doris parallel QA');
ok = run('node', ['scripts/doris-parallel-qa.mjs']) && ok;

console.log('\n[3/3] Firmware build (optional)');
if (hasCmd('pio')) {
  ok = run('pio', ['run', '-d', 'firmware']) && ok;
} else {
  console.log('SKIP: PlatformIO (pio) not installed');
}

if (!ok) {
  console.error('\nFAILED: one or more checks failed');
  process.exit(1);
}

console.log('\nOK: all checks passed');
