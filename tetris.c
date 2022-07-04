#include "graphics.h"
#include "tetris.h"

// 10x40 (However only 10x20 is visible to the player)
uint8_t play_field[FIELD_Y][FIELD_X] = {0};

void FillField(){
	for (int i = 0; i < FIELD_Y; i++){
		for (int j = 0; j < FIELD_X; j++){
			play_field[i][j] = 1;
		}
	}
}

void EmptyField(){
	for (int i = 0; i < FIELD_Y; i++){
		for (int j = 0; j < FIELD_X; j++){
			play_field[i][j] = 0;
		}
	}
}

void TestField(){
	for (int i = 0; i < 4; i++)
		play_field[20][i] = 1;
}

// The play field is numbers from lowest to greatest, top to bottom. So the bottom of the play field is not 0, it is 39.
int max_x(Tetronimo* tetronimo){
	int max = 0, x_pos = 0;
	for (int i = 0; i < NUM_PIECES; i++){
		x_pos = tetronimo->pieces[i].y;
		if (x_pos > max)
			max = x_pos;
	}
	// printf("max_x: %i\n", max);
	return max;
}
int min_x(Tetronimo* tetronimo){
	int min = INT32_MAX, x_pos = 0;
	for (int i = 0; i < NUM_PIECES; i++){
		x_pos = tetronimo->pieces[i].y;
		if (x_pos < min)
			min = x_pos;
	}
	// printf("min_x: %i\n", min);
	return min;
}
int max_y(Tetronimo* tetronimo){
	int max = 0, y_pos = 0;
	for (int i = 0; i < NUM_PIECES; i++){
		y_pos = tetronimo->pieces[i].y;
		if (y_pos > max)
			max = y_pos;
	}
	// printf("max_y: %i\n", max);
	return max;
}
int min_y(Tetronimo* tetronimo){
	int min = INT32_MAX, y_pos = 0;
	for (int i = 0; i < NUM_PIECES; i++){
		y_pos = tetronimo->pieces[i].y;
		if (y_pos < min)
			min = y_pos;
	}
	// printf("min_y: %i\n", min);
	return min;
}

// I_Piece constructor
I_Piece* new_I_Piece(){
	I_Piece* i_piece = (I_Piece*)malloc(sizeof(I_Piece));
	Tetronimo* tetronimo = &i_piece->tetronimo;
	tetronimo->t_type = T_I;
	tetronimo->d_rot = D_0;
	tetronimo->origin.x = SPAWN_X;
	tetronimo->origin.y = SPAWN_Y;

	// TODO: Instead of drawing it from the point of origin in new_piece, I should restructure this
	// so that I move the point of origin only, and then draw all iterations of the piece from the
	// point of origin (as I do here) instead of looping to move each piece. It would involve less 
	// code to do it this way.
	set_I_Piece(tetronimo);
	set_Tetronimo(tetronimo);
	return i_piece;
}

// Sets I piece with respect to its origin and degree of rotation.
void set_I_Piece(Tetronimo* tetronimo){
	Coords origin = tetronimo->origin;

	D_ROT d_rot = tetronimo->d_rot;
	switch(d_rot){
		case D_0:
		case D_180:
			for (int i = 0; i < NUM_PIECES; i++){
				tetronimo->pieces[i].x = origin.x - 1;
				tetronimo->pieces[i].y = origin.y + i;
			}
			break;
		case D_90:
		case D_270:
			for (int i = 0; i < NUM_PIECES; i++){
				tetronimo->pieces[i].x = origin.x + i - 3;
				tetronimo->pieces[i].y = origin.y + 2;
			}
			break;

	}	
}

// Sets the tetronimo onto the play_field;
void set_Tetronimo(Tetronimo* tetronimo){
	T_Type t_type = tetronimo->t_type;
	int x = 0, y = 0;
	for (int i = 0; i < NUM_PIECES; i++){
		x = tetronimo->pieces[i].x;
		y = tetronimo->pieces[i].y;
		play_field[y][x] = t_type;
	}

	// TODO: Debugging: Set the origin 
	x = tetronimo->origin.x;
	y = tetronimo->origin.y;
	play_field[y][x] = T_ORIGIN;
}


// TODO: Rewrite this such that the move is initiated on the origin, and then uses the set functions to set each piece type.
bool move_Tetronimo(SDL_Window* window, SDL_Renderer* renderer, Tetronimo* tetronimo, M_Direction dir){
	// When the piece can no longer move down further, we will need to "lock it" so that the player
	// cannot continue to rotate it.
	bool lock_piece = false;
	bool legal_move = true;
	bool can_move[4] = { 1, 1, 1, 1};
	T_Type t_type = tetronimo->t_type;
	D_ROT d_rot = tetronimo->d_rot;
	int x = 0, y = 0;
	// TODO: We shouldn't use an is_upright flag because other pieces will require 4 different degrees of rotation.
	// Unset the pieces
	for (int i = 0; i < NUM_PIECES; i++){
		x = tetronimo->pieces[i].x;
		y = tetronimo->pieces[i].y;
		play_field[y][x] = T_NONE;
	}
	// TODO: Debugging: Unset the origin 
	x =	tetronimo->origin.x;
	y =	tetronimo->origin.y;
	play_field[y][x] = T_NONE;

	int y_max = max_y(tetronimo);
	int y_min = min_y(tetronimo);
	int x_max = max_x(tetronimo);
	int x_min = min_x(tetronimo);

	// Perform the actual movement
	switch(t_type){
		case T_I:
		{
			switch(dir){
				case M_DOWN:
					// Only downward movement should determine if the piece will lock or not.
					if (y_max == FIELD_Y-1 || play_field[y_max+1][x] != T_NONE){
						lock_piece = true;
						break;
					}
					tetronimo->origin.y += 1;
					break;
				case M_LEFT:
					if (d_rot == D_0 || d_rot == D_180){
						// Unset the move flag if it is illegal
						for (int i = 0; i < NUM_PIECES; i++){
							x = tetronimo->pieces[i].x;
							y = tetronimo->pieces[i].y;
							if (x == 0 || play_field[y][x-1]){
								can_move[i] = false;
								break;
							}
						}
						// Check if all move flags are legal, if one is not, then we can't move the piece.
						for (int i = 0; i < NUM_PIECES; i++){
							if (!can_move[i]){
								legal_move = false;
								break;
							}
						}
					// Piece is horizontal, which means we only need to check one unit to the left of the leftmost piece.
					} else { 						
						if (x_min == 0 || play_field[y_max][x_min-1])
							legal_move = false;
					}
					if (legal_move){
						tetronimo->origin.x -= 1;
					}
					break;
				case M_RIGHT:
					if (d_rot == D_0 || d_rot == D_180){
						// Unset the move flag if it is illegal
						for (int i = 0; i < NUM_PIECES; i++){
							x = tetronimo->pieces[i].x;
							y = tetronimo->pieces[i].y;
							if (x == FIELD_X-1 || play_field[y][x+1]){
								can_move[i] = false;
								break;
							}
						}
						// Check if all move flags are legal, if one is not, then we can't move the piece.
						for (int i = 0; i < NUM_PIECES; i++){
							if (!can_move[i]){
								legal_move = false;
								break;
							}
						}
					// Piece is horizontal, which means we only need to check one unit to the left of the leftmost piece.
					} else { 						
						if (x_min == 0 || play_field[y_max][x_min-1])
							legal_move = false;
					}
					if (legal_move){
						tetronimo->origin.x += 1;
					}
					break;
				// Upward movement will be in for now while I test things
				case M_UP:
					if (d_rot == D_0 || d_rot == D_180){
						if (y_min == FIELD_Y/2 || play_field[y_min-1][x_max])
							legal_move = false;
					} else {
						for (int i = 0; i < NUM_PIECES; i++){
							x = tetronimo->pieces[i].x;
							y = tetronimo->pieces[i].y;
							if (y == (FIELD_Y/2)-1 || play_field[y-1][x]){
								legal_move = false;
								break;
							}
						}

					}
					if (legal_move){
						tetronimo->origin.y -= 1;
					}
					break;
				default:
					fprintf(stderr, "Error: Unknown move\n");
					break;
			}
			set_I_Piece(tetronimo);
			break;
		}
		default:
			fprintf(stderr, "Error: Unknown T_Type\n");
			break;
	}

	// printf("Moved to:\n");
	// for (int i = 0; i < NUM_PIECES; i++){
	// 	printf("(%i, %i)\n", tetronimo->pieces[i].x, tetronimo->pieces[i].y);
	// }
	set_Tetronimo(tetronimo);
	return lock_piece;
}
