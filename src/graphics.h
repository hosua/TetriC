#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include "tetris.h"
#include "queue.h"

#define FONT_PATH "fonts/pixel-letters.ttf"
#define FONT_SIZE 30


SDL_Rect GetRect(uint16_t x, uint16_t y, uint8_t block_size);
SDL_Rect* GetFieldLine(uint8_t x, uint8_t y, 
		SDL_Window* window, SDL_Renderer* renderer);
SDL_Rect* GetBlocksInLine(uint16_t dx, uint16_t dy, uint16_t y, size_t* num_blocks, 
		SDL_Window* window, SDL_Renderer* renderer);

void PrintPlayField();

// Renders the each block in the play field 
void RenderBlocks(uint16_t dx, uint16_t dy, 
		SDL_Window* window, SDL_Renderer* renderer);
// Sets the SDL Renderer to the proper color before rendering a tetronimo
void SetRenderColorByType(T_Type t_type, SDL_Renderer* renderer);

SDL_Rect* GetTetronimoByOrigin(uint16_t x, uint16_t y, uint8_t block_size, Tetronimo* tetronimo,
		SDL_Window* window, SDL_Renderer* renderer);

void ClearScreen(SDL_Window* window, SDL_Renderer* renderer);

void RenderText(int x, int y, char *text,
		SDL_Renderer *renderer, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);

void RenderUI(uint16_t dx, uint16_t dy, uint8_t block_size, char* buf, uint8_t buf_max, uint8_t curr_level,
		SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, TTF_Font* font);

void RenderQueue(uint8_t dx, uint8_t dy, uint8_t block_size, char* buf, size_t buf_max, uint8_t num_to_display,
		Queue queue, TTF_Font* font, SDL_Texture* texture, SDL_Window* window, SDL_Renderer* renderer);

void RenderStatsUI(uint8_t dx, uint8_t dy, uint8_t block_size, char* buf, size_t buf_max,
		TTF_Font* font, SDL_Texture* texture, SDL_Window* window, SDL_Renderer* renderer);

Tetronimo* draw_Piece(T_Type t_type, uint16_t x, uint16_t y);

#endif // GRAPHICS_H
