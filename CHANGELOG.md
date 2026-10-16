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
