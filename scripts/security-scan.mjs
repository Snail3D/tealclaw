#!/usr/bin/env node
/**
 * Simple repo secrets scan (best-effort).
 * - Scans tracked text files (git ls-files)
 * - Flags common API key/token formats
 * - Allows obvious placeholders like gsk_... / sk_live_xxx / TOKEN_HERE
 */
import { spawnSync } from 'node:child_process';
import fs from 'node:fs';

function sh(cmd, args) {
  const r = spawnSync(cmd, args, { encoding: 'utf8' });
  if (r.status !== 0) throw new Error((r.stderr || r.stdout || '').trim() || `${cmd} failed`);
  return (r.stdout || '').trim();
}

const files = sh('git', ['ls-files']).split('\n').filter(Boolean);

const SKIP_EXT = new Set([
  '.png', '.jpg', '.jpeg', '.gif', '.webp', '.ico', '.pdf', '.zip', '.gz', '.tgz',
  '.bin', '.elf', '.mp3', '.wav', '.mp4', '.mov'
]);

const allowPlaceholder = (s) => {
  const u = s.toLowerCase();
  return (
    u.includes('token_here') ||
    u.includes('your_token') ||
    u.includes('your-key') ||
    u.includes('your_key') ||
    u.includes('example') ||
    u.includes('gsk_...') ||
    u.includes('sk_live_...') ||
    u.includes('sk_test_...') ||
    u.includes('pplx-...') ||
    u.includes('csk_...') ||
    u.includes('rk_live_...')
  );
};

const patterns = [
  { name: 'Groq key', re: /\bgsk_[A-Za-z0-9]{20,}\b/g },
  { name: 'Google API key', re: /\bAIzaSy[A-Za-z0-9_-]{20,}\b/g },
  { name: 'Slack token', re: /\bxox[baprs]-[A-Za-z0-9-]{10,}\b/g },
  { name: 'Stripe live secret', re: /\bsk_live_[A-Za-z0-9]{10,}\b/g },
  { name: 'Stripe test secret', re: /\bsk_test_[A-Za-z0-9]{10,}\b/g },
  { name: 'JWT', re: /\beyJ[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+\.[A-Za-z0-9_-]+\b/g },
  { name: 'Private key block', re: /-----BEGIN (?:RSA|OPENSSH|EC|DSA) PRIVATE KEY-----/g },
];

let hits = 0;
const findings = [];

for (const f of files) {
  const ext = (f.match(/\.[^.\/]+$/) || [''])[0].toLowerCase();
  if (SKIP_EXT.has(ext)) continue;

  let buf;
  try { buf = fs.readFileSync(f); } catch { continue; }
  // Skip obvious binary
  const sample = buf.subarray(0, 8000);
  if (sample.includes(0)) continue;

  const text = buf.toString('utf8');
  for (const p of patterns) {
    const ms = [...text.matchAll(p.re)];
    for (const m of ms) {
      const val = m[0];
      if (allowPlaceholder(val)) continue;
      hits += 1;
      findings.push({ file: f, pattern: p.name, match: val.slice(0, 12) + '…' });
    }
  }
}

if (hits) {
  console.error('Potential secret material detected:');
  for (const x of findings.slice(0, 50)) {
    console.error(`- ${x.file}: ${x.pattern}: ${x.match}`);
  }
  if (findings.length > 50) console.error(`…and ${findings.length - 50} more`);
  process.exit(2);
}

console.log(`OK: no secrets detected in ${files.length} tracked files (best-effort)`);
