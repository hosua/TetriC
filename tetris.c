#include "graphics.h"
#include "tetris.h"

// 10x40 (However only 10x20 is visible to the player)
uint8_t play_field[FIELD_Y][FIELD_X] = {0};

// The play field is numbers from lowest to greatest, top to bottom. So the bottom of the play field is not 0, it is 39.
int max_x(Tetronimo* tetronimo){
	int max = 0, x_pos = 0;
	for (int i = 0; i < NUM_PIECES; i++){
		x_pos = tetronimo->pieces[i].x;
		if (x_pos > max)
			max = x_pos;
	}
	printf("max_x: %i\n", max);
	return max;
}
int min_x(Tetronimo* tetronimo){
	int min = INT32_MAX, x_pos = 0;
	for (int i = 0; i < NUM_PIECES; i++){
		x_pos = tetronimo->pieces[i].x;
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
int min_y(Tetronimo* tetronimo){
	int min = INT32_MAX, y_pos = 0;
	for (int i = 0; i < NUM_PIECES; i++){
		y_pos = tetronimo->pieces[i].y;
		if (y_pos < min)
			min = y_pos;
	}
	printf("min_y: %i\n", min);
	return min;
}

// Generate a random piece via RNG
Tetronimo* new_Piece(){
	return new_I_Piece();
}
// I_Piece constructor
Tetronimo* new_I_Piece(){
	Tetronimo* i_piece = (Tetronimo*)malloc(sizeof(Tetronimo));
	i_piece->t_type = T_I;
	i_piece->d_rot = D_0;
	i_piece->origin.x = SPAWN_X;
	i_piece->origin.y = SPAWN_Y;

	set_Piece(i_piece);
	set_Tetronimo(i_piece);
	return i_piece;
}

// Helper function, do not call set_Piece() directly.
// Sets piece with respect to its origin and degree of rotation (This does not set the actual play_field)
void set_Piece(Tetronimo* tetronimo){
	Coords origin = tetronimo->origin;
	T_Type t_type = tetronimo->t_type;
	D_ROT d_rot = tetronimo->d_rot;
	switch(t_type){
		// I-Piece
		case T_I:
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
			break;
		default:
			printf("ERROR: Could not set invalid T_Type\n");
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

	// TODO: Debugging: Set the origin. Origin breaks gameplay so we need to turn this off when testing gameplay
	// x = tetronimo->origin.x;
	// y = tetronimo->origin.y;
	// play_field[y][x] = T_ORIGIN;
}

// Returns the d_rot value of the tetronimo
D_ROT rotate_Tetronimo(Tetronimo* tetronimo, M_Direction dir){
	D_ROT d_rot = tetronimo->d_rot;
	switch(dir){
		case M_ROT_RIGHT:
			// Allow our d_rot value to wrap around the enumeration in both directions
			if (d_rot < 3)
				d_rot++;
			else
				d_rot = 0;
			
			break;
		case M_ROT_LEFT:
			if (d_rot > 0)
				d_rot--;
			else
				d_rot = 3;
			break;
		default:
			fprintf(stderr, "Error: Illegal rotation\n");
			break;
	}
	return d_rot;
}

void print_tetronimo_coords(Tetronimo *tetronimo){
	Coords* coords = tetronimo->pieces;
	for (int i = 0; i < NUM_PIECES; i++)
		printf("(%i, %i) ", coords[i].x, coords[i].y);
	printf("\n");
}

// Move tetronimo handles actually moving the pieces to their respective coordinates, as well as checking if a move 
// is legal or not.
bool move_Tetronimo(SDL_Window* window, SDL_Renderer* renderer, Tetronimo* tetronimo, M_Direction dir){
	// When the piece can no longer move down further, we will need to "lock it" so that the player
	// cannot continue to rotate it.
	bool is_falling = true;
	bool legal_move = true;
	bool can_move[4] = { 1, 1, 1, 1};
	T_Type t_type = tetronimo->t_type;
	D_ROT d_rot = tetronimo->d_rot;
	printf("d_rot: %i\n", d_rot);
	print_tetronimo_coords(tetronimo);
	int x = 0, y = 0;
	// Unset the pieces
	for (int i = 0; i < NUM_PIECES; i++){
		x = tetronimo->pieces[i].x;
		y = tetronimo->pieces[i].y;
		play_field[y][x] = T_NONE;
	}
	// TODO: Debugging: Unset the origin. Origin breaks gameplay so we need to turn this off when testing gameplay
	// x =	tetronimo->origin.x;
	// y =	tetronimo->origin.y;
	// play_field[y][x] = T_NONE;

	int y_max = max_y(tetronimo);
	int y_min = min_y(tetronimo);
	int x_max = max_x(tetronimo);
	int x_min = min_x(tetronimo);

	// Perform the actual movement
	switch(t_type){
		case T_I:
		{
			switch(dir){
				case M_ROT_RIGHT:
					// TODO: Need to implement bounary checks before performing the rotation
					tetronimo->d_rot = rotate_Tetronimo(tetronimo, M_ROT_RIGHT);
					break;
				case M_ROT_LEFT:
					// TODO: Need to implement bounary checks before performing the rotation
					tetronimo->d_rot = rotate_Tetronimo(tetronimo, M_ROT_LEFT);
					break;
				case M_DOWN:
					if (d_rot == D_0 || d_rot == D_180){
						if (y_max == FIELD_Y-1 || play_field[y_max+1][x_max] > T_ORIGIN)
							legal_move = false;
					} else {
						for (int i = 0; i < NUM_PIECES; i++){
							x = tetronimo->pieces[i].x;
							y = tetronimo->pieces[i].y;
							if (y == FIELD_Y-1 || play_field[y+1][x] > T_ORIGIN){
								legal_move = false;
								break;
							}
						}

					}
					if (legal_move){
						tetronimo->origin.y += 1;
					} else {
						// Only downward movement should determine if the piece will lock or not.
						is_falling = false;
					}
					break;
				case M_LEFT:
					if (d_rot == D_0 || d_rot == D_180){
						// Unset the move flag if it is illegal
						for (int i = 0; i < NUM_PIECES; i++){
							x = tetronimo->pieces[i].x;
							y = tetronimo->pieces[i].y;
							if (x == 0 || play_field[y][x-1] > T_ORIGIN){
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
					// Piece is horizontal, which means we only need to check one unit to the left of the left-most piece.
					} else { 						
						if (x_min == 0 || play_field[y_max][x_min-1] > T_ORIGIN)
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
							if (x == FIELD_X-1 || play_field[y][x+1] > T_ORIGIN){
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
					// Piece is horizontal, which means we only need to check one unit to the right of the right-most piece.
					} else { 						
						if (x_max == FIELD_X-1 || play_field[y_max][x_max+1] > T_ORIGIN)
							legal_move = false;
					}
					if (legal_move){
						tetronimo->origin.x += 1;
					}
					break;
				// Upward movement will be in for now while I test things
				case M_UP:
					if (d_rot == D_0 || d_rot == D_180){
						if (y_min == FIELD_Y/2 || play_field[y_min-1][x_max] > T_ORIGIN)
							legal_move = false;
					} else {
						for (int i = 0; i < NUM_PIECES; i++){
							x = tetronimo->pieces[i].x;
							y = tetronimo->pieces[i].y;
							if (y == (FIELD_Y/2)-1 || play_field[y-1][x] > T_ORIGIN){
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
			set_Piece(tetronimo);
			break;
		}
		default:
			fprintf(stderr, "Error: Unknown T_Type\n");
			break;
	}

	set_Tetronimo(tetronimo);
	return is_falling;
}
