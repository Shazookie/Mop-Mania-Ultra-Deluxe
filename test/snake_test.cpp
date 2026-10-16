// Test-only: checks Putrid's snake game logic and reward math with an autopilot.
// Build: g++ -std=c++17 -w -Iplatform test/snake_test.cpp game/PutridSnake.cpp $(ls game/*.cpp | grep -v -e MopMania.cpp -e PutridSnake.cpp) platform/platform.cpp -o build/snake_test
#include "../game/PutridSnake.h"
#include <cassert>
#include <iostream>
int main() {
    // reward math (examples from Seamus's rules)
    assert(snakeFreeGameRats(23) == 4);
    assert(snakeFreeGameRats(4) == 0);
    assert(snakeHighScoreRats(20, 23) == 0);   // no new high score
    assert(snakeHighScoreRats(23, 23) == 0);   // tying isn't beating
    assert(snakeHighScoreRats(24, 23) == 2);
    assert(snakeHighScoreRats(31, 23) == 3);   // 8 above -> +1
    assert(snakeHighScoreRats(33, 23) == 4);   // 10 above -> +2
    // greedy autopilot: head toward the rat, avoid walls/self when it can
    int best = 0, totalScore = 0, wallOrSelf = 0;
    for (unsigned seed = 1; seed <= 200; seed++) {
        SnakeGame g(seed);
        int startMs = g.frameMs();
        size_t lastLen = g.body().size();
        int lastScore = 0;
        for (int t = 0; t < 5000 && g.alive(); t++) {
            auto h = g.body().front(); auto r = g.rat();
            SnakeGame::Dir want[4]; int n = 0;
            if (r.x > h.x) want[n++] = SnakeGame::RIGHT; if (r.x < h.x) want[n++] = SnakeGame::LEFT;
            if (r.y > h.y) want[n++] = SnakeGame::DOWN;  if (r.y < h.y) want[n++] = SnakeGame::UP;
            SnakeGame::Dir all[4] = {SnakeGame::UP, SnakeGame::DOWN, SnakeGame::LEFT, SnakeGame::RIGHT};
            for (auto d : all) want[n < 4 ? n++ : 3] = d;
            for (int i = 0; i < n; i++) {
                int x = h.x + (want[i]==SnakeGame::RIGHT) - (want[i]==SnakeGame::LEFT);
                int y = h.y + (want[i]==SnakeGame::DOWN) - (want[i]==SnakeGame::UP);
                if (x>=0 && x<SnakeGame::WIDTH && y>=0 && y<SnakeGame::HEIGHT && !g.occupied(x,y)) { g.turn(want[i]); break; }
            }
            g.step();
            if (g.score() > lastScore) {
                assert(g.score() == lastScore + 1);
                assert(g.body().size() == lastLen + 1);   // grows by one per rat
                assert(!g.occupied(g.rat().x, g.rat().y) || !g.alive()); // new rat never on the snake
                lastScore = g.score();
            }
            lastLen = g.body().size();
        }
        if (g.score() >= 5) assert(g.frameMs() < startMs);  // got faster
        if (!g.alive()) wallOrSelf++;
        best = std::max(best, g.score()); totalScore += g.score();
    }
    SnakeGame big(1);
    std::cout << "rewards OK; 200 autopilot games: best " << best << ", average " << totalScore / 200
              << ", crashed " << wallOrSelf << "; speed 0 rats " << big.frameMs() << "ms" << std::endl;
}
