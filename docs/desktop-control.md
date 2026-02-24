# Desktop Control Foundation (macOS) — Architecture + Security Guardrails

## Scope (first shippable slice)

This foundation adds a **safe control plane** for desktop automation without executing real mouse/keyboard actions yet.

Delivered:
- Settings UI section: status + permission checklist
- Command surface:
  - `/desktop on|off|status`
  - integration commands: `desktop.on`, `desktop.off`, `desktop.status`
- Local adapter scaffold with pluggable backends
- macOS backend stub using AppleScript/System Events capability checks
- Topbar observable mode indicator when desktop control is enabled

Not delivered yet (intentionally):
- Actual mouse/keyboard injection
- Hidden/background control
- Any privilege escalation workflows

---

## Module layout

- Runtime loader: `scripts/desktop-control.js`
- UI + command integration: `index.html`

### Adapter contract

`DesktopControlAdapter` exposes:
- `status()` — capability + permission snapshot
- `on()` — enable desktop control foundation mode
- `off()` — disable mode

Backends are pluggable classes implementing the same surface.

Current backends:
1. `macos-system-events` (stub)
2. `noop` fallback for unsupported runtimes

---

## macOS backend stub behavior

`macos-system-events` currently performs **non-destructive checks only**:

1. AppleScript bridge availability check (`return "ok"`)
2. Accessibility check via:
   - `tell application "System Events" to return UI elements enabled`
3. Automation check via:
   - `tell application "System Events" to count every process`
4. Screen Recording is currently reported as **manual/unknown** until native API wiring is added.

No automation events are fired, and no input is injected.

---

## Security guardrails (hard requirements)

1. **No privilege escalation**
   - Do not run AppleScript with `with administrator privileges`
   - Do not attempt sudo elevation
2. **No hidden control**
   - Future control paths must be visibly user-mediated
3. **Explicit operator intent**
   - Enable/disable via `/desktop` commands or settings toggle
4. **Permission transparency**
   - Checklist always shows current capability state
5. **Least privilege by default**
   - Feature is off by default
6. **Fail closed**
   - If checks fail, mode does not silently become “ready”

---

## Manual permission steps for testing on macOS

Use these exact steps on the machine running TealClaw's desktop bridge host:

1. Open **System Settings** → **Privacy & Security**.
2. Open **Accessibility**:
   - Enable the host app/process running TealClaw desktop bridge (for example Terminal, iTerm, Electron host, or helper app).
3. Open **Automation**:
   - Find the same host app/process.
   - Enable control for **System Events**.
4. Open **Screen Recording**:
   - Enable the same host app/process.
5. Fully quit and relaunch the host app.
6. In TealClaw run:
   - `/desktop status`
   - `/desktop on`
   - `/desktop status` again to confirm checklist state.

If Automation/Accessibility toggles are missing, trigger `/desktop status` once first, then return to System Settings.

---

## Next implementation slice (planned)

- Native screen-recording preflight detection (replace manual/unknown status)
- Controlled, visible pointer/keyboard action API with explicit safety envelopes
- Structured action audit log for desktop operations
- Optional “kill switch” hotkey + UI stop control
