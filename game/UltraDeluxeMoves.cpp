// Ultra Deluxe: move effects. See UltraDeluxeMoves.h.
#include "UltraDeluxeMoves.h"
#include "character.h"
#include "enemy.h"
#include "utility.h"

#include <iostream>
#include <random>

int ud_normalDamage(character& c, enemy& e, double defenseFactor)
{
    int dmg = c.get_att() - (int)(e.get_def() * defenseFactor);
    return dmg < 5 ? 5 : dmg;
}

int ud_hit(enemy& e, int amount)
{
    if (amount < 1) amount = 1;
    int before = e.get_hp();
    // enemy::damaged subtracts defense / 2, so add it back to land exactly `amount`.
    e.damaged(amount + e.get_def() / 2);
    return before - e.get_hp();
}

bool ud_chance(int percent)
{
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> roll(1, 100);
    return roll(gen) <= percent;
}

void ud_resetBattleEffects(character& c, enemy& e)
{
    c.ud_recharge = 0;
    e.ud_resetEffects();
}

const char* ud_rechargeTag(character& c)
{
    return c.ud_recharge > 0 ? " (recharging)" : "";
}

int ud_checkRecharge(character& c, int choice)
{
    while (choice == 4 && c.ud_recharge > 0) {
        color(8);
        std::cout << c.get_aName4() << " is still recharging! Pick another move." << std::endl;
        color(7);
        if (!(std::cin >> choice)) {
            return 0;  // not a number: the battle menu's own "invalid choice" handling takes it from here
        }
    }
    return choice;
}

void ud_enemyTurn(character& c, enemy& e)
{
    if (c.ud_recharge > 0) {
        c.ud_recharge--;
    }

    // Damage over time (Scrubbing Surge bubbles, Dust Storm Surge)
    if (e.ud_dotTurns > 0) {
        e.ud_dotTurns--;
        color(3);
        std::cout << e.get_name() << " " << e.ud_dotText << std::endl;
        color(7);
        ud_hit(e, e.ud_dotDamage);
        MSdelay(800);
        if (e.get_hp() <= 0) {
            return;
        }
    }

    // Dazed (Turbo Takedown, Jittery Jetstream, Turbo Tornado)
    if (e.ud_stunned) {
        e.ud_stunned = false;
        color(6);
        std::cout << e.get_name() << " is dazed and can't move!" << std::endl;
        color(7);
        MSdelay(1500);
        return;
    }

    // Slippery foam (Foam Frenzy)
    if (e.ud_slipperyTurns > 0) {
        e.ud_slipperyTurns--;
        if (ud_chance(40)) {
            color(3);
            std::cout << e.get_name() << " slips on the foam and misses!" << std::endl;
            color(7);
            MSdelay(1500);
            return;
        }
    }

    e.nextMove(c, e.RandomNum());
}
