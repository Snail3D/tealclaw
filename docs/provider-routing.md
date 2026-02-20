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

- Direct provider selector in Settings > AI Configuration
- Custom base URL field with normalization
- Chat key auto-detection for Fireworks/OpenAI/Anthropic/OpenRouter/xAI prefixes
- OpenRouter chat key auto-seeds image generation key when missing
- `/keys` command now reports active direct provider + model

## Model picker

Model search now merges:

- Groq model list (live + fallback static), and
- active direct provider models (live when supported, static fallback otherwise)
