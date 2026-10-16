// Test-only: plays the title (waits for Enter) and the credits in a real terminal.
// Build: g++ -std=c++17 -w -Iplatform test/anim_test.cpp $(ls game/*.cpp | grep -v MopMania.cpp) platform/platform.cpp -o build/anim_test
#include "../game/Animations.h"
#include <iostream>
int main() {
    ud_playTitle();
    std::cout << "TITLE DONE" << std::endl;
    ud_playCredits();
    std::cout << "CREDITS DONE" << std::endl;
}
