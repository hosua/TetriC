#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include "tetris.h"

#define FONT_PATH "fonts/pixel-letters.ttf"
#define FONT_SIZE 15


char* to_string(int n);
SDL_Rect GetRect(uint8_t x, uint8_t y);
SDL_Rect* GetFieldLine(uint8_t x, uint8_t y, SDL_Window* window, SDL_Renderer* renderer);
void RenderPlayField(SDL_Window* window, SDL_Renderer* renderer);
void PrintPlayField();

void RenderBlocksByLine(SDL_Window* window, SDL_Renderer* renderer);
void SetRenderColorByType(T_Type t_type, SDL_Renderer* renderer);

void ClearScreen(SDL_Window* window, SDL_Renderer* renderer);


#endif // GRAPHICS_H
