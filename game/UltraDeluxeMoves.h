// Ultra Deluxe: what each move actually does in battle.
//
// Before Ultra Deluxe every move dealt the same damage (attack - enemy defense / 2,
// at least 5). Now each move has its own damage, accuracy and effect, based on the
// move descriptions the team wrote. The numbers live in UltraDeluxeMoves.cpp.
#ifndef ULTRA_DELUXE_MOVES_H
#define ULTRA_DELUXE_MOVES_H

class character;
class enemy;

// The damage a normal hit would do: attack - (enemy defense * defenseFactor), at least 5.
int ud_normalDamage(character& c, enemy& e, double defenseFactor = 0.5);

// Deal exactly `amount` damage (at least 1) and print the usual "You dealt X damage" line.
// Returns the damage actually dealt.
int ud_hit(enemy& e, int amount);

// True `percent` percent of the time.
bool ud_chance(int percent);

// The enemy's turn, with move effects applied first (bubble / dust storm damage,
// being dazed, slipping on foam). Use this instead of e.nextMove(c, e.RandomNum()).
void ud_enemyTurn(character& c, enemy& e);

// Called when a battle starts: clears effects left over from the last battle.
void ud_resetBattleEffects(character& c, enemy& e);

// " (recharging)" if move 4 can't be used this turn, otherwise "".
const char* ud_rechargeTag(character& c);

// If the player picked move 4 while it is recharging, say so and ask again.
// Returns the player's new choice.
int ud_checkRecharge(character& c, int choice);

#endif
