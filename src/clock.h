#ifndef CLOCK_H
#define CLOCK_H

#ifdef WIN32
#include <windows.h>
#endif // WIN32


#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#include <stdbool.h>
#include "input.h"

extern uint32_t _tick;
extern uint32_t _last_tick;
extern uint32_t _num_ticks_passed;

extern uint32_t _input_tick;
extern uint32_t _last_input_tick;

// Returns true if a tick occurred;
bool LevelTimer(uint8_t level);
bool InputTimer();

extern uint32_t _nudge_delay;
extern uint32_t _last_nudge_tick;

bool NudgeTimer();

// Cross platform microseconds sleep
void sleep_us(int microseconds);

#endif // CLOCK_H
