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
	printf("max_x: %i\n", max);
	return max;
}
int min_x(Tetronimo* tetronimo){
	int min = INT32_MAX, x_pos = 0;
	for (int i = 0; i < NUM_PIECES; i++){
		x_pos = tetronimo->pieces[i].y;
		if (x_pos < min)
			min = x_pos;
	}
	printf("min_x: %i\n", min);
	return min;
}
int max_y(Tetronimo* tetronimo){
	int max = 0, y_pos = 0;
	for (int i = 0; i < NUM_PIECES; i++){
		y_pos = tetronimo->pieces[i].y;
		if (y_pos > max)
			max = y_pos;
	}
	printf("max_y: %i\n", max);
	return max;
}


// I_Piece constructor
I_Piece* new_I_Piece(){
	I_Piece* i_piece = (I_Piece*)malloc(sizeof(I_Piece));
	i_piece->t_type = T_I;
	Tetronimo* tetronimo = &i_piece->tetronimo;
	tetronimo->is_upright = true;

	RGB_Color* color = &i_piece->color;
	// Some sort of blue
	color->r = 0;
	color->g = 102;
	color->b = 204;

	tetronimo->pos.x = SPAWN_X;
	tetronimo->pos.y = SPAWN_Y;
	int x = 0, y = 0;
	for (int i = 0; i < NUM_PIECES; i++){
		x = tetronimo->pieces[i].x = tetronimo->pos.x;
		y = tetronimo->pieces[i].y = tetronimo->pos.y + i + 1;
		printf("pieces[%i] = (%i, %i)\n", i, x, y);
	}
	set_I_Piece(i_piece);
	return i_piece;
}

bool move_I_Piece(SDL_Window* window, SDL_Renderer* renderer, I_Piece* i_piece, M_Direction dir){
	// When the piece can no longer move down further, we will need to "lock it" so that the player
	// cannot continue to rotate it.
	bool lock_piece = false;
	bool legal_move = true;
	int x = 0, y = 0;
	Tetronimo* tetronimo = &i_piece->tetronimo;
	bool is_upright = tetronimo->is_upright;
	// Unset the pieces
	for (int i = 0; i < NUM_PIECES; i++){
		x = tetronimo->pieces[i].x;
		y = tetronimo->pieces[i].y;
		play_field[y][x] = T_NONE;
	}

	int y_max = max_y(tetronimo);
	int x_max = max_x(tetronimo);
	int x_min = min_x(tetronimo);

	// Perform the actual movement
	switch(dir){
		case M_DOWN:
			// Only downward movement should determine if the piece will lock or not.
			if (y_max == FIELD_Y-1 || play_field[y_max+1][x] != T_NONE){
				lock_piece = true;
				break;
			}
			for (int i = 0; i < NUM_PIECES; i++)
				tetronimo->pieces[i].y += 1;
				
			tetronimo->pos.y += 1;
			break;
		case M_LEFT:
			if (is_upright){
				for (int i = 0; i < NUM_PIECES; i++){
					x = tetronimo->pieces[i].x;
					y = tetronimo->pieces[i].y;
					if (x == 0 || play_field[y][x-1]){
						legal_move = false;
						break;
					}
				}
			} else { // Piece is horizontal, which means we only need to check one unit to the left of the leftmost piece.
				if (x_min == 0 || play_field[y_max][x_min-1]){
					legal_move = false;
					break;
				}
			}
			if (legal_move){
				for (int i = 0; i < NUM_PIECES; i++){
					tetronimo->pieces[i].x -= 1;
				}	
				tetronimo->pos.x -= 1;
			}
			break;
		case M_RIGHT:
			if (is_upright){
				for (int i = 0; i < NUM_PIECES; i++){
					x = tetronimo->pieces[i].x;
					y = tetronimo->pieces[i].y;
					if (x == FIELD_X-1 || play_field[y][x+1]){
						legal_move = false;
						break;
					}
				}
			} else { // Piece is horizontal, which means we only need to check one unit to the right of the rightmost piece.
				if (x_max == FIELD_X-1 || play_field[y_max][x_max+1]){
					legal_move = false;
					break;
				}
			} 
			if (legal_move){
				for (int i = 0; i < NUM_PIECES; i++){
					tetronimo->pieces[i].x += 1;
				}	
				tetronimo->pos.x += 1;
			}
			
			break;
		// We should never be moving up but whatever
		case M_UP:
			for (int i = 0; i < NUM_PIECES; i++){
				tetronimo->pieces[i].y -= 1;
			}	
			tetronimo->pos.x -= 1;
			break;
	}

	printf("Moved to:\n");
	for (int i = 0; i < NUM_PIECES; i++){
		printf("(%i, %i)\n", tetronimo->pieces[i].x, tetronimo->pieces[i].y);
	}
	set_I_Piece(i_piece);
	return lock_piece;
}

// Set the play_field flags to indicate where the drawing should be done
void set_I_Piece(I_Piece* block){
	int x = 0, y = 0;
	for (int i = 0; i < NUM_PIECES; i++){
		x = block->tetronimo.pieces[i].x;
		y = block->tetronimo.pieces[i].y;
		play_field[y][x] = T_I;
	}
}
