#include "clock.h"
#include "input.h"

// SDL uses ms timing, so I will too.
// TODO: Eventually, higher levels should modify this tick value to increase difficulty.
uint32_t _tick = 500;
uint32_t _last_tick = 0;
uint32_t _num_ticks_passed = 0;

// Returns true if a tick has passed, false if not.
// Increments global tick only if the duration of the previous tick has passed. 
bool TickTimer(){
	uint32_t curr_tick = SDL_GetTicks();
	if (curr_tick > _last_tick + _tick){
		_num_ticks_passed++;	
		_last_tick += _tick;
		if (VERBOSE) printf("Tick: %i\n", _num_ticks_passed);
		return true;
	}
	return false;
}

uint32_t _input_tick = 40;
uint32_t _last_input_tick = 0;

// Input Timer should only have an effect on positional movement, and not rotation.
bool InputTimer(){
	uint32_t curr_tick = SDL_GetTicks();
	if (curr_tick > _last_input_tick + _input_tick){
		_last_input_tick += _input_tick;
		if (VERBOSE) printf("input tick\n");
		return true;
	}
	return false;
}
