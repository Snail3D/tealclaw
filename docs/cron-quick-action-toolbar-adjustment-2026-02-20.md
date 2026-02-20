# Cron Quick Action Toolbar Adjustment (2026-02-20)

## Request implemented
- Moved/kept cron quick action in the **top toolbar next to Help**.
- Removed the extra dedicated timer/alarm row below the header.
- Updated icon to a clearer **alarm-clock + plus** glyph.

## UX behavior now
- Cron button remains in top toolbar (`#cronAddBtn`) and opens cron modal as before.
- Button now shows active schedule/alarm/timer/pomodoro count via badge (`#cronQuickCount`).
- Button title/aria updates dynamically to include active count (e.g., `Schedules & alarms (3 active)`).

## Technical changes
- `index.html`
  - Topbar cron button icon updated.
  - Added `cron-quick-count` badge styles.
  - Removed `timer-alarm-row` markup block.
  - Simplified `renderCronBar()` to update toolbar badge/state instead of rendering row pills.

## Notes for branch coordination
- This touches the topbar icon/button area and cron rendering logic in `index.html`.
- If Doris is editing adjacent topbar/settings markup, rebase carefully around:
  - `#cronAddBtn` markup
  - removed `#timerAlarmRow`
  - `renderCronBar()` implementation
