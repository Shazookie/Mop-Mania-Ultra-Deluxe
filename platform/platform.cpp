// Portable implementations of the Windows features Mop Mania uses.
// See Windows.h in this folder for the overview.
#include "Windows.h"

#include <cstdio>
#include <cctype>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#undef system  // we need the real one name-free below

// Sound IDs from the original Resources.rc (IDR_WAVE1 = 101 ...).
static const char* kWaveNames[] = {
    "background.wav", "BAH.wav", "battle!.wav", "battlemusic.wav", "coins!.wav",
    "creativity.wav", "cricket.wav", "death.wav", "door.wav", "ending.wav",
    "fanfare.wav", "gameover.wav", "GoDown.wav", "horror.wav", "jazzybeat.wav",
    "Mattari.wav", "MattariCore.wav", "metalPipe.wav", "OE.wav", "putrid.wav",
    "riser.wav", "terror.wav", "TheGrimeReaper.wav", "u-s-eas-system-full.wav",
    "victory!.wav", "vine-boom.wav", "void.wav",
};

static void flush_out() { std::cout.flush(); std::fflush(stdout); }

// ---------------------------------------------------------------- colors --
// Windows console attribute: low nibble = text color, high nibble = background.
// Windows color order (blue, green, aqua, red...) differs from ANSI order.
static const int kWinToAnsi[8] = {0, 4, 2, 6, 1, 5, 3, 7};

static void apply_attr(int attr) {
    int fg = attr & 0x0F, bg = (attr >> 4) & 0x0F;
    std::string code = "\033[0";
    if (fg != 7) {  // 7 is the game's "default white"
        int c = kWinToAnsi[fg & 7];
        code += ";" + std::to_string((fg & 8) ? 90 + c : 30 + c);
    }
    if (bg != 0) {
        int c = kWinToAnsi[bg & 7];
        code += ";" + std::to_string((bg & 8) ? 100 + c : 40 + c);
    }
    code += "m";
    std::cout << code;
}

HANDLE GetStdHandle(DWORD) { return (HANDLE)1; }
HMODULE GetModuleHandle(const char*) { return nullptr; }
BOOL SetConsoleTextAttribute(HANDLE, WORD attr) { apply_attr(attr); return 1; }

// ------------------------------------------------------------ platform io --
#if defined(__wasi__)
// Web build: these are provided by the JavaScript page (web/mopmania.js).
#define MM_IMPORT(name) __attribute__((import_module("mopmania"), import_name(name)))
MM_IMPORT("wait_key") void mm_js_wait_key(void);
MM_IMPORT("play") void mm_js_play(const char* name, int len, int loop);
MM_IMPORT("stop") void mm_js_stop(void);
MM_IMPORT("lock_input") void mm_js_lock_input(int locked);
MM_IMPORT("rt_begin") void mm_js_rt_begin(void);
MM_IMPORT("rt_end") void mm_js_rt_end(void);
MM_IMPORT("poll_byte") int mm_js_poll_byte(void);
MM_IMPORT("frame_wait") void mm_js_frame_wait(int ms);

// Scratch space the page uses to pause and resume the game while it waits
// for the player (Asyncify).
alignas(16) static unsigned char g_async_buf[1 << 20];
extern "C" __attribute__((export_name("mm_async_buf"))) void* mm_async_buf() {
    auto* hdr = reinterpret_cast<std::uint32_t*>(g_async_buf);
    hdr[0] = (std::uint32_t)(std::uintptr_t)(g_async_buf + 8);
    hdr[1] = (std::uint32_t)(std::uintptr_t)(g_async_buf + sizeof g_async_buf);
    return g_async_buf;
}

static void wait_key() { flush_out(); mm_js_wait_key(); }
static void play_file(const std::string& name, bool loop) {
    mm_js_play(name.data(), (int)name.size(), loop ? 1 : 0);
}
static void stop_sound() { mm_js_stop(); }

void lockInput(bool locked) { flush_out(); mm_js_lock_input(locked ? 1 : 0); }

void realtimeBegin() { std::cout << "\033[?25l"; flush_out(); mm_js_rt_begin(); }
void realtimeEnd() { std::cout << "\033[?25h"; flush_out(); mm_js_rt_end(); }
int pollByte() { return mm_js_poll_byte(); }
void frameSleep(int ms) { flush_out(); mm_js_frame_wait(ms); }

#else
// Native build (macOS / Linux).
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

static void wait_key() {
    flush_out();
    if (!isatty(STDIN_FILENO)) { int ch; while ((ch = std::getchar()) != EOF && ch != '\n') {} return; }
    termios old{}, raw{};
    tcgetattr(STDIN_FILENO, &old);
    raw = old;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1; raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    char c; (void)!read(STDIN_FILENO, &c, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
}

// Ultra Deluxe: stop echoing keys while locked, and throw away whatever was typed.
static termios g_locked_saved;
static bool g_locked = false;
void lockInput(bool locked) {
    flush_out();
    if (!isatty(STDIN_FILENO)) return;
    if (locked && !g_locked) {
        tcgetattr(STDIN_FILENO, &g_locked_saved);
        termios t = g_locked_saved;
        t.c_lflag &= ~(ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
        g_locked = true;
    } else if (!locked && g_locked) {
        tcflush(STDIN_FILENO, TCIFLUSH);
        tcsetattr(STDIN_FILENO, TCSANOW, &g_locked_saved);
        g_locked = false;
    }
}

// Ultra Deluxe: real-time keys for the snake game.
static termios g_rt_saved;
static bool g_rt_on = false;
void realtimeBegin() {
    std::cout << "\033[?25l";  // hide the cursor
    flush_out();
    if (!isatty(STDIN_FILENO) || g_rt_on) return;
    tcgetattr(STDIN_FILENO, &g_rt_saved);
    termios t = g_rt_saved;
    t.c_lflag &= ~(ICANON | ECHO);
    t.c_cc[VMIN] = 0;
    t.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    g_rt_on = true;
}
void realtimeEnd() {
    std::cout << "\033[?25h";  // show the cursor again
    flush_out();
    if (!g_rt_on) return;
    tcflush(STDIN_FILENO, TCIFLUSH);
    tcsetattr(STDIN_FILENO, TCSANOW, &g_rt_saved);
    g_rt_on = false;
}
int pollByte() {
    if (!g_rt_on) return -1;
    unsigned char ch;
    return read(STDIN_FILENO, &ch, 1) == 1 ? ch : -1;
}
void frameSleep(int ms) {
    flush_out();
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// Sounds play from a "sounds" folder next to where the game is run, using
// the system's command-line player (afplay on macOS, paplay/aplay on Linux).
static pid_t g_sound_pid = 0;

static void stop_sound() {
    if (g_sound_pid > 0) { kill(-g_sound_pid, SIGTERM); waitpid(g_sound_pid, nullptr, 0); }
    g_sound_pid = 0;
}

static void play_file(const std::string& name, bool loop) {
    stop_sound();
    std::string path = "sounds/" + name;
    if (access(path.c_str(), R_OK) != 0) return;  // no sound files: play silently
#ifdef __APPLE__
    std::string player = "afplay";
#else
    std::string player = "(paplay || aplay -q)";
#endif
    std::string one = player + " \"$0\" >/dev/null 2>&1";
    std::string script = loop ? "while :; do " + one + " || exit; done" : one;
    pid_t pid = fork();
    if (pid == 0) {
        setsid();
        execl("/bin/sh", "sh", "-c", script.c_str(), path.c_str(), (char*)nullptr);
        _exit(0);
    }
    if (pid > 0) g_sound_pid = pid;
}

static struct SoundCleanup { ~SoundCleanup() { stop_sound(); } } g_sound_cleanup;
#endif

// ----------------------------------------------------------------- sound --
BOOL PlaySound(const char* sound, HMODULE, DWORD flags) {
    if (sound == nullptr) { stop_sound(); return 1; }
    bool loop = (flags & SND_LOOP) != 0;
    std::uintptr_t id = (std::uintptr_t)sound;
    if (id < 0x10000) {  // MAKEINTRESOURCE(IDR_WAVEn)
        int idx = (int)id - 101;
        if (idx >= 0 && idx < (int)(sizeof kWaveNames / sizeof *kWaveNames))
            play_file(kWaveNames[idx], loop);
        return 1;
    }
    std::string name(sound);
    auto slash = name.find_last_of("/\\");
    if (slash != std::string::npos) name = name.substr(slash + 1);
    play_file(name, loop);
    return 1;
}

BOOL PlaySound(const wchar_t* sound, HMODULE mod, DWORD flags) {
    if (sound == nullptr) return PlaySound((const char*)nullptr, mod, flags);
    std::string narrow;
    for (const wchar_t* p = sound; *p; ++p) narrow += (char)*p;
    return PlaySound(narrow.c_str(), mod, flags);
}

BOOL PlaySound(int, int, int) { stop_sound(); return 1; }

// ---------------------------------------------------------------- system --
static bool iequals_prefix(const char* s, const char* prefix) {
    for (; *prefix; ++s, ++prefix)
        if (std::tolower((unsigned char)*s) != std::tolower((unsigned char)*prefix)) return false;
    return true;
}

int mm_system(const char* cmd) {
    if (!cmd) return 1;
    if (iequals_prefix(cmd, "pause")) {
        std::cout << "Press any key to continue . . . ";
        wait_key();
        std::cout << "\n";
        flush_out();
    } else if (iequals_prefix(cmd, "cls")) {
        std::cout << "\033[2J\033[3J\033[H";
        flush_out();
    } else if (iequals_prefix(cmd, "color ")) {
        // "Color XY": X = background, Y = text (hex digits)
        int attr = (int)std::strtol(cmd + 6, nullptr, 16);
        apply_attr(attr);
    }
    return 0;
}
