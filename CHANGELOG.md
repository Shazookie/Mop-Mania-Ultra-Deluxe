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
