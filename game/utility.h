#ifndef _UTILITY_H
#define _UTILITY_H

#include <iostream>
#include <Windows.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <string>

void Game_Menu_Test();
void clear();	//clears the screen
void MSdelay(int n);	//delays the next output of n amount of milliseconds
void Sdelay(int n);	//delays the next output of n amount of seconds
void Dotdot();	//dot dot dot for text
void whereText();
void whatText();
void color(int n);	//change the color
void color(char n);
void playMusic(std::string filename);	//plays audio file that loop
void endMusic();
void playSFX(std::string filename);	//plays audio file that does not loop
void DotdotTwo();	//dot dot dot for text
void ellipsis();
void playSFX_once(std::string filename);
void lockInput(bool locked);
//Ultra Deluxe: real-time keys (for Putrid's snake game)
void realtimeBegin();	//keys are read one at a time with pollByte() and not shown on screen
void realtimeEnd();	//back to normal typing; throws away any keys not read yet
int pollByte();	//next key byte, or -1 if none is waiting (arrow keys arrive as ESC [ A/B/C/D)
void frameSleep(int ms);	//wait for the next frame (not affected by the web version's text speed)	//Ultra Deluxe: true = ignore the keyboard (and throw away anything typed) until unlocked
#endif