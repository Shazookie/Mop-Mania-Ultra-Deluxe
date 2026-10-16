// Ultra Deluxe: Putrid Python's snake game, played from his shop.
//
// Rules (from Seamus):
//   - The first game is free and pays 1 dead rat for every 5 points.
//   - Every game after that costs 1 rat (spent win or lose).
//   - Beating your high score pays 2 rats, plus 1 more for every full 5 points
//     above the old high score.
//   - The snake speeds up as it grows.
// Putrid's lines are placeholders marked [PUTRID: ...] until Seamus writes them.
#ifndef PUTRID_SNAKE_H
#define PUTRID_SNAKE_H

#include <deque>
#include <random>

class character;

// The game itself, with no drawing or keyboard: one step() per frame.
class SnakeGame
{
public:
	static const int WIDTH = 20;	// play area in cells
	static const int HEIGHT = 12;

	enum Dir { UP, DOWN, LEFT, RIGHT };
	struct Cell { int x, y; };

	explicit SnakeGame(unsigned seed);

	void turn(Dir d);	// queue a turn (ignored if it would reverse into yourself)
	bool step();		// move one cell; returns false once the snake has crashed
	int score() const { return eaten; }
	int frameMs() const;	// how long one frame lasts at the current length (gets faster)

	const std::deque<Cell>& body() const { return snake; }	// front = head
	Cell rat() const { return food; }
	bool alive() const { return !dead; }
	bool occupied(int x, int y) const;

private:
	std::deque<Cell> snake;
	std::deque<Dir> pending;	// turns typed but not used yet (max 2)
	Dir dir = RIGHT;
	Cell food{ 0, 0 };
	int eaten = 0;
	bool dead = false;
	std::mt19937 rng;
	void placeRat();
};

// Rewards for a finished game (see the rules above).
int snakeFreeGameRats(int score);
int snakeHighScoreRats(int score, int oldHighScore);

// Putrid's shop option [3]: the whole snake game flow (free game, paid games, rewards).
void putridSnakeGame(character& c);

#endif
