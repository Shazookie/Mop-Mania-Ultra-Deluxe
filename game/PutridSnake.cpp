// Ultra Deluxe: Putrid Python's snake game. See PutridSnake.h for the rules.
#include "PutridSnake.h"
#include "character.h"
#include "utility.h"

#include <iostream>
#include <string>

// ===================================================================== game ==

SnakeGame::SnakeGame(unsigned seed) : rng(seed)
{
	int y = HEIGHT / 2;
	snake.push_back({ 6, y });	// head
	snake.push_back({ 5, y });
	snake.push_back({ 4, y });
	placeRat();
}

bool SnakeGame::occupied(int x, int y) const
{
	for (const Cell& c : snake) {
		if (c.x == x && c.y == y) return true;
	}
	return false;
}

void SnakeGame::placeRat()
{
	std::uniform_int_distribution<int> rx(0, WIDTH - 1), ry(0, HEIGHT - 1);
	if ((int)snake.size() >= WIDTH * HEIGHT) { food = { -1, -1 }; return; }
	do {
		food = { rx(rng), ry(rng) };
	} while (occupied(food.x, food.y));
}

void SnakeGame::turn(Dir d)
{
	Dir last = pending.empty() ? dir : pending.back();
	bool reverse = (last == UP && d == DOWN) || (last == DOWN && d == UP) ||
	               (last == LEFT && d == RIGHT) || (last == RIGHT && d == LEFT);
	if (d == last || reverse || pending.size() >= 2) return;
	pending.push_back(d);
}

bool SnakeGame::step()
{
	if (dead) return false;
	if (!pending.empty()) { dir = pending.front(); pending.pop_front(); }

	Cell head = snake.front();
	if (dir == UP) head.y--;
	else if (dir == DOWN) head.y++;
	else if (dir == LEFT) head.x--;
	else head.x++;

	bool eating = (head.x == food.x && head.y == food.y);
	if (!eating) snake.pop_back();	// the tail moves out of the way first

	if (head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT || occupied(head.x, head.y)) {
		if (!eating) snake.push_back(snake.back());	// keep the length for the final picture
		dead = true;
		return false;
	}
	snake.push_front(head);
	if (eating) {
		eaten++;
		placeRat();
	}
	return true;
}

int SnakeGame::frameMs() const
{
	int ms = 190 - eaten * 7;	// speeds up with every rat
	return ms < 70 ? 70 : ms;
}

int snakeFreeGameRats(int score)
{
	return score / 5;
}

int snakeHighScoreRats(int score, int oldHighScore)
{
	if (score <= oldHighScore) return 0;
	return 2 + (score - oldHighScore) / 5;
}

// ================================================================= screen ==

namespace {

const char* PURPLE = "\033[0;35m";
const char* PURPLE_BRIGHT = "\033[0;95m";
const char* GRAY = "\033[0;90m";
const char* RESET = "\033[0m";

void drawBoard(const SnakeGame& g, int highScore, bool firstGame, const std::string& message)
{
	std::string out = "\033[H";	// redraw in place (less flicker than clearing)
	out += PURPLE;
	out += "  PUTRID'S SNAKE";
	out += RESET;
	std::string scores = "     Score: " + std::to_string(g.score()) + "    High score: " + (firstGame ? std::string("--") : std::to_string(highScore));
	out += scores + std::string(scores.size() < 30 ? 30 - scores.size() : 0, ' ') + "\n";	// pad over the last frame, but stay inside the board width

	std::string wall = GRAY + std::string((SnakeGame::WIDTH + 2) * 2, '#') + RESET + "\n";
	out += wall;
	const auto& body = g.body();
	for (int y = 0; y < SnakeGame::HEIGHT; y++) {
		out += GRAY; out += "##"; out += RESET;
		for (int x = 0; x < SnakeGame::WIDTH; x++) {
			if (body.front().x == x && body.front().y == y) {
				out += PURPLE_BRIGHT; out += "SS"; out += RESET;
			}
			else if (g.occupied(x, y)) {
				out += PURPLE; out += "ss"; out += RESET;
			}
			else if (g.rat().x == x && g.rat().y == y) {
				out += GRAY; out += "r~"; out += RESET;	// a dead rat
			}
			else {
				out += "  ";
			}
		}
		out += GRAY; out += "##"; out += RESET; out += "\n";
	}
	out += wall;
	out += GRAY;
	out += "  Arrow keys or WASD to move, Q to quit";
	out += RESET;
	out += "\n";
	out += message + "\033[K\n";	// \033[K clears the rest of the line
	std::cout << out;
	std::cout.flush();
}

// Read every waiting key; turn the snake. Returns true if the player pressed Q.
bool readKeys(SnakeGame& g)
{
	bool quit = false;
	int b;
	while ((b = pollByte()) != -1) {
		if (b == 27) {	// arrow keys: ESC [ A/B/C/D
			int b2 = pollByte();
			int b3 = pollByte();
			if (b2 == '[' || b2 == 'O') {
				if (b3 == 'A') g.turn(SnakeGame::UP);
				else if (b3 == 'B') g.turn(SnakeGame::DOWN);
				else if (b3 == 'C') g.turn(SnakeGame::RIGHT);
				else if (b3 == 'D') g.turn(SnakeGame::LEFT);
			}
		}
		else if (b == 'w' || b == 'W') g.turn(SnakeGame::UP);
		else if (b == 's' || b == 'S') g.turn(SnakeGame::DOWN);
		else if (b == 'a' || b == 'A') g.turn(SnakeGame::LEFT);
		else if (b == 'd' || b == 'D') g.turn(SnakeGame::RIGHT);
		else if (b == 'q' || b == 'Q') quit = true;
	}
	return quit;
}

// Plays one game and returns the score.
int playOneGame(int highScore, bool firstGame)
{
	SnakeGame g(std::random_device{}());
	clear();
	realtimeBegin();
	for (int n = 3; n >= 1; n--) {
		drawBoard(g, highScore, firstGame, std::string(PURPLE) + "  Get ready... " + std::to_string(n) + RESET);
		frameSleep(700);
	}
	while (pollByte() != -1) {}	// throw away anything pressed during the countdown
	bool quit = false;
	while (g.alive() && !quit) {
		drawBoard(g, highScore, firstGame, "");
		frameSleep(g.frameMs());
		quit = readKeys(g);
		if (!quit) g.step();
	}
	drawBoard(g, highScore, firstGame, std::string(PURPLE) + (quit ? "  You quit." : "  CRASH!") + RESET);
	frameSleep(1200);
	realtimeEnd();
	return g.score();
}

// Placeholder for one of Putrid's lines until Seamus writes it.
void putridLine(const std::string& placeholder)
{
	color(5);
	std::cout << "[PUTRID: " << placeholder << "]" << std::endl;
	color(7);
	MSdelay(1500);
}

void giveRats(character& c, int rats)
{
	for (int i = 0; i < rats; i++) {
		c.acq_deadRat();
		MSdelay(250);
	}
}

bool askYesNo()
{
	std::string answer;
	std::cin >> answer;
	return answer == "y" || answer == "Y";
}

}  // namespace

// ================================================================== flow ==

void putridSnakeGame(character& c)
{
	clear();

	// ---- the first game is free ----
	if (!c.ud_snakeFreeGameDone) {
		putridLine("introduces the game, sneakily egging you on to score big");
		color(8); std::cout << "Your first game is free: 1 dead rat for every 5 points." << std::endl; color(7);
		system("pause");

		int score = playOneGame(0, true);
		c.ud_snakeFreeGameDone = true;
		c.ud_snakeHighScore = score;

		clear();
		std::cout << "Score: " << score << std::endl;
		int rats = snakeFreeGameRats(score);
		giveRats(c, rats);
		if (rats == 0) {
			color(8); std::cout << "Not enough points for a rat." << std::endl; color(7);
		}
		putridLine("pays out and springs the trap: from now on every game costs a rat");
		color(8); std::cout << "From now on: 1 rat per game. Beat your high score of " << score
		                    << " to win 2 rats, plus 1 for every 5 points above it." << std::endl; color(7);
		system("pause");
		clear();
		return;
	}

	// ---- paid games ----
	while (true) {
		clear();
		if (c.get_deadRat() < 1) {
			putridLine("you have no rats to play with");
			system("pause");
			clear();
			return;
		}
		putridLine("before a paid game: the price and the high score to beat");
		color(8);
		std::cout << "High score: " << c.ud_snakeHighScore << "    Your rats: " << c.get_deadRat() << std::endl;
		color(7);
		std::cout << "Play for 1 dead rat? [y/n]" << std::endl;
		if (!askYesNo()) {
			clear();
			putridLine("you leave the game");
			MSdelay(500);
			clear();
			return;
		}

		c.Rat_Sale(1);
		int oldHigh = c.ud_snakeHighScore;
		int score = playOneGame(oldHigh, false);

		clear();
		std::cout << "Score: " << score << "    High score: " << oldHigh << std::endl;
		int rats = snakeHighScoreRats(score, oldHigh);
		if (rats > 0) {
			c.ud_snakeHighScore = score;
			color(2); std::cout << "NEW HIGH SCORE!" << std::endl; color(7);
			putridLine("snarky comment because you beat your high score");
			giveRats(c, rats);
		}
		else {
			putridLine("taunts you for losing your rat");
		}
		system("pause");
	}
}
