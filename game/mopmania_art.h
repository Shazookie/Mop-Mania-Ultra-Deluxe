#ifndef MOPMANIA_ART_H
#define MOPMANIA_ART_H
// Generated art for Mop Mania Ultra Deluxe. Do not edit by hand; regenerate instead.
#include <string>

struct ArtFrame { const char* text; const char* mask; int holdMs; };
struct ArtAnim  { const ArtFrame* frames; int count; int baseColor; };

namespace Art {
    extern const char* const BROOMBA_SMALL_FRONT;
    extern const char* const BROOMBA_SMALL_BACK;
    extern const char* const BROOMBA_SMALL_ATTACK;
    extern const char* const BROOMBA_LARGE_FRONT;
    extern const char* const BROOMBA_LARGE_BACK;
    extern const char* const BROOMBA_LARGE_ATTACK;
    extern const char* const JETWET_SMALL_FRONT;
    extern const char* const JETWET_SMALL_BACK;
    extern const char* const JETWET_SMALL_ATTACK;
    extern const char* const JETWET_LARGE_FRONT;
    extern const char* const JETWET_LARGE_BACK;
    extern const char* const JETWET_LARGE_ATTACK;
    extern const char* const BYSON_SMALL_FRONT;
    extern const char* const BYSON_SMALL_BACK;
    extern const char* const BYSON_SMALL_ATTACK;
    extern const char* const BYSON_LARGE_FRONT;
    extern const char* const BYSON_LARGE_BACK;
    extern const char* const BYSON_LARGE_ATTACK;
    extern const ArtAnim ATTACK_BROOMBA_SMALL;
    extern const ArtAnim ATTACK_BROOMBA_LARGE;
    extern const ArtAnim ATTACK_JETWET_SMALL;
    extern const ArtAnim ATTACK_JETWET_LARGE;
    extern const ArtAnim ATTACK_BYSON_SMALL;
    extern const ArtAnim ATTACK_BYSON_LARGE;
    extern const ArtAnim TITLE_INTRO;
    extern const ArtAnim ENDING_CREDITS;
    extern const ArtAnim PUTRID_FIRST_MEETING;
    extern const ArtAnim PUTRID_RETURN_VISIT;
}

// Print a still sprite in the current color (no clear).
void showArt(const char* art);
// Clear, print each frame (using its color mask if it has one), wait its hold time.
void playAnim(const ArtAnim& a);
// Character name as used in the game: "Broomba", "JetWet" or "BysonV8".
void showSelectArt(const std::string& name);   // large front, for character select
void showCombatArt(const std::string& name);   // small front, next to the battle menu
void playAttackAnim(const std::string& name);  // small attack animation
void playTitleIntro();      // ends on the title screen; waits for Enter
void playEndingCredits();
void playPutridFirstMeeting();
void playPutridReturnVisit();
#endif
