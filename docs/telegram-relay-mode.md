# Telegram Relay Mode (v1) — Implementation Plan

## Goal
Make Telegram the **primary transport** for TealClaw chat when desired, so users can run production workflows through Telegram/OpenClaw without requiring browser↔gateway WebSocket pairing.

## Why
Current Agent mode is powerful but setup-heavy (gateway URL/token + pairing). Many users already run OpenClaw via Telegram. Relay mode gives a simpler path:

- TealClaw sends user messages to Telegram
- TealClaw listens for Telegram replies
- Chat continues in TealClaw UI with minimal setup

## v1 Scope (implemented)
1. **New chat mode:** `telegram-relay` in Settings > Chat Mode
2. **Safe defaults:** relay is OFF by default; requires existing `tgToken` + `tgChatId`
3. **Outbound relay:** user messages (and optional image attachment) sent via Telegram Bot API
4. **Inbound relay:** polling `getUpdates` and rendering incoming Telegram messages in chat
5. **Config toggles (v1):**
   - Relay mode on/off
   - Receive inbound Telegram replies
   - Relay poll interval (seconds)
   - Optional TTS for inbound relay messages
6. **Command UX:** `/telegram relay on|off|status`
7. **Status UX:** top pill + `/keys` reflect relay state

## Compatibility / Safety
- Existing one-way Telegram forwarding (`tgEnabled`) remains unchanged.
- Existing Direct and Agent modes remain intact.
- Relay mode only activates when explicitly enabled.
- No secrets added to repo; token/chat ID stay in browser storage.

## Adapter note (contract uncertainty)
OpenClaw gateway RPC contracts for direct channel send/receive bridging are not guaranteed across builds.

For v1, TealClaw uses a **Bot API relay adapter** (stable, mockable boundary in code). Gateway-native relay can be added behind the same adapter later.

### TODO (v2)
- Add optional gateway-backed adapter path (e.g. `message.send` / channel events) when contract is standardized.
- Add webhook option (instead of polling) for lower latency and battery usage.
- Add richer media inbound rendering (voice/photo/file previews).
