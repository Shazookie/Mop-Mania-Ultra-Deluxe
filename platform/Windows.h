// Stand-in for <Windows.h> so Mop Mania builds on macOS, Linux and the web.
//
// The game only uses a handful of Windows features. This header provides
// look-alikes for each one, implemented in platform.cpp:
//   - SetConsoleTextAttribute  -> ANSI color codes
//   - system("pause"/"CLS"/"Color ..") -> portable versions
//   - PlaySound                -> plays sounds/<name>.wav when available
// The original game code is left untouched.
#ifndef MOPMANIA_WINDOWS_SHIM_H
#define MOPMANIA_WINDOWS_SHIM_H

#include <cstdint>
#include <cstdlib>

typedef void* HANDLE;
typedef void* HMODULE;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef int BOOL;

#define STD_OUTPUT_HANDLE ((DWORD)-11)

#define SND_SYNC      0x0000
#define SND_ASYNC     0x0001
#define SND_NODEFAULT 0x0002
#define SND_LOOP      0x0008
#define SND_FILENAME  0x00020000
#define SND_RESOURCE  0x00040004

#define MAKEINTRESOURCE(i) ((const char*)(std::uintptr_t)(i))

HANDLE  GetStdHandle(DWORD which);
BOOL    SetConsoleTextAttribute(HANDLE h, WORD attr);
HMODULE GetModuleHandle(const char* name);

BOOL PlaySound(const char* sound, HMODULE mod, DWORD flags);
BOOL PlaySound(const wchar_t* sound, HMODULE mod, DWORD flags);
BOOL PlaySound(int zero, int, int);   // PlaySound(0, 0, 0) == stop

// Route the game's system("pause") / system("CLS") / system("Color 0E")
// calls through a portable implementation.
int mm_system(const char* cmd);
#define system(cmd) mm_system(cmd)

#endif
