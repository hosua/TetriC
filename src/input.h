#ifndef INPUT_H
#define INPUT_H

#include "tetris.h"
#include "sounds.h"

#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#define NUM_KEYS 6

// Key name enums
// left down up right z x
typedef enum { K_LEFT, K_DOWN, K_UP, K_RIGHT, K_Z, K_X } K_Name;
const char* K_Name_to_str(K_Name k_name);

// If keys[i] = 1, key is down, else it is up
void PrintKeys();

// Sets the key array to true or false depending on the key state
void SetKeyArray(SDL_Event event);

// Checks the arrow states in the keys array and moves the piece depending on if the key is set or not. 
void DownwardMovementHandler(uint8_t* down_points, Tetronimo* tetronimo);

// Handles rotation movement, ignores repeated keys.
void MovementHandler(SDL_Event event, Tetronimo* tetronimo);

// Mutes and unmutes sounds;
void VolumeController(SDL_Event event);

#endif // INPUT_H
