#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#include <stdbool.h>

extern uint32_t _tick;
extern uint32_t _last_tick;
extern uint32_t _num_ticks_passed;

// Returns true if a tick occurred;
bool TickCounter();
