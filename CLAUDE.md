# Mop Mania Ultra Deluxe

Seamus's own edition of Mop Mania (text RPG by Joshua Fouch, Seamus Sommers,
and Eljay Jimenez). The original Mop Mania is kept separately and must not be
changed; all edits happen in this project.

## Layout
- `game/` — the game's C++ source. Ultra Deluxe changes go here.
- `platform/` — makes the Windows-only code build on macOS, Linux, and the web.
  Game code may keep calling `system("pause")`, `color()`, `playMusic()`, etc.
- `web/` — browser runtime, page template, compiled `mopmania.wasm`.
- `dist/index.html` — the built, self-contained web page.
- `CHANGELOG.md` — what Ultra Deluxe changed vs. the original. Update it with every change.

## Build and test
- Native: `./build.sh`, then `cd game && ../build/mopmania`
- Web: `WASI_SDK=... BINARYEN=... ./build-web.sh` (writes dist/)
- Scripted playthrough of the web build:
  `node test/play.js "y,1,y,1,m,r,1,1,1" 200` (comma-separated answers; key presses are automatic)
- After each change: rebuild both, play through the changed part with test/play.js, commit.
- Move effects: `test/moves_test.cpp` fights each character against a tough enemy (see its header).

## Planned (keep in mind)
- **Saving** is wanted soon. Keep new game state (rats, snake high score, event flags) in places
  that are easy to save and load later.
- **Putrid's snake game:** first game free (1 rat per 5 points); after that 1 rat per game (spent
  either way); a new high score pays 2 rats + 1 per full 5 points above the old high score.
  Purple snake, dead rats as food. Speed-up as it grows: not decided. Seamus writes the dialogue.
- **Rusty's shop** in the Rusty Bucket: design still being discussed.

## Working with Seamus
- Ask before writing or changing any dialogue; show him the exact lines.

## Style
- Match the existing dialogue tone and pacing (`MSdelay`, `ellipsis()`, `color()`).
- New events follow the existing `event` subclasses with `trigger(character&)`.
- No Windows-only calls; use the helpers in utility.h.
- Original Windows color numbers: 2 green, 4 red, 6 yellow, 7 default, 8 gray, 0xE light yellow.
