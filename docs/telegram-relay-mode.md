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
6. **Outbound path (auto-negotiated):**
   - Tries gateway methods in order: `send` → `message.send` → `channel.send` → `channels.send` → `telegram.send`
   - Records active adapter path in relay diagnostics
   - Falls back to Telegram Bot API send path if gateway methods are unavailable

## Runtime Design
- Relay runtime binds to active OpenClaw WS connection.
- Inbound handlers process gateway events (chat + telegram/channel adapters).
- Message dedupe uses bounded seen-set and outbound echo suppression.
- Startup reconciliation:
  - Reads history via method negotiation (`chat.history`, `history.get`, `chat.sync`, `message.history`)
  - Replays only messages newer than last cursor
  - Persists cursor locally per agent+chat

## Adapter / Contract Note
Gateway channel event contracts vary by build. TealClaw now ships an active compatibility layer:
- Inbound listens to expanded event aliases (chat/channel/telegram variants)
- Outbound auto-negotiates compatible gateway send method (no hidden localStorage flags required)
- Session candidate matching includes canonical and legacy telegram key patterns
- **Main-session fallback remains disabled by default** (`tc-tg-relay-main-fallback`) to avoid cross-session leakage

## Compatibility
- v1 relay commands remain supported.
- Existing Telegram forwarding (`tgEnabled`) remains unchanged.
- Direct and Agent modes remain intact.

## Security / Safety
- No secrets are committed.
- Tokens/chat IDs remain local runtime config.
- Single inbound consumer model avoids update races while gateway is active.
