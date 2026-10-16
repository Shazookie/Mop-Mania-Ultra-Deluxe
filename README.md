# Mop Mania Ultra Deluxe

Seamus's edition of *Mop Mania: The Purging of Le' Hospitale*, the text RPG by
Joshua Fouch, Seamus Sommers, and Eljay Jimenez. See `CHANGELOG.md` for what is
different from the original.

The original only ran on Windows through Visual Studio. This copy also runs on
macOS and Linux, and in any modern web browser. The game code in `game/` started as the team's
code and is where Ultra Deluxe changes are made.

## Play it in a browser

Open `dist/index.html`. That single file is the whole game, so you can also
put it on any static web host to share it:

- **GitHub Pages:** make a repo, add `dist/index.html` as `index.html`, then
  turn on Pages under *Settings → Pages* (deploy from the main branch).
- **Netlify / Cloudflare Pages:** drag the `dist` folder onto their deploy page.

## Play it in a terminal (macOS / Linux)

```sh
./build.sh
cd game && ../build/mopmania
```

Run it from inside `game/` so it can find `endcredit.txt` (and `sounds/`, below).
On a Mac, install the compiler first with `xcode-select --install`.

On Windows, the original Visual Studio project still works as it always did.

## Sound

The `.wav` files were in `.gitignore`, so they never made it into the repo.
If you still have them, the game will use them:

- **Terminal:** put them in `game/sounds/`.
- **Web:** put them in a `sounds/` folder next to `index.html`. A *Sound*
  button appears once one loads.

The game expects these names: background, BAH, battle!, battlemusic, coins!,
creativity, cricket, death, door, ending, fanfare, gameover, GoDown, horror,
jazzybeat, Mattari, MattariCore, metalPipe, OE, putrid, riser, terror,
TheGrimeReaper, u-s-eas-system-full, victory!, vine-boom, void (each `.wav`).

## How the port works

| Folder | What's in it |
| --- | --- |
| `game/` | The original C++ source, untouched. |
| `platform/Windows.h`, `platform/platform.cpp` | A stand-in for `<Windows.h>`: console colors become ANSI colors, `system("pause")` / `system("CLS")` / `system("Color ..")` become portable versions, and `PlaySound` plays files from `sounds/`. |
| `platform/web/thread` | The browser build's `std::this_thread::sleep_for`. |
| `web/` | The browser runtime (`mopmania-runtime.js`), the page template, and the compiled `mopmania.wasm`. |
| `dist/index.html` | The finished, self-contained web page. |
| `test/` | Scripts that play the web build automatically. |

The web version compiles the same C++ to WebAssembly with
[wasi-sdk](https://github.com/WebAssembly/wasi-sdk), then runs Binaryen's
Asyncify pass so the page can pause the game while it waits for your answer
or sleeps between lines. To rebuild it, see the top of `build-web.sh`.

Two files from the original repo are left out of the build because they were
leftovers with their own copies of functions: `testercharactercode.cpp`
(a separate test program with its own `main`) and `Game_Tester_Menu.cpp`
(a duplicate of `Game_Menu_Test` in `utility.cpp`).

### Known limit

Every move between rooms calls `gameMap::play()` again without returning, so
the call stack grows the whole game. In a browser that allows about 3,000
room moves in one session before the page runs out of stack. A normal
playthrough uses far fewer.
