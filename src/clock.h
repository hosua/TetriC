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

typedef struct Clock {
	uint8_t tick;
	uint32_t last_tick;
	uint32_t num_ticks_passed;

	uint8_t input_tick;
	uint32_t last_input_tick;

	uint16_t nudge_delay;
	uint16_t last_nudge_tick;
} Clock;

extern Clock* _clock;
void print_ClockVals();

bool NudgeTimer();
void init_Clock(uint8_t tick, // The number of ms per game tick
				uint8_t input_tick, // Used to slow down the repeat rate when holding down
				uint16_t nudge_delay // Stops nudge sound from playing too often
				);

// Returns true if a tick occurred;
bool LevelTimer(uint8_t level);
bool InputTimer();

// Cross platform microseconds sleep
void sleep_us(int microseconds);

#endif // CLOCK_H
