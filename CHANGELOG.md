# Mop Mania Ultra Deluxe changelog

Changes compared with the original Mop Mania.

## Edition 1
- Title screen and web page now say "Mop Mania Ultra Deluxe".

## Edition 2 — bug fixes and polish
- **Fixed the crash** when typing a direction that isn't on the menu (like `l` in the Cellar or `b` at Home).
- **Wrong keys on the map** now just show the room's menu again, instead of replaying the room's event
  (which reopened Putrid's shop or replayed the Rusty Bucket intro).
- **Putrid Python's shop:** you can always get in. He only throws you out ("get outta here ya broke boy!!!")
  when you try to buy something you can't afford, and he checks before handing it over.
- **Putrid Python's shop:** [3] Play Game shows the "no game yet (DLC)" message instead of kicking you out.
- **Putrid Python's shop:** typing a letter no longer loops forever; it gets the "not even an option" line once.
- **Putrid Python's intro:** after "Do we got ourselves a deal?", the usual `type: [y/n]` flashes for a second
  (the player can't type during it), then "That was rhetorical".
- **Map menu:** the back option reads `[b]: Return to <room>`.
- **Pause menu:** "Go back to beginning" is now "Return Home" and "Go back" is now "Resume Game" (same order).
- **Rusty Bucket drinks:** [4] Exit goes back to the Rusty Bucket menu (the code used to check for 5).
- **Wax-coat** now has the same pause after buying as the other potions.

## Edition 3 — moves, voices, Rusty Bucket
- **Every move does something different now** (see `game/UltraDeluxeMoves.cpp`). The Attack Descriptions
  screen lists each move's damage, accuracy and effect.
  - Broomba: Vacuum Vortex (100%, heals you 25% of damage dealt), Turbo Takedown (80%, 35% daze),
    Debris Dash (90%, ignores half of enemy defense), Dust Buster Blitz (160%, then a turn to recharge)
  - JetWet: Mop'n Drop (110%, enemy defense −10%), Jittery Jetstream (80%, 30% daze),
    Foam Frenzy (50%, enemy attacks 40% likely to miss for 2 turns), Scrubbing Surge (100% + bubbles 3 turns)
  - Byson: Filth Flurry (3 hits × 40%, 80% accurate each), Turbo Tornado (90%, 30% daze),
    Particle Pulse (70%, enemy attack −15%, stacks), Dust Storm Surge (60% + dust storm 4 turns)
  - Effects reset at the start of every battle.
- **Wrong way voices:** typing a direction or key that isn't on the map menu gets a short line from
  Janitorius, then Mom, then something Rusty once told you (they take turns).
- **Rusty Bucket:** the full walk-in description only plays on your first visit. Later visits show
  two quick lines, then the options.

## Edition 4 — Putrid's snake game
- **[3] Play Game in Putrid Python's shop is a real snake game.** Purple snake (`SS` head), dead rats
  (`r~`) to eat, and it speeds up with every rat. Arrow keys or WASD; Q quits. On phones, on-screen
  arrows appear during the game.
- **Rewards:** the first game is free and pays 1 rat per 5 points. After that every game costs 1 rat
  (spent either way). Beating your high score pays 2 rats + 1 for every full 5 points above it.
- Putrid's 7 lines are placeholders like `[PUTRID: taunts you for losing your rat]` until Seamus writes them.
- The high score is kept on the character so it can be saved once saving exists.
- The web page header shows the version ("Version 4"). It comes from the newest "Edition" heading in
  this file, so it updates by itself.

## Edition 5 — Putrid Python's shop entrance
- **The Cellar menu always has `[s]: Enter Putrid Python's Shop`.** Walking back into the Cellar no
  longer opens the shop by itself; the shop opens from `[s]`.
- **Putrid's shop menu:** [1] Potions, [2] Specialty Items, [3] Play Game, **[4] Talk** (new; his lines
  are a placeholder `[PUTRID: talks with you]` until Seamus writes them), [5] Exit.
- **Text size** picker on the web page (Small / Medium / Large), remembered by your browser. Until you
  pick one it uses small on narrow screens and medium elsewhere.

## Edition 6 — character art
- **ASCII art for Broomba, Swifter JetWet and BysonV8** (from `art/mopmania_sprites_v3.txt`):
  - Character select shows each character's **large front** art above its description. On screens too
    narrow for it (phones), the small front art is shown instead so it doesn't wrap.
  - Battles show your character's **small front** art above your stats, and the **small attack** art
    every time you attack.
  - The back views (and the large attack art) are stored for later animation but not shown yet.
- The art lives in `game/CharacterArtData.cpp` as raw string literals (no escaped backslashes, every line
  within 80 columns). `tools/gen_art.py` regenerates it from the art file.

## Edition 7 — title, attack animations and credits
- **Animated title** at launch (`art/mopmania_title_and_credits.txt`): 23 frames at their listed hold
  times; the last 4 loop until you press Enter. **Enter skips the intro at any point.** Then the
  Janitorius "void" story starts as before. (Replaces the old one-line title + "Press any key".)
- **Attack animations** (`art/mopmania_attack_animations.txt`): picking any of the four attacks clears
  the screen and plays your character's small attack animation (about 120 ms a frame, the hit held
  about half a second), then goes back to the usual battle screen, attack text and damage. This
  replaces the still attack art from Edition 6.
- **Battle menu:** your character's small front art now sits to the right of the menu (above it on
  screens too narrow for both), instead of above your stats.
- **Ending + credits:** the 53-frame animated sequence replaces the old end credits screen.
- **80 x 30 screens:** the title and credits are drawn for 80 columns x 30 rows. The web version
  shrinks the text while they play if needed (phones), and the terminal version asks the window to
  grow, or asks you to make it bigger. Phones also shrink the text for the wider attack animations.
- The large attack animations and back views are stored for later but not used yet.
- Animations ignore the web page's Text speed setting, so they play the same on Instant.
- All art is raw string literals (`game/CharacterArtData.cpp`, `game/AnimationData.cpp`, generated by
  `tools/gen_art.py` and `tools/gen_animations.py`); playback is in `game/Animations.cpp`.
