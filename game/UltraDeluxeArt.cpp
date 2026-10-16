// Ultra Deluxe: extra behaviour around Seamus's generated art. See UltraDeluxeArt.h.
#include "UltraDeluxeArt.h"
#include "mopmania_art.h"
#include "utility.h"

#include <iostream>

namespace {

// Draw one frame of an animation with mopmania_art's own playAnim (so its color
// masks are used), without waiting; the caller does the waiting.
void drawFrame(const ArtAnim& a, int i)
{
	ArtFrame f = a.frames[i];
	f.holdMs = 0;
	ArtAnim one = { &f, 1, a.baseColor };
	playAnim(one);
}

bool isEnter(int b) { return b == '\r' || b == '\n'; }

// Wait `ms`, but stop early (and return true) if Enter is pressed.
bool waitOrEnter(int ms)
{
	int waited = 0;
	while (true) {
		int b;
		while ((b = pollByte()) != -1) {
			if (isEnter(b)) return true;
		}
		if (waited >= ms) return false;
		int step = ms - waited < 30 ? ms - waited : 30;
		frameSleep(step);
		waited += step;
	}
}

int widest(const char* text)
{
	int w = 0, cur = 0;
	for (const char* p = text; *p; p++) {
		if (*p == '\n') { cur = 0; continue; }
		if (++cur > w) w = cur;
	}
	return w;
}

int whoIs(const std::string& n)	// same names mopmania_art expects
{
	if (n == "Broomba") return 0;
	if (n == "JetWet" || n == "Swifter JetWet") return 1;
	return 2;	// BysonV8
}

}  // namespace

void ud_playTitle()
{
	const ArtAnim& t = Art::TITLE_INTRO;
	screenFit(80, 30);
	bool live = realtimeAvailable();
	if (live) realtimeBegin(1);	// 1 = waiting for Enter

	const int loopStart = t.count - 4;	// the last 4 frames idle-loop
	bool skipped = false;
	for (int i = 0; i < loopStart && !skipped; i++) {
		drawFrame(t, i);
		if (live) skipped = waitOrEnter(t.frames[i].holdMs);
		else frameSleep(t.frames[i].holdMs);
	}
	if (live) {
		int i = loopStart;
		while (!skipped) {
			drawFrame(t, i);
			skipped = waitOrEnter(t.frames[i].holdMs);
			i = (i + 1 < t.count) ? i + 1 : loopStart;
		}
		realtimeEnd();
	}
	else {	// no real-time keys (input from a file): wait for a line
		drawFrame(t, loopStart);
		std::string line;
		std::getline(std::cin, line);
	}
	screenFit(0, 0);
	clear();
	color(7);
}

void ud_showCombatArt(const std::string& name)
{
	const char* arts[] = { Art::BROOMBA_SMALL_FRONT, Art::JETWET_SMALL_FRONT,
	                       Art::BYSON_SMALL_FRONT };
	std::string s(arts[whoIs(name)]);
	if (!s.empty() && s.front() == '\n') s.erase(0, 1);
	while (!s.empty() && s.back() == '\n') s.pop_back();

	int lines = 1;
	for (char ch : s) if (ch == '\n') lines++;
	const int column = 34;	// the menu's longest line is 26 characters
	int cols = terminalColumns();
	color(7);
	if (cols != 0 && cols < column + widest(s.c_str())) {	// too narrow
		showCombatArt(name);	// mopmania_art's version: above the menu
		return;
	}
	// Draw the art on the right, then move back up so the menu prints beside it.
	std::string out;
	size_t start = 0;
	while (start <= s.size()) {
		size_t end = s.find('\n', start);
		if (end == std::string::npos) end = s.size();
		out += "\033[" + std::to_string(column + 1) + "G";	// go to column
		out += s.substr(start, end - start) + "\n";
		start = end + 1;
	}
	out += "\033[" + std::to_string(lines) + "A\r";
	std::cout << out;
	std::cout.flush();
}

void ud_playAttackAnim(const std::string& name)
{
	const ArtAnim* anims[] = { &Art::ATTACK_BROOMBA_SMALL, &Art::ATTACK_JETWET_SMALL,
	                           &Art::ATTACK_BYSON_SMALL };
	const ArtAnim& a = *anims[whoIs(name)];
	int w = 0;
	for (int i = 0; i < a.count; i++) {
		int fw = widest(a.frames[i].text);
		if (fw > w) w = fw;
	}
	screenFit(w, 10);		// phones: shrink the text so it fits
	std::cout << "\033[?25l";	// hide the cursor
	playAttackAnim(name);
	std::cout << "\033[?25h";
	screenFit(0, 0);
}
