#ifndef TETRIS_H
#define TETRIS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_X 800
#define SCREEN_Y 550
#define FIELD_X 10
#define FIELD_Y 40 // Note that only 10x20 tiles are visible to player.
#define SPAWN_X 5 // The coordinates where the tetronimos spawn
#define SPAWN_Y 20
#define BLOCK_SIZE 25
// Number of blocks per tetronimo
#define NUM_PIECES 4 

// Movement direction
typedef enum { M_LEFT, M_DOWN, M_RIGHT, M_UP } M_Direction;

// First 20 (the top 20) lines respective to the y-axis are not visible.
// 10x40 actual, 10x20 visible
extern uint8_t play_field[FIELD_Y][FIELD_X];

// Each type of tetromino there are 8 including empties.
typedef enum { T_NONE, T_O, T_I, T_S, T_Z, T_L, T_J, T_T } T_Type;

void FillField();
void EmptyField();
void TestField();

typedef struct RGB_Color {
	uint8_t r, g, b;
} RGB_Color;

typedef struct Coords {
	uint8_t x, y;
} Coords;

typedef struct Tetronimo {
	Coords pos; // The pivot at which the piece will be rotated is (x,y)
	Coords pieces[4]; // The coordinates of each individual piece
	bool is_upright; // The position which the piece is facing will be important for determining if movement is legal.
} Tetronimo;

typedef struct I_Piece {
	T_Type t_type;
	Tetronimo tetronimo;
	RGB_Color color;
} I_Piece;

I_Piece* new_I_Piece();
bool move_I_Piece(SDL_Window* window, SDL_Renderer* renderer, I_Piece* i_piece, M_Direction dir);
// set is a helper function for new and move, you shouldn't need to call it directly
void set_I_Piece(I_Piece* block);

#endif // TETRIS_H
