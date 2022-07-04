#ifndef INPUT_H
#define INPUT_H

#include "tetris.h"

#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

void InputController(SDL_Window* window, SDL_Renderer* renderer, Tetronimo* tetronimo);

#endif // INPUT_H
