// Ultra Deluxe: playing the animations. See Animations.h.
//
// Timing uses frameSleep() rather than MSdelay(): in the web version MSdelay
// follows the Text speed setting (Instant = no waiting), which would make the
// animations invisible and the title's "press Enter" loop spin without pause.
// In the terminal version the two are the same.
#include "Animations.h"
#include "CharacterArt.h"
#include "character.h"
#include "utility.h"

#include <iostream>
#include <string>

namespace {

const int SCREEN_COLS = 80;	// title and credits are drawn for 80 x 30
const int SCREEN_ROWS = 30;

// Clear the screen and draw one frame. No newline after the last line, so a
// full 30-row frame doesn't scroll the top line off a 30-row screen.
void showFrame(const char* art)
{
	clear();
	color(7);
	std::string s(art);
	if (!s.empty() && s.front() == '\n') s.erase(0, 1);
	if (!s.empty() && s.back() == '\n') s.pop_back();
	std::cout << s;
	std::cout.flush();
}

int frameWidth(const UDFrame* frames, int count)
{
	int widest = 0;
	for (int i = 0; i < count; i++) {
		int cur = 0;
		for (const char* p = frames[i].art; *p; p++) {
			if (*p == '\n') { cur = 0; continue; }
			if (++cur > widest) widest = cur;
		}
	}
	return widest;
}

bool isEnter(int b) { return b == '\r' || b == '\n'; }

// Wait `ms`, but stop early (and return true) if the player presses Enter.
bool waitOrEnter(int ms)
{
	for (int waited = 0; waited < ms; waited += 30) {
		int b;
		while ((b = pollByte()) != -1) {
			if (isEnter(b)) return true;
		}
		frameSleep(ms - waited < 30 ? ms - waited : 30);
	}
	int b;
	while ((b = pollByte()) != -1) {
		if (isEnter(b)) return true;
	}
	return false;
}

}  // namespace

// --------------------------------------------------------------- combat --

void ud_playAttackAnimation(character& c)
{
	const UDFrame* frames = nullptr;
	int count = 0;
	if (dynamic_cast<Broomba*>(&c)) {
		frames = UD_ANIM_BROOMBA_SMALL; count = UD_ANIM_BROOMBA_SMALL_COUNT;
	}
	else if (dynamic_cast<SwifterJetWet*>(&c)) {
		frames = UD_ANIM_JETWET_SMALL; count = UD_ANIM_JETWET_SMALL_COUNT;
	}
	else if (dynamic_cast<BysonV8*>(&c)) {
		frames = UD_ANIM_BYSONV8_SMALL; count = UD_ANIM_BYSONV8_SMALL_COUNT;
	}
	if (!frames) return;

	screenFit(frameWidth(frames, count), 10);	// phones: shrink text so it fits
	std::cout << "\033[?25l";			// hide the cursor
	for (int i = 0; i < count; i++) {
		showFrame(frames[i].art);
		frameSleep(i == count - 1 ? 500 : 120);	// hold the hit a little longer
	}
	std::cout << "\033[?25h";			// show the cursor again
	screenFit(0, 0);
	clear();
}

void ud_showArtBesideMenu(character& c)
{
	const char* art = nullptr;
	if (dynamic_cast<Broomba*>(&c)) art = ART_BROOMBA_SMALL_FRONT;
	else if (dynamic_cast<SwifterJetWet*>(&c)) art = ART_JETWET_SMALL_FRONT;
	else if (dynamic_cast<BysonV8*>(&c)) art = ART_BYSON_SMALL_FRONT;
	if (!art) return;

	std::string s(art);
	if (!s.empty() && s.front() == '\n') s.erase(0, 1);
	if (!s.empty() && s.back() == '\n') s.pop_back();

	int widest = 0, lines = 1, cur = 0;
	for (char ch : s) {
		if (ch == '\n') { lines++; cur = 0; continue; }
		if (++cur > widest) widest = cur;
	}

	const int column = 34;	// the menu's longest line is 26 characters
	int cols = terminalColumns();
	color(7);
	if (cols != 0 && cols < column + widest) {	// too narrow: art above the menu
		std::cout << s << std::endl;
		return;
	}
	// Draw the art at the right, then move back up so the menu prints beside it.
	std::string out;
	size_t start = 0;
	while (start <= s.size()) {
		size_t end = s.find('\n', start);
		if (end == std::string::npos) end = s.size();
		out += "\033[" + std::to_string(column + 1) + "G";	// move to column
		out += s.substr(start, end - start) + "\n";
		start = end + 1;
	}
	out += "\033[" + std::to_string(lines) + "A\r";
	std::cout << out;
	std::cout.flush();
}

// ---------------------------------------------------------------- title --

void ud_playTitle()
{
	screenFit(SCREEN_COLS, SCREEN_ROWS);
	bool live = realtimeAvailable();
	if (live) realtimeBegin(1);	// 1 = waiting for Enter (no arrow pad)

	const int loopStart = UD_ANIM_TITLE_COUNT - 4;	// last 4 frames idle-loop
	bool skipped = false;
	for (int i = 0; i < loopStart && !skipped; i++) {
		showFrame(UD_ANIM_TITLE[i].art);
		if (live) {
			skipped = waitOrEnter(UD_ANIM_TITLE[i].holdMs);
		}
		else {
			frameSleep(UD_ANIM_TITLE[i].holdMs);
		}
	}
	if (live) {
		int i = loopStart;
		while (!skipped) {
			showFrame(UD_ANIM_TITLE[i].art);
			skipped = waitOrEnter(UD_ANIM_TITLE[i].holdMs);
			i = (i + 1 < UD_ANIM_TITLE_COUNT) ? i + 1 : loopStart;
		}
		realtimeEnd();
	}
	else if (!skipped) {	// no real-time keys (input from a file): wait for a line
		showFrame(UD_ANIM_TITLE[loopStart].art);
		std::string line;
		std::getline(std::cin, line);
	}
	screenFit(0, 0);
	clear();
}

// -------------------------------------------------------------- credits --

void ud_playCredits()
{
	screenFit(SCREEN_COLS, SCREEN_ROWS);
	std::cout << "\033[?25l";	// hide the cursor
	for (int i = 0; i < UD_ANIM_CREDITS_COUNT; i++) {
		showFrame(UD_ANIM_CREDITS[i].art);
		frameSleep(UD_ANIM_CREDITS[i].holdMs);
	}
	std::cout << "\033[?25h" << std::endl;
	system("pause");
	screenFit(0, 0);
}
