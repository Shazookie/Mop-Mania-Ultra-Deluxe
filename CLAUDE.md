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

## Style
- Match the existing dialogue tone and pacing (`MSdelay`, `ellipsis()`, `color()`).
- New events follow the existing `event` subclasses with `trigger(character&)`.
- No Windows-only calls; use the helpers in utility.h.
- Original Windows color numbers: 2 green, 4 red, 6 yellow, 7 default, 8 gray, 0xE light yellow.
