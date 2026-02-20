# TealClaw Settings UX/UI Audit (Doris)

Date: 2026-02-20  
Scope: `index.html` settings sheet + modal flows (rendered UI + code audit)

---

## 1) Full Settings UI Audit

## A. Information Architecture (IA)

### What works
- The current ordering starts with setup-critical items (API keys, agent, relay), which helps first-run success.
- Major capability groups exist (API, chat mode, voice, integrations, security, appearance, accessibility).

### IA issues
1. **Single long-scroll sheet is overloaded**
   - 18+ sections in one uninterrupted stack.
   - High cognitive load, especially on mobile.
2. **No progressive disclosure for advanced settings**
   - Beginner and expert controls sit side-by-side (e.g., Telegram relay internals + visual customization + BLE lock).
3. **No in-sheet navigation/search/filter**
   - Users must remember where settings live.
4. **Critical/destructive actions mixed with routine actions**
   - `Share` and `Clear All` are adjacent at same hierarchy level.
5. **Hidden feature discoverability issue**
   - Motion & Alarms is on long-press settings icon; this is non-obvious and poor for accessibility.

---

## B. Accessibility (WCAG-oriented)

## Keyboard/focus
1. **Settings sheet is not a dialog semantically**
   - Missing `role="dialog"`, `aria-modal`, and labeled title relationship.
2. **No focus trap while sheet is open**
   - Keyboard users can move behind modal context.
3. **Focus return is not managed**
   - Closing sheet does not reliably return focus to trigger control.
4. **Frequent full-sheet rerenders reset focus context**
   - `renderSheetContent()` after many toggles can disorient keyboard/screen reader users.

## Targets/motor
5. **Multiple controls are below 44x44 touch target guidance**
   - e.g., close buttons, some `btn-sm` actions.

## Text/legibility
6. **Many interactive/description texts at 9–10px**
   - Very small for mobile and low vision.

## Color/state
7. **Some state signaling relies heavily on subtle color differences**
   - Pills and tiny dots are not robust for low vision/color blindness.

---

## C. Readability

1. **Over-dense section composition**
   - Long explanatory copy mixed directly into control-heavy blocks.
2. **Typography scale is compressed**
   - Too many small sizes (9/10/11) with minimal hierarchy contrast.
3. **Inconsistent microcopy style**
   - Some labels are command-like (“Set up”), others descriptive; tense/style varies.
4. **Wall-of-settings effect**
   - Hard to scan quickly for “what should I do first?”

---

## D. Blind / Screen-Reader Friendliness

1. **Section headings are mostly visual `<div>`s**
   - Weak semantic heading navigation.
2. **Many inputs depend on placeholder instead of explicit label association**
   - Placeholders are not reliable labels.
3. **Toggle buttons are generic “On/Off” labels**
   - Context is visually adjacent but not always explicit in control naming.
4. **Modal context isolation is incomplete**
   - Background content isn’t clearly inert/hidden from assistive tech.
5. **Long-press-only route for Motion & Alarms**
   - Not a viable screen-reader/keyboard path.

---

## E. Visual Consistency

1. **Component patterns are mixed**
   - Segment toggles, checkboxes, card rows, inline controls, chips all in one vertical rhythm with inconsistent spacing.
2. **Heavy inline styles in render templates**
   - Hard to keep consistent over time; introduces drift.
3. **Button hierarchy is inconsistent**
   - Similar-looking actions can have very different impact (e.g., destructive vs routine).
4. **Section spacing rhythm is uneven**
   - Visual chunking can feel noisy.

---

## 2) Prioritized Fixes (quick wins first)

## P0 (same-night quick wins, highest impact)
1. **Make settings sheet a true accessible dialog**
   - Add dialog semantics + focus trap + focus return + inert background.
2. **Raise baseline legibility**
   - Minimum body/control helper text from 9/10px to 12px equivalent where possible.
3. **Standardize touch target minimums**
   - Min 44px for all tappable controls in settings.
4. **Add explicit labels/`aria-label` for close and icon-only controls**
   - Especially `sheetClose`, modal close buttons, compact action controls.
5. **Separate destructive action styling/placement**
   - Move `Clear All` to a dedicated danger zone with extra confirmation affordance.

## P1 (next pass, still fast)
6. **Add sticky in-sheet section jump nav or filter/search**
   - “Setup / Chat / Voice / Privacy / Appearance / Advanced”.
7. **Replace ambiguous On/Off button labeling with contextual switch labels**
   - Example: “High Contrast: On”.
8. **Reduce rerender focus loss**
   - Preserve active control and restore focus after rerender.

## P2 (structural improvements)
9. **Split into IA groups with progressive disclosure**
   - Basic vs Advanced sections.
10. **Create reusable settings row/toggle components**
   - Reduce inline style drift and improve consistency.

---

## 3) Concrete Component + Style Recommendations

## Spacing
- Adopt a strict 4/8 scale:
  - Section top margin: 20px
  - Section title to content: 10–12px
  - Row padding: 12–14px vertical, 14–16px horizontal
  - Control group gap: 8px
- Keep one visual density mode by default; compact mode can be optional.

## Typography
- Section label: 12px/600 (avoid tiny all-caps at 10px).
- Primary row label: 14px/600.
- Secondary/help text: 12px/400.
- Avoid 9px text for user-facing copy in settings.

## Controls
- **Switches**
  - Use one canonical switch pattern with `role="switch"` + `aria-checked`.
  - Label should include feature name and state context.
- **Buttons**
  - Primary, Secondary, Danger variants with clear contrast and shape consistency.
  - Min tap target 44x44.
- **Inputs**
  - Every input must have visible label + programmatic association.

## States
- Hover, focus, active, disabled, error, success should be consistent across settings controls.
- Focus ring: keep current visible ring pattern, ensure it appears on all interactive elements.
- Validation text should appear inline near field and in `aria-live` polite region for SR users.

---

## 4) Implementation-Ready Spec (same-night feasible)

## Spec A — Accessible Settings Dialog Foundation

### Changes
1. Add semantics to settings container:
   - `role="dialog"`
   - `aria-modal="true"`
   - `aria-labelledby="settingsTitle"`
2. Add id to title node (`id="settingsTitle"`).
3. Add `aria-label="Close settings"` to close button.
4. Implement focus trap while open.
5. Store opener element on open; restore focus on close.
6. Set non-dialog app shell inert (or `aria-hidden`) when sheet is open.

### Acceptance criteria
- [ ] Screen reader announces “Settings dialog” with title.
- [ ] Tab/Shift+Tab cycle inside dialog only.
- [ ] Escape closes dialog and focus returns to Settings trigger.
- [ ] Background is not reachable by keyboard while open.

---

## Spec B — Readability + Touch Target Quick Upgrade

### Changes
1. Update settings typography scale:
   - `.sheet-section-title` 10px → 12px
   - `.sheet-row-desc` 10px → 12px
   - helper text blocks inline styles 9/10px → 11/12px
2. Enforce minimum touch target:
   - `.btn`, `.sheet-btn`, `.mode-btn`, `.sheet-close`, key toggles min-height 44px where practical.

### Acceptance criteria
- [ ] No interactive control under 40px high in settings area (goal 44px).
- [ ] No body/helper text under 11px in settings.
- [ ] Settings remains usable on narrow mobile width without clipping.

---

## Spec C — Toggle Accessibility/Consistency

### Changes
1. Add a unified toggle helper renderer (or utility function) with:
   - contextual label,
   - explicit state text,
   - `aria-pressed` or `role="switch"` + `aria-checked`.
2. Replace ambiguous naked “On/Off” where context can be lost.

### Acceptance criteria
- [ ] SR announces feature name + current state for each toggle.
- [ ] Visual toggle style consistent across Accessibility, Experimental, Security blocks.

---

## Spec D — IA Quick Partition (low risk)

### Changes
1. Reorder sections into:
   - **Get Started** (API keys, Agent, Chat mode)
   - **Voice & Chat** (Voice Output, Telegram Relay)
   - **Automation & Devices** (Device identity, Motion/Alarms entry, BLE)
   - **Privacy & Security** (PIN, Share, Clear All, security notes)
   - **Appearance & Accessibility**
2. Move Motion & Alarms entry into visible settings row (keep long-press as shortcut only).
3. Place `Clear All` in dedicated “Danger Zone” section with red heading and stronger warning copy.

### Acceptance criteria
- [ ] New user can connect key + choose mode within first viewport + one scroll.
- [ ] Motion & Alarms accessible without long-press gesture.
- [ ] Destructive action is visually isolated and double-confirmed.

---

## Optional patch suggestions (targeted)

1. **Settings dialog semantics in markup**
```html
<div class="sheet" id="settingsSheet" role="dialog" aria-modal="true" aria-labelledby="settingsTitle">
  ...
  <div class="sheet-title" id="settingsTitle">Settings</div>
  <button class="sheet-close" id="sheetClose" aria-label="Close settings">&times;</button>
</div>
```

2. **Focus trap hooks in JS** (`openSheet` / `closeSheet`)
- Save `lastFocusedEl = document.activeElement` on open.
- On open, focus first interactive element in sheet.
- Handle Tab key loop inside sheet.
- On close, restore `lastFocusedEl.focus()`.

3. **Danger zone split**
- Move `#sheetClearAll` into dedicated section at end with distinct warning text and icon.

---

## Notes for engineering
- Current architecture can support these improvements without backend changes.
- Most P0 items are CSS/markup/utility JS edits and are same-night shippable.
- Largest UX win per effort: **dialog semantics + focus management + typography/touch target uplift**.

---

## 5) Addendum — Ruthless UX Clarity + Flow-Tested Findings (Snail direction)

This pass emphasizes **functional sense-making**, **interaction feel**, and **end-to-end usability** from actually using key flows.

### Flows exercised
1. Settings open → API key editor (Groq) → save/back.
2. Telegram Quick Setup Wizard (validation path with empty token).
3. OpenClaw Agent add flow (validation path with missing URL/token).
4. Guest link creation modal entry/cancel.
5. Keyboard traversal while settings is open.

### What felt good in live use
- Telegram wizard has clear staged structure and catches invalid token early.
- Agent add flow has immediate validation feedback.
- Key editor opens focused and is easy to understand.

### High-friction points observed in live use
1. **Flow interruption by full-context overload**
   - Even when using wizard/mini flows, user is still mentally anchored inside a giant settings stack.
2. **Error/toast residue leaks across tasks**
   - Validation messages from one flow can linger while moving to another, reducing trust/clarity.
3. **Guest link modal is powerful but cognitively heavy**
   - Too many fields shown at once for first creation pass.
4. **Keyboard flow can escape intended modal rhythm**
   - Focus behavior is not consistently constrained to the active surface.
5. **Interaction feel is serviceable but not premium**
   - Inconsistent control density + microcopy + spacing makes settings feel more "debug panel" than "product control center."

---

## 6) Competitive Pattern Benchmark (what top products do)

### A. Linear (high-performing SaaS benchmark)
Source highlights:
- `linear.app/now/settings-are-not-a-design-failure`
- `linear.app/now/how-we-redesigned-the-linear-ui`

Patterns worth copying:
1. **Settings as onboarding, not landfill**
   - Clear “what can I do here?” orientation first.
2. **Noise reduction + alignment discipline**
   - Tight layout consistency creates quality feel quickly.
3. **Stress-test major states before polish**
   - Environment + hierarchy + appearance tested before shipping.

### B. ChatGPT / Claude / Perplexity personalization model
Source highlights:
- OpenAI memory + custom instruction controls
- Claude personalization docs (profile vs project instructions vs style)
- Perplexity help snippets on account/profile-based personalization

Patterns worth copying:
1. **Layered personalization model**
   - Account-wide defaults vs task/project-specific behavior.
2. **Explicit control and reversibility**
   - Users can turn memory/customization on/off quickly.
3. **Simple labels around high-impact features**
   - Personalization and memory have plain-language framing.

### C. Accessibility/UX references for practical standards
- NNGroup progressive disclosure: reduce first-screen complexity and defer advanced options.
- Deque WCAG 2.5.5 summary: 44x44 target sizing strongly improves touch usability.
- MDN keyboard guidance: focusable controls need visible focus styling and keyboard-operable semantics.

---

## 7) How TealClaw can stay one step ahead (not just parity)

1. **Settings Command Center (first screen)**
   - At top: readiness checklist with 4 big cards:
     - Connect AI key
     - Pick chat mode
     - Configure voice
     - Test send
   - This turns setup into completion, not exploration.

2. **Smart progressive disclosure by role**
   - Default: “Essential settings” only.
   - Expand: “Power/Advanced”.
   - Persist user preference.

3. **Live quality feedback loop**
   - Add “Configuration health” score (e.g., 72/100) with one-tap fixes.
   - Example: “Agent mode selected but no gateway configured.”

4. **Inline preview-first UX**
   - Appearance + voice settings should preview instantly in-place (sample text + sample voice button).

5. **Safety-first destructive controls**
   - Dedicated danger zone with consequence preview (what will be deleted) before confirm.

---

## 8) Practical fixes that improve perceived quality tonight

### Immediate polish pack (same-night)
1. **Normalize spacing + typography in settings only**
   - 12px min helper text, 14px row labels, consistent section rhythm.
2. **Unify all toggle rows to one component shape**
   - Same hit target, label placement, state placement.
3. **Add sticky mini-header inside sheet**
   - “Get Started / Chat / Security / Appearance / Advanced” anchors.
4. **Move Share + Clear All apart**
   - `Share` under Setup/Transfer; `Clear All` in red danger zone footer.
5. **Improve microcopy for functional sense**
   - Replace generic “Set up” with “Add key”, “Connect gateway”, “Enable relay”, etc.
6. **Clear stale toasts on flow change**
   - Prevent old errors from polluting new tasks.
7. **Preserve focus + scroll context on rerender**
   - Avoid jumping/losing place after toggles.

### Acceptance criteria (perceived quality)
- [ ] First-time user can complete key+mode setup in <90 seconds without hunting.
- [ ] No settings control appears visually cramped or sub-40px target.
- [ ] Every toggle row reads as a full sentence with unambiguous state.
- [ ] Keyboard tab path stays inside active settings surface.
- [ ] Destructive action is impossible to trigger accidentally.

---

## 9) Optional patch roadmap (90–150 min)

1. **Refactor: settings row primitives** (30–45 min)
   - `SettingsSection`, `SettingsRow`, `SettingsToggleRow`, `SettingsActionRow`.
2. **Add sticky section jump bar** (20–30 min)
   - hashless in-sheet anchors + smooth scroll + active section highlight.
3. **Dialog/focus manager utility** (20–35 min)
   - shared for settings + guest link + wizard.
4. **Danger zone + action separation** (10–20 min)
5. **Microcopy pass + button label normalization** (10–20 min)

Result: TealClaw settings will feel **intentional, premium, and fast to understand** immediately, without waiting for a full architecture rewrite.

---

## 10) Strict spacing + interaction polish spec (concrete selector-level changes)

### Current measurable issue (settings render block)
- In settings rendering region (`~3200–4010`), inline text usage currently includes:
  - `font-size:9px` (13 instances)
  - `font-size:10px` (25 instances)
  - `font-size:11px` (36 instances)
- This is the primary reason the sheet feels dense/low-premium.

### Hard rules to enforce
1. **No new 9px text in settings.**
2. **10px only for tertiary metadata/badges.**
3. **All actionable labels >=12px.**
4. **All interactive controls >=44px touch target (or equivalent hit area).**
5. **Single row rhythm:** 12–14px vertical interior spacing.

### Selector-level patch targets

#### Typography and hierarchy
- `.sheet-section-title`
  - from `10px` → `12px`
- `.sheet-row-label`
  - from `13px` → `14px`
- `.sheet-row-desc`
  - from `10px` → `12px`
- `.sheet-input-label`
  - from `11px` → `12px`
- `.sheet-textarea`
  - from `11px` → `12px`

#### Hit area + control feel
- `.sheet-close`
  - from `28x28` → **`44x44`** visual/hit box
- `.btn-sm`
  - increase to minimum `height: 36px` (prefer 40+ in settings)
- `.mode-btn`
  - ensure min-height `44px`
- `.a11y-toggle`, `.exp-toggle`, `.pin-mode-btn`, `.ble-mode-btn`
  - enforce min-height `40–44px`

#### Vertical rhythm
- `.sheet-body`
  - increase bottom breathing room for mobile (`padding-bottom: 28–32px`)
- `.sheet-section`
  - standardize separation (`margin-top: 20px`)
- toggle rows currently using inline `padding:8px 0`
  - increase to `10px 0` or `12px 0` for better scanability/tap comfort.

### Microcopy normalization targets
- Replace generic right-aligned row action text `Set up` with:
  - `Add key` (API fields)
  - `Connect` (gateway)
  - `Configure` (relay)
- Replace ambiguous on/off-only labels in grouped toggles with contextual state in aria-label and/or visible companion text.

---

## 11) Real-flow QA script for tonight’s polish (hands-on)

Run this exact QA loop after patching:

1. **First-run setup clarity (90 sec test)**
   - Open settings, find Groq key, set mode, return.
   - Pass if no hunting or uncertainty.

2. **Telegram wizard friction test**
   - Start wizard with empty token, confirm validation clarity, cancel safely.
   - Pass if no confusing state residue.

3. **Agent add validation test**
   - Submit empty URL/token and confirm readable actionable error.

4. **Guest link creation cognitive load test**
   - Open/scan/create-cancel path.
   - Pass if top 3 required inputs are obvious immediately.

5. **Keyboard-only containment test**
   - Tab/Shift+Tab cycles only inside active surface.
   - Escape closes in correct order; focus returns to trigger.

6. **Mobile thumb test (simulated narrow viewport)**
   - Verify no tap targets feel cramped.
   - Verify sticky section nav (if added) remains usable.

### “Step ahead” outcome definition
TealClaw wins when settings feels like a **guided control center** (fast, obvious, confident), not a long technical form. If users can complete key setup + mode choice + one personalization in under 2 minutes without confusion, perceived quality jumps immediately.
