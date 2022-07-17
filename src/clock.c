#include "clock.h"

// global clock
Clock* _clock = NULL;
// tick = 17, input_tick = 50, nudge delay = 1000, 
void init_Clock(uint8_t tick, // The number of ms per game tick
				uint8_t input_tick, // Used to slow down the repeat rate when holding down
				uint16_t nudge_delay // Stops nudge sound from playing too often
				){
	_clock = (Clock*) malloc(sizeof(Clock));
	_clock->tick = tick;
	_clock->input_tick = input_tick;
	_clock->nudge_delay = nudge_delay;
	_clock->last_tick = 0;
	_clock->num_ticks_passed = 0;
	_clock->last_input_tick = 0;
	_clock->last_nudge_tick = 0;
}

void print_ClockVals(){

	printf("SDL_GetTicks: %i\n", SDL_GetTicks());
	printf("tick: %i\n", _clock->tick);
	printf("input tick: %i\n", _clock->input_tick);
	printf("nudge delay: %i\n", _clock->nudge_delay);
	printf("last tick: %i\n", _clock->last_tick);
	printf("num ticks passed: %i\n", _clock->num_ticks_passed);
	printf("last input tick: %i\n", _clock->last_input_tick);
	printf("last nudge tick: %i\n", _clock->last_nudge_tick);

}

// Returns true if a level tick has passed, false if not.
// Each tick is roughtly 1/60 seconds
bool LevelTimer(uint8_t level){
	uint32_t curr_tick = SDL_GetTicks();
	// Frames per Gridcell
	uint8_t fpg = 0;
	switch (level){
		/* Level 	Frames per Gridcell
		 * 00 		48
		 * 01 		43
		 * 02 		38
		 * 03 		33
		 * 04 		28
		 * 05 		23
		 * 06 		18
		 * 07 		13
		 * 08 		8
		 * 09 		6
		 * 10–12 	5
		 * 13–15 	4
		 * 16–18 	3
		 * 19–28 	2
		 * 29+ 		1
		 */
		case 0: fpg = 48; break;
		case 1: fpg = 43; break;
		case 2: fpg = 38; break;
		case 3: fpg = 33; break;
		case 4: fpg = 28; break;
		case 5: fpg = 23; break;
		case 6: fpg = 18; break;
		case 7: fpg = 13; break;
		case 8: fpg = 8; break;
		case 9: fpg = 6; break;
		case 10: case 11: case 12:
			fpg = 5;
			break;
		case 13: case 14: case 15:
			fpg = 4;
			break;
		case 16: case 17: case 18:
			fpg = 3;
			break;
		case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28:
			fpg = 2;
			break;
		default:
			fpg = 1;
			break;
	}
	if (curr_tick > _clock->last_tick + (_clock->tick * fpg)){
		_clock->num_ticks_passed++;	
		_clock->last_tick += (_clock->tick * fpg);
		if (VERBOSE) printf("Tick: %i\n", _clock->num_ticks_passed);
		return true;
	}
	return false;
}

// Input Timer will only have an effect on downward movement.
bool InputTimer(){
	uint32_t curr_tick = SDL_GetTicks();
	if (curr_tick > _clock->last_input_tick + _clock->input_tick){
		_clock->last_input_tick += _clock->input_tick;
		return true;
	}
	return false;
}

// A timer to prevent the nudge sound from being spammed
// Returns true if nudge timer bool should be reset
bool NudgeTimer(){
	uint32_t curr_tick = SDL_GetTicks();
	if (curr_tick > _clock->last_nudge_tick + _clock->nudge_delay){
		_clock->last_nudge_tick += _clock->nudge_delay;
		return true;
	}
	return false;
}

/** https://gist.github.com/rafaelglikis/ee7275bf80956a5308af5accb4871135
 * Converted this to use microseconds instead of milliseconds
 * Cross-platform sleep function for C
 * @param int microseconds 
 * This is being used to limit FPS to ~60
 */
void sleep_us(int microseconds){
    #ifdef WIN32
        Sleep(microseconds/1000);
    #elif _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = microseconds / 1000000;
        ts.tv_nsec = (microseconds % 1000000) * 1000;
        nanosleep(&ts, NULL);
    #else
        usleep(microseconds);
    #endif
}
