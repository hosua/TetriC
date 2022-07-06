#include "graphics.h"
#include "tetris.h"

// 10x40 (However only 10x20 is visible to the player)
uint8_t play_field[FIELD_Y][FIELD_X] = {0};

// The play field is numbers from lowest to greatest, top to bottom. So the bottom of the play field is not 0, it is 39.
int max_x(Tetronimo* tetronimo){
	int max = 0, x_pos = 0;
	for (int i = 0; i < TETRA; i++){
		x_pos = tetronimo->pieces[i].x;
		if (x_pos > max)
			max = x_pos;
	}
	printf("max_x: %i\n", max);
	return max;
}
int min_x(Tetronimo* tetronimo){
	int min = INT32_MAX, x_pos = 0;
	for (int i = 0; i < TETRA; i++){
		x_pos = tetronimo->pieces[i].x;
		if (x_pos < min)
			min = x_pos;
	}
	printf("min_x: %i\n", min);
	return min;
}
int max_y(Tetronimo* tetronimo){
	int max = 0, y_pos = 0;
	for (int i = 0; i < TETRA; i++){
		y_pos = tetronimo->pieces[i].y;
		if (y_pos > max)
			max = y_pos;
	}
	printf("max_y: %i\n", max);
	return max;
}
int min_y(Tetronimo* tetronimo){
	int min = INT32_MAX, y_pos = 0;
	for (int i = 0; i < TETRA; i++){
		y_pos = tetronimo->pieces[i].y;
		if (y_pos < min)
			min = y_pos;
	}
	printf("min_y: %i\n", min);
	return min;
}

const char* T_Type_to_str(T_Type t_type){
	switch(t_type){
		case T_NONE: return "T_NONE"; break;
		case T_O: return "T_O"; break;
		case T_I: return "T_I"; break;
		case T_S: return "T_S"; break;
		case T_Z: return "T_Z"; break;
		case T_L: return "T_L"; break;
		case T_J: return "T_J"; break;
		case T_T: return "T_T"; break;
		default: return "Unknown T_Type"; break;
	}
	return "Unknown T_Type";
}
const char* D_Rot_to_str(D_Rot d_rot){
	switch(d_rot){
		case D_0: return "D_0"; break;
		case D_90: return "D_90"; break;
		case D_180: return "D_180"; break;
		case D_270: return "D_270"; break;
		default: return "Unknown D_Rot"; break;
	}
	return "Unknown D_Rot";
}

// Generate a random piece via RNG
// For now we only have two pieces, but this will generate all random pieces once they are implemented
// enum order: T_NONE T_O T_I T_S T_Z T_L T_J T_T
Tetronimo* rand_Piece(){
	T_Type lower = T_O;
	T_Type upper = T_L;
	T_Type rand_t_type = rand() % (upper + 1 - lower ) + lower; 
	Tetronimo* tetronimo = new_Piece(rand_t_type);
	return tetronimo;
}

Tetronimo* new_Piece(T_Type t_type){
	Tetronimo* new_piece = (Tetronimo*)malloc(sizeof(Tetronimo));
	new_piece->t_type = t_type;
	new_piece->d_rot = D_90;
	new_piece->origin.x = SPAWN_X;
	new_piece->origin.y = SPAWN_Y;

	set_Piece(new_piece);
	set_Tetronimo(new_piece);
	return new_piece;
}


// Helper function, do not call set_Piece() directly.
// Sets piece with respect to its origin and degree of rotation (This does not set the actual play_field)
void set_Piece(Tetronimo* tetronimo){
	Coords origin = tetronimo->origin;
	T_Type t_type = tetronimo->t_type;
	D_Rot d_rot = tetronimo->d_rot;
	switch(t_type){
		// I-Piece
		case T_I:
			switch(d_rot){
				// Vertical
				case D_0: case D_180:
					for (int i = 0; i < TETRA; i++){
						tetronimo->pieces[i].x = origin.x - 1;
						tetronimo->pieces[i].y = origin.y + i;
					}
					break;
				// Horizontal
				case D_90: case D_270:
					for (int i = 0; i < TETRA; i++){
						tetronimo->pieces[i].x = origin.x + i - 3;
						tetronimo->pieces[i].y = origin.y + 2;
					}
					break;
				default:
					fprintf(stderr, "Error: Invalid degree of rotation\n");
					break;
			}	
			break;
		case T_O:
			// All degrees of rotation result in the same layout for O-Pieces
			// 2 0
			// 3 1
			tetronimo->pieces[0].x = origin.x - 1;
			tetronimo->pieces[0].y = origin.y + 1;
			tetronimo->pieces[1].x = origin.x - 1;
			tetronimo->pieces[1].y = origin.y + 2;

			tetronimo->pieces[2].x = origin.x - 2;
			tetronimo->pieces[2].y = origin.y + 1;
			tetronimo->pieces[3].x = origin.x - 2;
			tetronimo->pieces[3].y = origin.y + 2;
			break;
		case T_S:
			switch(d_rot){
				//   0
				//   1 2
				//     3
				case D_0: case D_180:
					tetronimo->pieces[0].x = origin.x - 1;
					tetronimo->pieces[0].y = origin.y;
					tetronimo->pieces[1].x = origin.x - 1;
					tetronimo->pieces[1].y = origin.y + 1;

					tetronimo->pieces[2].x = origin.x;
					tetronimo->pieces[2].y = origin.y + 1;
					tetronimo->pieces[3].x = origin.x;
					tetronimo->pieces[3].y = origin.y + 2;
				break;
				//
				//   1 0
				// 3 2 
				case D_90: case D_270:
					tetronimo->pieces[0].x = origin.x;
					tetronimo->pieces[0].y = origin.y + 1;
					tetronimo->pieces[1].x = origin.x - 1;
					tetronimo->pieces[1].y = origin.y + 1;

					tetronimo->pieces[2].x = origin.x - 1;
					tetronimo->pieces[2].y = origin.y + 2;
					tetronimo->pieces[3].x = origin.x - 2;
					tetronimo->pieces[3].y = origin.y + 2;
				break;
				default:
					fprintf(stderr, "Error: Invalid degree of rotation\n");
					break;
			}
			break;
		case T_Z:
			switch(d_rot){
				//     0
				//   2 1
				//   3 
				case D_0: case D_180:
					tetronimo->pieces[0].x = origin.x;
					tetronimo->pieces[0].y = origin.y;
					tetronimo->pieces[1].x = origin.x;
					tetronimo->pieces[1].y = origin.y + 1;

					tetronimo->pieces[2].x = origin.x - 1;
					tetronimo->pieces[2].y = origin.y + 1;
					tetronimo->pieces[3].x = origin.x - 1;
					tetronimo->pieces[3].y = origin.y + 2;
				break;
				//
				// 3 2 
				//   1 0
				case D_90: case D_270:
					tetronimo->pieces[0].x = origin.x;
					tetronimo->pieces[0].y = origin.y + 2;
					tetronimo->pieces[1].x = origin.x - 1;
					tetronimo->pieces[1].y = origin.y + 2;

					tetronimo->pieces[2].x = origin.x - 1;
					tetronimo->pieces[2].y = origin.y + 1;
					tetronimo->pieces[3].x = origin.x - 2;
					tetronimo->pieces[3].y = origin.y + 1;
				break;
				default:
					fprintf(stderr, "Error: Invalid degree of rotation\n");
					break;
			}
			break;
		case T_L:
			switch(d_rot){
				case D_0:
					//   0
					//   1
					// 3 2
					for (int i = 0; i < 3; i++){
						tetronimo->pieces[i].x = origin.x - 1;
						tetronimo->pieces[i].y = origin.y + i;
					}
					tetronimo->pieces[3].x = origin.x - 2;
					tetronimo->pieces[3].y = origin.y + 2;
					break;
				case D_90:
					// 3
					// 2 1 0
					// 
					for (int i = 0; i < 3; i++){
						tetronimo->pieces[i].x = origin.x - i;	
						tetronimo->pieces[i].y = origin.y + 1;
					}
					tetronimo->pieces[3].x = origin.x - 2;
					tetronimo->pieces[3].y = origin.y;
					break;
				case D_180:
					//   1 0
					//   2
					//   3
					tetronimo->pieces[0].x = origin.x;
					tetronimo->pieces[0].y = origin.y;
					for (int i = 1; i < TETRA; i++){
						tetronimo->pieces[i].x = origin.x - 1;	
						tetronimo->pieces[i].y = origin.y - 1 + i;	
					}
					break;
				case D_270:
					//
					// 0 1 2
					//     3
					for (int i = 0; i < 3; i++){
						tetronimo->pieces[i].x = origin.x - 2 + i;
						tetronimo->pieces[i].y = origin.y + 1;
					}
					tetronimo->pieces[3].x = origin.x;
					tetronimo->pieces[3].y = origin.y + 2;
					break;
				default:
					fprintf(stderr, "Error: Invalid degree of rotation\n");
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

	for (int i = 0; i < TETRA; i++){
		x = tetronimo->pieces[i].x;
		y = tetronimo->pieces[i].y;
		play_field[y][x] = t_type;
	}
}

// Returns the d_rot value of the tetronimo
D_Rot rotate_Tetronimo(Tetronimo* tetronimo, M_Direction move){
	D_Rot d_rot = tetronimo->d_rot;
	switch(move){
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

void print_Tetronimo_Coords(Tetronimo *tetronimo){
	Coords* coords = tetronimo->pieces;
	for (int i = 0; i < TETRA; i++)
		printf("(%i, %i) ", coords[i].x, coords[i].y);
	printf("\n");
}

// Returns an array of the indicies of the lines to clear. If a line's value is 0, then it is
// not a line to clear.
// Helper function for ClearLines()
uint8_t* GetLinesToClear(uint8_t *lines_cleared){
	// If lines == 0; then it is not a line we are clearing. 
	uint8_t *line_numbers= calloc(TETRA, sizeof(uint8_t));
	for (int y = (FIELD_Y/2)-1; y < FIELD_Y; y++){
		bool full_line = true;		
		for (int x = 0; x < FIELD_X; x++){
			if (!play_field[y][x]){
				full_line = false;
				break;
			}
		}
		if (full_line){
			line_numbers[(*lines_cleared)++] = y;
		}
	}
	if (*lines_cleared){
		printf("Lines to clear:\n");
		for (int i = 0; i < *lines_cleared; i++){
			printf("%u\n", line_numbers[i]);	
		}
	}
	return line_numbers;
}

// Shift all lines above y_min (Remember that y-axes is flipped)
// Helper function for ClearLines()
void ShiftLines(uint8_t y_min, uint8_t lines_cleared){
	for (int y = y_min; y > (FIELD_Y/2)-1; y--){
		for (int x = 0; x < FIELD_X; x++){
			play_field[y+lines_cleared][x] = play_field[y][x];
			play_field[y][x] = T_NONE;
		}
	}
}

// Clear lines when a full one (or more) is detected
void ClearLines(){
	uint8_t lines_cleared = 0;
	uint8_t* line_numbers = GetLinesToClear(&lines_cleared);

	for (int i = 0; i < lines_cleared; i++){
		int y = line_numbers[i];
		for (int x = 0; x < FIELD_X; x++){
			play_field[y][x] = T_NONE;	
		}
	}
	// First element in line_numbers will always be y_min
	ShiftLines(line_numbers[0], lines_cleared);

	free(line_numbers);
}

// Move tetronimo handles actually moving the pieces to their respective coordinates, as well as checking if a move 
// is legal or not.
// This function is fat, viewer discretion advised.
bool move_Tetronimo(SDL_Window* window, SDL_Renderer* renderer, Tetronimo* tetronimo, M_Direction move){
	// When the piece can no longer move down further, we will need to "lock it".
	// We do this by returning is_falling, and when it is false, the tetronimo is freed in main, and
	// then a new one is generated.
	bool is_falling = true;
	bool legal_move = true;
	T_Type t_type = tetronimo->t_type;
	D_Rot d_rot = tetronimo->d_rot;
	printf("d_rot: %s\n", D_Rot_to_str(d_rot));
	print_Tetronimo_Coords(tetronimo);
	int x = 0, y = 0;
	// Unset the pieces
	for (int i = 0; i < TETRA; i++){
		x = tetronimo->pieces[i].x;
		y = tetronimo->pieces[i].y;
		play_field[y][x] = T_NONE;
	}

	int y_max = max_y(tetronimo);
	int y_min = min_y(tetronimo);
	int x_max = max_x(tetronimo);
	int x_min = min_x(tetronimo);
	Coords origin = tetronimo->origin;
	printf("T_Type: %s\n", T_Type_to_str(t_type));
	// Perform the actual movement
	switch(t_type){
		// I-Piece
		case T_I:
		{
			switch(move){
				case M_ROT_RIGHT: // Fall through
				case M_ROT_LEFT:
					// TODO: Need to implement bounary checks before performing the rotation
					if (d_rot == D_0 || d_rot == D_180){
						// Check bounds
						if (x_min <= 1 || x_max >= FIELD_X-1 || y_max == FIELD_Y-1)
							legal_move = false;

						for (int i = 0; i < TETRA; i++){
							// Ignore the overlapping piece
							if (i != 2){
								x = origin.x + i - 3;
								y = origin.y + 2;
								if (play_field[y][x]){
									legal_move = false;
									break;
								}
							}
						}
					} else { // D_90, D_270
						for (int i = 0; i < TETRA; i++){
							// Ignore the overlapping piece
							if (i != 2){
								x = origin.x - 1;
								y = origin.y + i;
								if (play_field[y][x]){
									legal_move = false;
									break;
								}
							}
						}

					}
					// We are technically rotating it right for both left and right, but it doesn't matter for the I-Piece.
					if (legal_move)
						tetronimo->d_rot = rotate_Tetronimo(tetronimo, move);
					break;
				case M_DOWN:
					if (d_rot == D_0 || d_rot == D_180){
						if (y_max == FIELD_Y-1 || play_field[y_max+1][x_max])
							legal_move = false;
					} else {
						for (int i = 0; i < TETRA; i++){
							x = tetronimo->pieces[i].x;
							y = tetronimo->pieces[i].y;
							if (y == FIELD_Y-1 || play_field[y+1][x]){
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
						// Unset the move flag if it is illegal and break
						for (int i = 0; i < TETRA; i++){
							x = tetronimo->pieces[i].x;
							y = tetronimo->pieces[i].y;
							if (x == 0 || play_field[y][x-1]){
								legal_move = false;
								break;
							}
						}
						// Piece is horizontal, which means we only need to check one unit to the left of the left-most piece.
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
						// Unset the move flag if it is illegal and break
						for (int i = 0; i < TETRA; i++){
							x = tetronimo->pieces[i].x;
							y = tetronimo->pieces[i].y;
							if (x == FIELD_X-1 || play_field[y][x+1]){
								legal_move = false;
								break;
							}
						}
						// Piece is horizontal, which means we only need to check one unit to the right of the right-most piece.
					} else { 						
						if (x_max == FIELD_X-1 || play_field[y_max][x_max+1])
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
						for (int i = 0; i < TETRA; i++){
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
		}
		break;
		// O-Piece
		case T_O:
		{
			switch(move){
				case M_ROT_RIGHT: // Fall through
				case M_ROT_LEFT:
					// All rotations for O-Piece are legal because they don't do anything
					tetronimo->d_rot = rotate_Tetronimo(tetronimo, move);
					break;
				case M_DOWN:
					// O-Piece:
					// 2 0
					// 3 1
					for (int i = 0; i < TETRA; i++){
						if (i % 1 == 0 || i % 3 == 0){
							x = tetronimo->pieces[i].x;
							y = tetronimo->pieces[i].y;
							if (play_field[y+1][x] || y == FIELD_Y-1){
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
					// Unset the move flag if it is illegal and break
					for (int i = 0; i < TETRA; i++){
						// Check only our 2 left pieces
						if (i % 2 == 0 || i % 3 == 0){
							x = tetronimo->pieces[i].x;
							y = tetronimo->pieces[i].y;
							if (x == 0 || play_field[y][x-1]){
								legal_move = false;
								break;
							}
						}
					}

					if (legal_move)
						tetronimo->origin.x -= 1;
					break;
				case M_RIGHT:
					// Unset the move flag if it is illegal and break
					for (int i = 0; i < TETRA; i++){
						// Check only our 2 left pieces
						if (i == 0 || i % 1 == 0){
							x = tetronimo->pieces[i].x;
							y = tetronimo->pieces[i].y;
							if (x == FIELD_X-1 || play_field[y][x+1]){
								legal_move = false;
								break;
							}
						}
					}
					if (legal_move)
						tetronimo->origin.x += 1;

					break;
					// Upward movement will be in for now while I test things
				case M_UP:
					if (d_rot == D_0 || d_rot == D_180){
						if (y_min == FIELD_Y/2 || play_field[y_min-1][x_max])
							legal_move = false;
					} else {
						for (int i = 0; i < TETRA; i++){
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
		}
		break;
		// S-Piece
		case T_S:
		{
			switch(move){
				case M_ROT_RIGHT: // Fall through
				case M_ROT_LEFT:
					x = origin.x;
					y = origin.y;
					switch(d_rot){
						//   0
						//   1 2
						//     3
						case D_0: case D_180:
							if (x_min == 0){
								legal_move = false;
								break;
							}
							if (play_field[y+2][x-2] || 
									play_field[y+2][x-1]){
								legal_move = false;
							}
							break;
						//
						//   1 0
						// 3 2 
						case D_90: case D_270:
							if (play_field[y][x-1] || 
									play_field[y+2][x]){
								legal_move = false;
							}
							break;
						default:
							fprintf(stderr, "Error: Unknown rotation\n");
							break;
					}
					if (legal_move)
						tetronimo->d_rot = rotate_Tetronimo(tetronimo, move);
					break;
				case M_DOWN:
					x = origin.x;
					y = origin.y;
					if (y_max == FIELD_Y-1){
						is_falling = false;
						break;
					}
					switch(d_rot){
						//   0
						//   1 2
						//     3
						case D_0: case D_180:
							if (play_field[y+2][x-1] ||
									play_field[y+3][x]){
								legal_move = false;
							}
							break;
						//
						//   1 0
						// 3 2 
						case D_90: case D_270:
							if (play_field[y+3][x-2] ||
									play_field[y+3][x-1] ||
									play_field[y+2][x]){
								legal_move = false;
							}
							break;
						default:
							fprintf(stderr, "Error: Unknown rotation\n");
							break;
					}
					if (legal_move){
						tetronimo->origin.y += 1;
					} else {
						// Only downward movement should determine if the piece will lock or not.
						is_falling = false;
					}
					break;
				case M_LEFT:
					x = origin.x;
					y = origin.y;
					if (x_min == 0){
						legal_move = false;
					}
					switch(d_rot){
						//   0
						//   1 2
						//     3
						case D_0: case D_180:
							if (play_field[y][x-2] ||
									play_field[y+1][x-2] ||
									play_field[y+2][x-1]){
								legal_move = false;
							}
							break;
						//
						//   1 0
						// 3 2 
						case D_90: case D_270:
							if (play_field[y+1][x-2] ||
									play_field[y+2][x-3]){
								legal_move = false;
							}
							break;
						default:
							fprintf(stderr, "Error: Unknown rotation\n");
							break;
					}
					if (legal_move)
						tetronimo->origin.x -= 1;
					break;
				case M_RIGHT:
					x = origin.x;
					y = origin.y;
					if (x_max == FIELD_X-1){
						legal_move = false;
					}
					switch(d_rot){
						//   0
						//   1 2
						//     3
						case D_0: case D_180:
							if (play_field[y][x] ||
									play_field[y+1][x+1] ||
									play_field[y+2][x+1]){
								legal_move = false;
							}
							break;
						//
						//   1 0
						// 3 2 
						case D_90: case D_270:
							if (play_field[y+1][x+1] ||
									play_field[y+2][x]){
								legal_move = false;
							}
							break;
						default:
							fprintf(stderr, "Error: Unknown rotation\n");
							break;
					}
					if (legal_move)
						tetronimo->origin.x += 1;

					break;
					// Upward movement will be in for now while I test things
				case M_UP:
					x = origin.x;
					y = origin.y;
					if (y_min == (FIELD_Y/2)-1){
						legal_move = false;
					}
					switch(d_rot){
						//   0
						//   1 2
						//     3
						case D_0: case D_180:
							if (play_field[y-1][x-1] ||
									play_field[y][x]){
								legal_move = false;
							}
							break;
						//
						//   1 0
						// 3 2 
						case D_90: case D_270:
							if (play_field[y][x] ||
									play_field[y][x-1] ||
									play_field[y+1][x-1]){
								legal_move = false;
							}
							break;
						default:
							fprintf(stderr, "Error: Unknown rotation\n");
							break;
					}
					if (legal_move){
						tetronimo->origin.y -= 1;
					}
					break;
				default:
					fprintf(stderr, "Error: Unknown move\n");
					break;
			}

		}
		break;
		// Z-Piece
		case T_Z:
		{
			switch(move){
				case M_ROT_RIGHT: // Fall through
				case M_ROT_LEFT:
					x = origin.x;
					y = origin.y;
					switch(d_rot){
						//     0
						//   2 1
						//   3
						case D_0: case D_180:
							if (x_min == 0){
								legal_move = false;
							}
							if (play_field[y+1][x-2] ||
									play_field[y+2][x]){
								legal_move = false;
							}

							break;
						//
						// 3 2 
						//   1 0
						case D_90: case D_270:
							if (play_field[y][x] ||
									play_field[y+1][x]){
								legal_move = false;
							}
							break;
						default:
							fprintf(stderr, "Error: Unknown rotation\n");
							break;
					}
					if (legal_move)
						tetronimo->d_rot = rotate_Tetronimo(tetronimo, move);
					break;
				case M_DOWN:
					x = origin.x;
					y = origin.y;
					if (y_max == FIELD_Y-1){
						is_falling = false;
						break;
					}
					switch(d_rot){
						//     0
						//   2 1
						//   3
						case D_0: case D_180:
							if (play_field[y+2][x] ||
									play_field[y+3][x-1]){
								legal_move = false;
							}
							break;
						//
						// 3 2 
						//   1 0
						case D_90: case D_270:
							if (play_field[y+2][x-2] ||
									play_field[y+3][x-1] ||
									play_field[y+3][x]){
								legal_move = false;
							}
							break;
						default:
							fprintf(stderr, "Error: Unknown rotation\n");
							break;
					}
					if (legal_move){
						tetronimo->origin.y += 1;
					} else {
						// Only downward movement should determine if the piece will lock or not.
						is_falling = false;
					}
					break;
				case M_LEFT:
					x = origin.x;
					y = origin.y;
					if (x_min == 0){
						legal_move = false;
						break;
					}
					switch(d_rot){
						//     0
						//   2 1
						//   3
						case D_0: case D_180:
							if (play_field[y][x-1] ||
									play_field[y+1][x-2] ||
									play_field[y+2][x-2]){
								legal_move = false;
							}
							break;
						//
						// 3 2 
						//   1 0
						case D_90: case D_270:
							if (play_field[y+1][x-3] ||
									play_field[y+2][x-2]){
								legal_move = false;
							}
							break;
						default:
							fprintf(stderr, "Error: Unknown rotation\n");
							break;
					}
					if (legal_move)
						tetronimo->origin.x -= 1;
					break;
				case M_RIGHT:
					x = origin.x;
					y = origin.y;
					if (x_max == FIELD_X-1){
						legal_move = false;
						break;
					}
					switch(d_rot){
						//     0
						//   2 1
						//   3
						case D_0: case D_180:
							if (play_field[y][x+1] ||
									play_field[y+1][x+1] ||
									play_field[y+2][x]){
								legal_move = false;
							}
							break;
						//
						// 3 2 
						//   1 0
						case D_90: case D_270:
							if (play_field[y+1][x] ||
									play_field[y+2][x+1]){
								legal_move = false;
							}
							break;
						default:
							fprintf(stderr, "Error: Unknown rotation\n");
							break;
					}
					if (legal_move)
						tetronimo->origin.x += 1;

					break;
					// Upward movement will be in for now while I test things
				case M_UP:
					x = origin.x;
					y = origin.y;
					if (y_min == (FIELD_Y/2)-1){
						legal_move = false;
						break;
					}
					switch(d_rot){
						//     0
						//   2 1
						//   3
						case D_0: case D_180:
							if (play_field[y-1][x] ||
									play_field[y][x-1]){
								legal_move = false;
							}
							break;
						//
						// 3 2 
						//   1 0
						case D_90: case D_270:
							if (play_field[y+1][x] ||
									play_field[y][x-1] ||
									play_field[y][x-2]){
								legal_move = false;
							}
							break;
						default:
							fprintf(stderr, "Error: Unknown rotation\n");
							break;
					}
					if (legal_move){
						tetronimo->origin.y -= 1;
					}
					break;
				default:
					fprintf(stderr, "Error: Unknown move\n");
					break;
			}
		}
		break;
		// L-Piece
		case T_L:
		{
			switch(move){
				case M_ROT_LEFT: 
					if ((d_rot == D_180 && x_min == 0) || 
							(d_rot == D_0 && x_max == FIELD_X-1)){
						legal_move = false;
					}

					x = origin.x;
					y = origin.y;
					switch(d_rot){
						case D_0:
							//   0
							//   1
							// 3 2
							if (play_field[y+1][x] ||
									play_field[y+2][x] ||
									play_field[y+1][x-2]){
								legal_move = false;
							}
							break;
						case D_90:
							// 3
							// 2 1 0
							// 
							if (play_field[y][x-1] ||
									play_field[y+2][x-2] ||
									play_field[y+2][x-1]){
								legal_move = false;
							}
							break;
						case D_180:
							//   1 0
							//   2
							//   3
							if (play_field[y][x-2] ||
									play_field[y+1][x-2] ||
									play_field[y+1][x]){
								legal_move = false;
							}
							break;
						case D_270:
							//
							// 0 1 2
							//     3
							if (play_field[y][x] ||
									play_field[y][x-1] ||
									play_field[y+2][x-1]){
								legal_move = false;
							}
							break;

					}
					if (legal_move)
						tetronimo->d_rot = rotate_Tetronimo(tetronimo, move);
					break;
				case M_ROT_RIGHT:
					if ((d_rot == D_180 && x_min == 0) || 
							(d_rot == D_0 && x_max == FIELD_X-1)){
						legal_move = false;
					}
					x = origin.x;
					y = origin.y;
					switch(d_rot){
						case D_0:
							//   0
							//   1
							// 3 2
							if (play_field[y][x-2] ||
									play_field[y+1][x-2] ||
									play_field[y+1][x]){
								legal_move = false;
							}
							break;
						case D_90:
							// 3
							// 2 1 0
							// 
							if (play_field[y][x] ||
									play_field[y][x-1] ||
									play_field[y+2][x-1]){
								legal_move = false;
							}
							break;
						case D_180:
							//   1 0
							//   2
							//   3
							if (play_field[y+1][x-2] ||
									play_field[y+1][x] ||
									play_field[y+2][x]){
								legal_move = false;
							}
							break;
						case D_270:
							//
							// 0 1 2
							//     3
							if (play_field[y][x-1] ||
									play_field[y+2][x-1] ||
									play_field[y+2][x-2]){
								legal_move = false;
							}
							break;
					}
					if (legal_move)
						tetronimo->d_rot = rotate_Tetronimo(tetronimo, move);
					break;
				case M_DOWN:
					if (y_max == FIELD_Y-1){
						is_falling = false;
						break;
					}
					switch(d_rot){
						case D_0:
							//   0
							//   1
							// 3 2
							for (int i = 2; i <= 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (play_field[y+1][x])
									legal_move = false;
							}
							break;
						case D_90:
							// 3
							// 2 1 0
							// 
							for (int i = 0; i <= 2; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (play_field[y+1][x])
									legal_move = false;
							}
							break;
						case D_180:
							//   1 0
							//   2
							//   3
							x = origin.x;
							y = origin.y;
							if (play_field[y+3][x-1] ||
									play_field[y+1][x]){
								legal_move = false;
							}
							break;
						case D_270:
							//
							// 0 1 2
							//     3
							for (int i = 0; i < TETRA; i++){
								if (i != 2){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (play_field[y+1][x])
										legal_move = false;
								}
							}	
							break;
					}

					if (legal_move){
						tetronimo->origin.y += 1;
					} else {
						// Only downward movement should determine if the piece will lock or not.
						is_falling = false;
					}
					break;
				case M_LEFT:
					if (x_min == 0){
						legal_move = false;
						break;
					}
					switch(d_rot){
						case D_0:
							//   0
							//   1
							// 3 2
							for (int i = 0; i < TETRA; i++){
								if (i != 2){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (play_field[y][x-1]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_90:
							// 3
							// 2 1 0
							// 
							for (int i = 2; i <= 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (play_field[y][x-1]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_180:
							//   1 0
							//   2
							//   3
							for (int i = 1; i < TETRA; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (play_field[y][x-1]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_270:
							//
							// 0 1 2
							//     3
							for (int i = 0; i < TETRA; i++){
								if (i != 1 || i != 2){	
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (play_field[y][x-1]){
										legal_move = false;
										break;
									}
								}
							}
							break;
					}
					if (legal_move)
						tetronimo->origin.x -= 1;
					break;
				case M_RIGHT:
					if (x_max == FIELD_X-1){
						legal_move = false;
						break;
					}
					switch(d_rot){
						case D_0:
							//   0
							//   1
							// 3 2
							for (int i = 0; i < 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (play_field[y][x+1]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_90:
							// 3
							// 2 1 0
							// 
							for (int i = 0; i < TETRA; i++){
								if (i != 1 || i != 2){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (play_field[y][x+1]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_180:
							//   1 0
							//   2
							//   3
							for (int i = 0; i < TETRA; i++){
								if (i != 1){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (play_field[y][x+1]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_270:
							//
							// 0 1 2
							//     3
							for (int i = 2; i < TETRA; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (play_field[y][x+1]){
									legal_move = false;
									break;
								}
							
							}
							break;
					}
					if (legal_move)
						tetronimo->origin.x += 1;

					break;
					// Upward movement will be in for now while I test things
				case M_UP:
					switch(d_rot){
						case D_0:
							//   0
							//   1
							// 3 2
							for (int i = 0; i < 3; i++){
								if (i != 1 || i != 2){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (play_field[y-1][x]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_90:
							// 3
							// 2 1 0
							// 
							for (int i = 0; i < TETRA; i++){
								if (i != 2){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (play_field[y-1][x]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_180:
							//   1 0
							//   2
							//   3
							for (int i = 0; i <= 1; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (play_field[y-1][x]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_270:
							//
							// 0 1 2
							//     3
							for (int i = 0; i <= 2; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (play_field[y-1][x]){
									legal_move = false;
									break;
								}
							
							}
							break;
					}
					if (legal_move){
						tetronimo->origin.y -= 1;
					}
					break;
				default:
					fprintf(stderr, "Error: Unknown move\n");
					break;
			}
		}
		break;
		default:
			fprintf(stderr, "Error: Unknown T_Type in move_Tetronimo()\n");
			break;
	}
	// Set piece coords in tetronimo struct
	set_Piece(tetronimo);
	// Set tetronimo to play_field
	set_Tetronimo(tetronimo); 
	return is_falling;
}
