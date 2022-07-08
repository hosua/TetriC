#ifndef INPUT_H
#define INPUT_H

#include "tetris.h"

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
extern bool keys[NUM_KEYS];
void PrintKeys();

// Sets the key array to true or false depending on the key state
void SetKeyArray(SDL_Event event, SDL_Window* window);

// Checks the arrow states in the keys array and moves the piece depending on if the key is set or not. 
void MovementHandler(SDL_Window* window, SDL_Renderer* renderer, Tetronimo* tetronimo);

// Handles rotation movement, ignores repeated keys.
void RotationHandler(SDL_Event event, SDL_Window* window, SDL_Renderer* renderer, Tetronimo* tetronimo);


#endif // INPUT_H
