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

struct RGB_Color;
struct Coords;
struct Tetronimo;
struct I_Piece;

// Movement direction (And left and right rotation)
typedef enum { 
	M_LEFT, 
	M_DOWN, 
	M_RIGHT, 
	M_UP, 
	M_ROT_RIGHT, 
	M_ROT_LEFT 
} M_Direction;

// First 20 (the top 20) lines respective to the y-axis are not visible.
// 10x40 actual, 10x20 visible
extern uint8_t play_field[FIELD_Y][FIELD_X];

// Each type of tetromino. There are 7 pieces.
// T_ORIGIN will just be for drawing the origin during debugging
// Enums will automatically assign the next enumeration as +1 of the previous value
// When checking for invalid moves, make the check if ( foo > T_ORIGIN )
typedef enum { 
	T_NONE, 
	T_ORIGIN, 
	T_O, 
	T_I, 
	T_S, 
	T_Z, 
	T_L, 
	T_J, 
	T_T
} T_Type;

// Degrees of rotation
typedef enum { 
	D_0, 
	D_90, 
	D_180, 
	D_270 
} D_ROT;

int max_x(struct Tetronimo* tetronimo);
int min_x(struct Tetronimo* tetronimo);
int max_y(struct Tetronimo* tetronimo);
int min_y(struct Tetronimo* tetronimo);

typedef struct Coords {
	uint8_t x, y;
} Coords;

typedef struct Tetronimo {
	T_Type t_type;
	Coords origin; // The pivot at which the piece will be rotated is (x,y)
	Coords pieces[4]; // The coordinates of each individual piece
	D_ROT d_rot;
} Tetronimo;

Tetronimo* new_Piece();

Tetronimo* new_I_Piece();


void set_Piece(Tetronimo* tetronimo);
// Returns true or false indicating if the piece is still falling or not
bool move_Tetronimo(SDL_Window* window, SDL_Renderer* renderer, Tetronimo* tetronimo, M_Direction dir);
// set_Tetronimo is a helper function for new and move, it shouldn't be called directly
void set_Tetronimo(Tetronimo* tetronimo);

#endif // TETRIS_H
