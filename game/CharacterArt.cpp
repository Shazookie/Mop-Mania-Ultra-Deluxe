// Ultra Deluxe: showing the character art. See CharacterArt.h.
#include "CharacterArt.h"
#include "character.h"
#include "utility.h"

#include <iostream>

void ud_printArt(const char* art)
{
	if (art == nullptr) return;
	if (*art == '\n') art++;
	color(7);
	std::cout << art << std::endl;
}

static int artWidth(const char* art)
{
	int widest = 0, cur = 0;
	for (const char* p = art; *p; p++) {
		if (*p == '\n') { cur = 0; continue; }
		if (++cur > widest) widest = cur;
	}
	return widest;
}

void ud_printArtFit(const char* large, const char* small)
{
	int cols = terminalColumns();
	ud_printArt((cols == 0 || artWidth(large) <= cols) ? large : small);
}

static const char* smallArt(character& c, bool attacking)
{
	if (dynamic_cast<Broomba*>(&c))
		return attacking ? ART_BROOMBA_SMALL_ATTACK : ART_BROOMBA_SMALL_FRONT;
	if (dynamic_cast<SwifterJetWet*>(&c))
		return attacking ? ART_JETWET_SMALL_ATTACK : ART_JETWET_SMALL_FRONT;
	if (dynamic_cast<BysonV8*>(&c))
		return attacking ? ART_BYSON_SMALL_ATTACK : ART_BYSON_SMALL_FRONT;
	return nullptr;
}

void ud_showSmallFront(character& c)
{
	ud_printArt(smallArt(c, false));
}

void ud_showSmallAttack(character& c)
{
	ud_printArt(smallArt(c, true));
}
