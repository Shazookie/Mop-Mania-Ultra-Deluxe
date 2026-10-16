// Test-only: fights each character against a tough enemy to check move effects.
// Build: g++ -std=c++17 -w -Iplatform test/moves_test.cpp $(ls game/*.cpp | grep -v MopMania.cpp) platform/platform.cpp -o build/moves_test
// Run from game/: ../build/moves_test < /dev/null
#include "../game/character.h"
#include "../game/enemy.h"
#include "../game/UltraDeluxeMoves.h"
#include "../game/utility.h"
#include <iostream>
#include <thread>
int main() {
    character* heroes[3] = { new Broomba("Broomba", 100, 100, 150), new SwifterJetWet("JetWet", 100, 150, 100), new BysonV8("BysonV8", 150, 100, 100) };
    for (auto* c : heroes) {
        GarbageDan e(3);
        e.set_stats(2000, 60, 80, 3);
        ud_resetBattleEffects(*c, e);
        std::cout << "\n===== " << c->get_name() << " vs " << e.get_name() << " (def " << e.get_def() << ", att " << e.get_att() << ")\n";
        for (int m = 1; m <= 4; m++) {
            std::cout << "--- move " << m << "  enemy hp " << e.get_hp() << " def " << e.get_def() << " att " << e.get_att() << "\n";
            if (m == 1) c->Attack1(e); else if (m == 2) c->Attack2(e); else if (m == 3) c->Attack3(e); else c->Attack4(e);
            std::cout << "  [enemy turn]\n";
            ud_enemyTurn(*c, e);
        }
        std::cout << "--- try move 4 again while recharging: tag='" << ud_rechargeTag(*c) << "'\n";
        for (int t = 0; t < 3; t++) { std::cout << "  [enemy turn]\n"; ud_enemyTurn(*c, e); }
        std::cout << "  player hp " << c->get_hp() << "/" << c->get_maxhp() << "\n";
    }
}
