// Ultra Deluxe: ASCII art for the three playable characters.
// The art itself is in CharacterArtData.cpp (raw string literals).
#ifndef CHARACTER_ART_H
#define CHARACTER_ART_H

class character;

// Small = combat sprites, large = character select.
// Each attack piece was drawn for one move (noted in CharacterArtData.cpp),
// but it is shown for every attack that character makes.
extern const char* const ART_BROOMBA_SMALL_FRONT;
extern const char* const ART_BROOMBA_SMALL_BACK;	// kept for later
extern const char* const ART_BROOMBA_SMALL_ATTACK;
extern const char* const ART_BROOMBA_LARGE_FRONT;
extern const char* const ART_BROOMBA_LARGE_BACK;	// kept for later
extern const char* const ART_BROOMBA_LARGE_ATTACK;	// kept for later

extern const char* const ART_JETWET_SMALL_FRONT;
extern const char* const ART_JETWET_SMALL_BACK;		// kept for later
extern const char* const ART_JETWET_SMALL_ATTACK;
extern const char* const ART_JETWET_LARGE_FRONT;
extern const char* const ART_JETWET_LARGE_BACK;		// kept for later
extern const char* const ART_JETWET_LARGE_ATTACK;	// kept for later

extern const char* const ART_BYSON_SMALL_FRONT;
extern const char* const ART_BYSON_SMALL_BACK;		// kept for later
extern const char* const ART_BYSON_SMALL_ATTACK;
extern const char* const ART_BYSON_LARGE_FRONT;
extern const char* const ART_BYSON_LARGE_BACK;		// kept for later
extern const char* const ART_BYSON_LARGE_ATTACK;	// kept for later

// Print a piece of art (skips the newline right after R"ART( ).
void ud_printArt(const char* art);

// Print `large` if it fits across the screen, otherwise `small`
// (narrow phone screens).
void ud_printArtFit(const char* large, const char* small);

// The small front sprite for whichever character `c` is (combat screen).
void ud_showSmallFront(character& c);
// The small attack sprite for whichever character `c` is (when attacking).
void ud_showSmallAttack(character& c);

#endif
