#!/usr/bin/env node
import { spawnSync } from 'node:child_process';

const intervalMs = Number(process.env.DORIS_QA_INTERVAL_MS || 15000);
const singleRun = process.argv.includes('--once');

function sh(cmd, args = []) {
  return spawnSync(cmd, args, { encoding: 'utf8' });
}

function headSha() {
  const r = sh('git', ['rev-parse', 'HEAD']);
  if (r.status !== 0) return '';
  return (r.stdout || '').trim();
}

function now() {
  return new Date().toISOString();
}

function runQa() {
  const r = sh('node', ['scripts/doris-parallel-qa.mjs']);
  const out = `${now()}\n${r.stdout || ''}${r.stderr || ''}\n`;
  process.stdout.write(out);
  return r.status === 0;
}

let last = '';

function tick(initial = false) {
  const cur = headSha();
  if (!cur) {
    console.log(`${now()} unable to read git HEAD`);
    return;
  }
  if (initial || cur !== last) {
    console.log(`${now()} HEAD ${cur} ${initial ? '(initial)' : '(changed)'}`);
    const ok = runQa();
    console.log(`${now()} QA ${ok ? 'PASS' : 'FAIL'} for ${cur}`);
    last = cur;
  }
}

tick(true);
if (!singleRun) {
  setInterval(() => tick(false), intervalMs);
}
