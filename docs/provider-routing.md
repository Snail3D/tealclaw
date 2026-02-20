# Provider Routing + Gateway Fallback (Night Shift update)

## Direct providers

TealClaw direct mode now supports:

- Groq
- Fireworks
- OpenAI
- Anthropic
- OpenRouter
- xAI
- Mistral
- Together
- DeepSeek

### Routing keys

- `aiProvider` — selected direct provider id
- `aiKey` — direct provider API key (non-Groq)
- `whisperKey` — Groq key (voice/research + optional fallback)
- `aiBaseUrl` — optional custom provider base URL
- `directFallbackToGroq` — default `true`

### Fallback behavior

If selected provider fails with a recoverable error (network/429/5xx), TealClaw can auto-fallback to Groq when:

1. `directFallbackToGroq` is enabled, and
2. a valid Groq key exists.

## Gateway compatibility

Gateway mode still prefers WebSocket.

Optional HTTP fallback now supports multiple compatibility paths:

- `/v1/chat/completions`
- `/chat/completions`
- `/api/v1/chat/completions`

Auth compatibility attempts:

- `Authorization: Bearer <token>`
- `x-api-key: <token>`

### Gateway fallback control

- `gatewayHttpFallback` (default `false`)
- Auto-allowed for localhost / same-origin gateways

## UX/auth hooks added

- Background-first routing controls (auto-save, no manual apply flow)
- Automation status indicator (active provider + fallback states + validation)
- Override controls only: direct fallback + gateway HTTP fallback toggles
- Advanced routing fields collapsed under "Advanced routing overrides"
- Chat key auto-detection for Fireworks/OpenAI/Anthropic/OpenRouter/xAI prefixes
- OpenRouter chat key auto-seeds image generation key when missing
- `/keys` command now reports active direct provider + model

## Quick-action backend command layer

A backend integration command router now handles larger integration actions with minimal UI:

- `gateway.reconnect`
- `provider.set`
- `provider.fallback`
- `gateway.httpfallback`
- `relay.on` / `relay.off` / `relay.status`
- `health.check`

Trigger paths:

- Slash: `/grok <action>`
- Natural text trigger: `grok <action>` / `quick action <action>` / `qa <action>`
- tc-action command wiring: `integration:<command>` or `{"type":"command","integration":"..."}`

## Model picker

Model search now merges:

- Groq model list (live + fallback static), and
- active direct provider models (live when supported, static fallback otherwise)
