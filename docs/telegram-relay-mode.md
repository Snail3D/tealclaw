# Telegram Relay Mode (v2) — Gateway-Native Architecture

## Goal
Eliminate Telegram Bot API `getUpdates` contention between TealClaw and OpenClaw gateway by moving relay inbound to a **single-consumer gateway fanout model**.

## Problem (v1)
In v1, both TealClaw and OpenClaw gateway consumed Telegram inbound updates directly. Dual consumers caused race conditions and intermittent missed messages.

## v2 Fix Summary
1. **Removed TealClaw Bot API `getUpdates` polling** from relay runtime.
2. **Inbound source switched to gateway-native** stream/events path.
3. **Relay UX preserved** (`/telegram relay on|off|status`, Settings toggles).
4. **Startup reconciliation added** using gateway chat history + local cursor (timestamp-based) to avoid stale offset behavior.
5. **Explicit diagnostics added** in status output:
   - inbound source = `gateway-native`
   - gateway link/runtime state
   - startup reconciliation state
   - inbound path + last inbound timestamp
6. **Outbound path:**
   - Preferred: gateway `send` method (when available)
   - Fallback: existing Telegram Bot API send path (no inbound polling)

## Runtime Design
- Relay runtime binds to active OpenClaw WS connection.
- Inbound handlers process gateway events (chat + telegram/channel adapters).
- Message dedupe uses bounded seen-set and outbound echo suppression.
- Startup reconciliation:
  - Reads `chat.history` from candidate telegram session keys
  - Replays only messages newer than last cursor
  - Persists cursor locally per agent+chat

## Adapter / Contract Note
Gateway channel event contracts vary by build. TealClaw includes an adapter layer with conservative defaults:
- **Main-session fallback is disabled by default** (`tc-tg-relay-main-fallback`)
- Optional `message.send` adapter path is disabled by default (`tc-tg-relay-message-send-adapter`)
- TODO remains to tighten event contract names/shapes once fully standardized across gateway versions

## Compatibility
- v1 relay commands remain supported.
- Existing Telegram forwarding (`tgEnabled`) remains unchanged.
- Direct and Agent modes remain intact.

## Security / Safety
- No secrets are committed.
- Tokens/chat IDs remain local runtime config.
- Single inbound consumer model avoids update races while gateway is active.
