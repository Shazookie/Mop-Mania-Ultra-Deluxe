// Ultra Deluxe: extra behaviour around Seamus's generated art (mopmania_art.h).
// The art itself is only read from mopmania_art; nothing here copies it.
#ifndef ULTRA_DELUXE_ART_H
#define ULTRA_DELUXE_ART_H

#include <string>

// Title intro: Enter skips it at any point; once it finishes, the last 4
// frames loop until Enter. Gets the screen to at least 80 x 30 first.
void ud_playTitle();

// The small front art to the right of the battle menu (above it on screens
// too narrow for both). Call just before the menu is printed.
void ud_showCombatArt(const std::string& name);

// playAttackAnim, but phones shrink the text so the wide frames don't wrap.
void ud_playAttackAnim(const std::string& name);

#endif
