#include "clock.h"
#include "input.h"

// SDL uses ms timing, so I will too.
uint32_t _tick = 500;
uint32_t _last_tick = 0;
uint32_t _num_ticks_passed = 0;

// Increments global tick only if the duration of the previous tick has passed
bool TickCounter(){
	uint32_t curr_tick = SDL_GetTicks();
	if (curr_tick > _last_tick + _tick){
		_num_ticks_passed++;	
		_last_tick += _tick;
		printf("Tick: %i\n", _num_ticks_passed);
		return true;
	}
	return false;
}
