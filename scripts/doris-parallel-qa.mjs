#!/usr/bin/env node
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { spawnSync } from 'node:child_process';

const root = process.cwd();

function read(rel) {
  return fs.readFileSync(path.join(root, rel), 'utf8');
}

function ok(cond, msg, fail = null) {
  if (cond) return { ok: true, msg };
  return { ok: false, msg: fail || msg };
}

function section(title) {
  console.log(`\n## ${title}`);
}

const checks = [];

const indexHtml = read('index.html');
const readme = read('README.md');
const cfgRef = read('docs/config-reference.txt');
const relayDoc = read('docs/telegram-relay-mode.md');

section('Provider Runtime Checks');
checks.push(ok(/cerebras:\{label:'Cerebras'/.test(indexHtml), 'Cerebras preset present'));
checks.push(ok(/perplexity:\{label:'Perplexity'/.test(indexHtml), 'Perplexity preset present'));
checks.push(ok(/DIRECT_PROVIDER_ORDER=.*cerebras.*perplexity/.test(indexHtml), 'Provider order includes Cerebras + Perplexity'));
checks.push(ok(/val\.startsWith\('csk-'\)\|\|val\.startsWith\('csk_'\)/.test(indexHtml), 'Key detection supports csk- / csk_'));
checks.push(ok(/val\.startsWith\('pplx-'\)/.test(indexHtml), 'Key detection supports pplx-'));
checks.push(ok(/MODEL_FALLBACKS[\s\S]*cerebras:\[/.test(indexHtml), 'Model fallback list includes Cerebras'));
checks.push(ok(/MODEL_FALLBACKS[\s\S]*perplexity:\[/.test(indexHtml), 'Model fallback list includes Perplexity'));

section('Command Surface Checks');
checks.push(ok(/if\(cmd==='\/providers'\)/.test(indexHtml), '/providers command handler present'));
checks.push(ok(/if\(cmd\.startsWith\('\/provider '\)\)/.test(indexHtml), '/provider <name> command handler present'));
checks.push(ok(/SLASH COMMANDS:[\s\S]*\/providers, \/provider <name>/.test(indexHtml), 'Help command list includes provider commands'));
checks.push(ok(/\| `\/providers` \/ `\/provider <name>` \|/.test(readme), 'README command table includes provider commands'));

section('Enterprise Security UX Checks');
checks.push(ok(/Gateway URL must be https/.test(indexHtml), 'Gateway hash import enforces https (toast copy present)'));
checks.push(ok(/payload\.length>120000/.test(indexHtml), 'Config hash import has size cap'));
checks.push(ok(/Include API keys\/tokens in the encrypted link\?/.test(indexHtml), 'Encrypted share link prompts include/exclude secrets'));
checks.push(ok(/Include API keys \(Groq\/OpenAI\/etc\) in this session invitation\?/.test(indexHtml), 'Session invitation prompts include/exclude API keys'));
checks.push(ok(/No API keys were included \(safer\)\./.test(indexHtml), 'Session share bubble indicates keys excluded'));
checks.push(ok(/id="obLocalToken"[^>]*type="password"/.test(indexHtml), 'Local gateway onboarding token input is masked'));
checks.push(ok(/Auth token too large/.test(indexHtml), 'Local gateway onboarding enforces token length cap'));

section('Doc Alignment Checks');
checks.push(ok(/`cerebras`, `perplexity`/.test(cfgRef), 'Config reference lists Cerebras + Perplexity'));
checks.push(ok(/aiBaseUrl/.test(cfgRef), 'Config reference includes aiBaseUrl'));
checks.push(ok(/directFallbackToGroq/.test(cfgRef), 'Config reference includes directFallbackToGroq'));
checks.push(ok(/gatewayHttpFallback/.test(cfgRef), 'Config reference includes gatewayHttpFallback'));
checks.push(ok(/streamEnabled[\s\S]*Default: true/.test(cfgRef), 'Config reference streamEnabled default matches runtime'));
checks.push(ok(/Tries gateway methods in order: `send` → `message\.send` → `channel\.send` → `channels\.send` → `telegram\.send`/.test(relayDoc), 'Relay doc reflects outbound method negotiation'));
checks.push(ok(/Reads history via method negotiation/.test(relayDoc), 'Relay doc reflects history method negotiation'));

section('Syntax Check');
const scripts = [];
for (const m of indexHtml.matchAll(/<script([^>]*)>([\s\S]*?)<\/script>/g)) {
  const attrs = (m[1] || '').toLowerCase();
  if (attrs.includes('application/ld+json') || attrs.includes('application/json')) continue;
  scripts.push(m[2]);
}
const tmp = path.join(os.tmpdir(), `tealclaw-qa-${Date.now()}.js`);
fs.writeFileSync(tmp, scripts.join('\n'));
const syn = spawnSync('node', ['--check', tmp], { encoding: 'utf8' });
try { fs.unlinkSync(tmp); } catch {}
checks.push(ok(syn.status === 0, 'index.html script syntax check passes', syn.stderr || 'syntax check failed'));

let pass = 0;
let fail = 0;
for (const c of checks) {
  if (c.ok) {
    pass += 1;
    console.log(`PASS - ${c.msg}`);
  } else {
    fail += 1;
    console.log(`FAIL - ${c.msg}`);
  }
}

console.log(`\nSummary: ${pass} passed, ${fail} failed`);
if (fail > 0) process.exit(1);
