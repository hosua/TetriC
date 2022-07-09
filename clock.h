#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#include <stdbool.h>

extern uint32_t _tick;
extern uint32_t _last_tick;
extern uint32_t _num_ticks_passed;

extern uint32_t _input_tick;
extern uint32_t _last_input_tick;

// Returns true if a tick occurred;
bool LevelTimer(uint8_t level);
bool InputTimer();

// Cross platform microseconds sleep
void sleep_us(int microseconds);

