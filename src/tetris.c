#include "tetris.h"

uint32_t _lines_cleared = 0;
uint32_t _player_score = 0;
uint8_t _curr_level = 0;
uint8_t _lines_until_level = 0;
float _fps = 0.0f;

uint16_t _tetronimo_counter[NUM_TETRONIMOS+1] = {0};

// 10x40 (However only 10x20 is visible to the player)
T_Type _play_field[FIELD_Y][FIELD_X] = {T_NONE};

void QuitGame(SDL_Window* window, SDL_Renderer* renderer){
	printf("Game over!\n"
		   "You cleared %i lines before losing.\n"
		   "Your final score was: %i\n", _lines_cleared, _player_score);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_CloseAudio();
	SDL_Quit();
}

// The play field is numbers from lowest to greatest, top to bottom. So the bottom of the play field is not 0, it is 39.
int max_x(Tetronimo* tetronimo){
	int max = 0, x_pos = 0;
	for (int i = 0; i < TETRA; i++){
		x_pos = tetronimo->pieces[i].x;
		if (x_pos > max)
			max = x_pos;
	}
	if (VERBOSE) printf("max_x: %i\n", max);
	return max;
}
int min_x(Tetronimo* tetronimo){
	int min = INT32_MAX, x_pos = 0;
	for (int i = 0; i < TETRA; i++){
		x_pos = tetronimo->pieces[i].x;
		if (x_pos < min)
			min = x_pos;
	}
	if (VERBOSE) printf("min_x: %i\n", min);
	return min;
}
int max_y(Tetronimo* tetronimo){
	int max = 0, y_pos = 0;
	for (int i = 0; i < TETRA; i++){
		y_pos = tetronimo->pieces[i].y;
		if (y_pos > max)
			max = y_pos;
	}
	if (VERBOSE) printf("max_y: %i\n", max);
	return max;
}
int min_y(Tetronimo* tetronimo){
	int min = INT32_MAX, y_pos = 0;
	for (int i = 0; i < TETRA; i++){
		y_pos = tetronimo->pieces[i].y;
		if (y_pos < min)
			min = y_pos;
	}
	if (VERBOSE) printf("min_y: %i\n", min);
	return min;
}

const char* T_Type_to_str(T_Type t_type){
	switch(t_type){
		case T_NONE: return "T_NONE"; break;
		case T_O: return "O"; break;
		case T_I: return "I"; break;
		case T_S: return "S"; break;
		case T_Z: return "Z"; break;
		case T_L: return "L"; break;
		case T_J: return "J"; break;
		case T_T: return "T"; break;
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
T_Type rand_T_Type(){
	T_Type lower = T_O;
	T_Type upper = T_T;
	T_Type t_type = rand() % (upper + 1 - lower ) + lower; 
	return t_type;
}

Tetronimo* new_Tetronimo(T_Type t_type){
	Tetronimo* tetronimo = (Tetronimo*)malloc(sizeof(Tetronimo));
	tetronimo->t_type = t_type;
	tetronimo->d_rot = D_90;
	tetronimo->origin.x = SPAWN_X;
	tetronimo->origin.y = SPAWN_Y;
	set_Tetronimo(tetronimo);
	set_To_Field(tetronimo);
	return tetronimo;
}

// Helper function for set_Tetronimo to set tetronimo onto the _play_field;
void set_To_Field(Tetronimo* tetronimo){
	T_Type t_type = tetronimo->t_type;
	int x = 0, y = 0;

	for (int i = 0; i < TETRA; i++){
		x = tetronimo->pieces[i].x;
		y = tetronimo->pieces[i].y;
		_play_field[y][x] = t_type;
	}
}

void print_Tetronimo_Coords(Tetronimo *tetronimo){
	Coords* coords = tetronimo->pieces;
	for (int i = 0; i < TETRA; i++)
		printf("(%i, %i) ", coords[i].x, coords[i].y);
	printf("\n");
}

// Helper function for GetLinesToClear()
bool LineIsFull(uint8_t y){
	bool is_full = true;
	for (int x = 0; x < FIELD_X; x++){
		if(!_play_field[y][x]){
			is_full = false;
			break;
		}
	}
	return is_full;
}
// Returns an array of the indicies of the lines to clear. If a line's value is 0, then it is
// not a line to clear.
// Helper function for CheckLines()
bool* GetLinesToClear(){
	// If lines[i] == 1; then it is a line we are clearing. 
	bool *line_numbers = calloc(FIELD_Y, sizeof(bool));

	for (int y = 0; y < FIELD_Y; y++)
		line_numbers[y] = LineIsFull(y);

	return line_numbers;
}

// Clears and shifts all lines above the cleared line down once
// Helper function for CheckLines()
void ClearLine(uint8_t y_min){
	// Increment global lines cleared counter
	_lines_cleared++;
	if (y_min >= FIELD_Y || y_min <= 0){
		fprintf(stderr, "Error: ClearLine exceeded _play_field boundaries\n");
		return;
	}
	for (int y = y_min; y >= FIELD_Y/2; y--){
		for (int x = 0; x < FIELD_X; x++){
			_play_field[y][x] = _play_field[y-1][x];
			_play_field[y-1][x] = T_NONE;
		}
	}
}
// Clears and shifts lines whenever a full one is detected
uint8_t CheckLines(){
	uint8_t lines_cleared = 0;
	bool* line_numbers = GetLinesToClear();
	for (int y = FIELD_Y-1; y >= FIELD_Y/2; y--){
		if (line_numbers[y]){
			ClearLine(y);
			lines_cleared++;
			y++;
			line_numbers = GetLinesToClear();
		}
	}
	free(line_numbers);
	return lines_cleared;
}

bool IsPlayerDead(){
	for (int x = 0; x < FIELD_X; x++)
		if (_play_field[(FIELD_Y/2)-1][x])
			return true;
	return false;
}

void GetLinesUntilNextLevel(uint8_t level){
	printf("Level %i\n", level);
	switch(level){
		case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
			_lines_until_level = (level + 1) * 10;
			break;
		case 10: case 11: case 12: case 13: case 14: case 15:
			_lines_until_level = 100;
			break;
		case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25:
			_lines_until_level = (level - 5) * 10;
			break;
		default: 
			_lines_until_level = 200;
			break;
	}
	printf("Lines until next level: %i\n", _lines_until_level);
}

// Calculates and returns the score accrued from the lines cleared in a turn 
// (This function does not include points that are added from soft dropping a piece)
// Reference: https://tetris.fandom.com/wiki/Scoring
uint16_t CalcScore(uint8_t lines_cleared, uint8_t level){
	uint16_t calculated_score = 0;
	switch(lines_cleared){
		case 0:
			calculated_score = 0;
			break;
		case 1:
			calculated_score = 40 * (level + 1);
			break;
		case 2:
			calculated_score = 100 * (level + 1);
			break;
		case 3:
			calculated_score = 300 * (level + 1);
			break;
		case 4:
			calculated_score = 1200 * (level + 1);
			break;
		default:
			fprintf(stderr, "Error: More than 4 lines were deleted in a single turn, this shouldn't be possible\n");
			calculated_score = 0;
			break;
	}
	return calculated_score;
}

// Sets Tetronimo with respect to its origin and degree of rotation, then calls set_To_Field()
void set_Tetronimo(Tetronimo* tetronimo){
	Coords origin = tetronimo->origin;
	T_Type t_type = tetronimo->t_type;
	D_Rot d_rot = tetronimo->d_rot;
	switch(t_type){
		// I-Piece
		case T_I:
			switch(d_rot){
				//     0
				//     1
				//     2
				//     3
				case D_0: case D_180:
					for (int i = 0; i < TETRA; i++){
						tetronimo->pieces[i].x = origin.x - 1;
						tetronimo->pieces[i].y = origin.y + i;
					}
					break;
				//   
				//
				// 0 1 2 3
				//
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
					// 3 0
					//   1
					//   2
					for (int i = 0; i <= 2; i++){
						tetronimo->pieces[i].x = origin.x - 1;
						tetronimo->pieces[i].y = origin.y + i;
					}
					tetronimo->pieces[3].x = origin.x - 2;
					tetronimo->pieces[3].y = origin.y;
					break;
				case D_90:
					//     3
					// 2 1 0
					// 
					for (int i = 0; i <= 2; i++){
						tetronimo->pieces[i].x = origin.x - i;	
						tetronimo->pieces[i].y = origin.y + 1;
					}
					tetronimo->pieces[3].x = origin.x;
					tetronimo->pieces[3].y = origin.y;
					break;
				case D_180:
					//   1 
					//   2
					//   3 0
					tetronimo->pieces[0].x = origin.x;
					tetronimo->pieces[0].y = origin.y + 2;
					for (int i = 1; i < TETRA; i++){
						tetronimo->pieces[i].x = origin.x - 1;	
						tetronimo->pieces[i].y = origin.y - 1 + i;	
					}
					break;
				case D_270:
					//
					// 0 1 2
					// 3    
					for (int i = 0; i < 3; i++){
						tetronimo->pieces[i].x = origin.x - 2 + i;
						tetronimo->pieces[i].y = origin.y + 1;
					}
					tetronimo->pieces[3].x = origin.x - 2;
					tetronimo->pieces[3].y = origin.y + 2;
					break;
				default:
					fprintf(stderr, "Error: Invalid degree of rotation\n");
					break;
			}
		break;
		case T_J:
			switch(d_rot){
				case D_0:
					//   0
					//   1
					// 3 2
					for (int i = 0; i <= 2; i++){
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
					for (int i = 0; i <= 2; i++){
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
					for (int i = 0; i <= 2; i++){
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
		case T_T:
			switch(d_rot){
				case D_0:
					//   0
					// 3 1
					//   2
					for (int i = 0; i <= 2; i++){
						tetronimo->pieces[i].x = origin.x - 1;
						tetronimo->pieces[i].y = origin.y + i;
					}
					tetronimo->pieces[3].x = origin.x - 2;
					tetronimo->pieces[3].y = origin.y + 1;
					break;
				case D_90:
					//   3
					// 0 1 2
					// 
					for (int i = 0; i <= 2; i++){
						tetronimo->pieces[i].x = origin.x + i - 2;
						tetronimo->pieces[i].y = origin.y + 1;
					}
					tetronimo->pieces[3].x = origin.x - 1;
					tetronimo->pieces[3].y = origin.y;
					break;
				case D_180:
					//   0 
					//   1 3
					//   2
					for (int i = 0; i <= 2; i++){
						tetronimo->pieces[i].x = origin.x - 1;
						tetronimo->pieces[i].y = origin.y + i;
					}
					tetronimo->pieces[3].x = origin.x;
					tetronimo->pieces[3].y = origin.y + 1;
					break;
				case D_270:
					//
					// 0 1 2
					//   3
					for (int i = 0; i <= 2; i++){
						tetronimo->pieces[i].x = origin.x + i - 2;
						tetronimo->pieces[i].y = origin.y + 1;
					}
					tetronimo->pieces[3].x = origin.x - 1;
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
	if (VERBOSE){
		printf("T_Type: %s\n", T_Type_to_str(t_type));
		printf("D_Rot: %s\n", D_Rot_to_str(d_rot));
		print_Tetronimo_Coords(tetronimo);
	}

	int x = 0, y = 0;
	// Unset the pieces
	for (int i = 0; i < TETRA; i++){
		x = tetronimo->pieces[i].x;
		y = tetronimo->pieces[i].y;
		_play_field[y][x] = T_NONE;
	}

	int y_max = max_y(tetronimo);
	int y_min = min_y(tetronimo);
	int x_max = max_x(tetronimo);
	int x_min = min_x(tetronimo);
	Coords origin = tetronimo->origin;
	// Perform the actual movement
	switch(t_type){
		// I-Piece
		case T_I:
		{
			switch(move){
				case M_ROT_RIGHT: case M_ROT_LEFT:
					switch(d_rot){
						//     0
						//     1
						//     2
						//     3
						case D_0: case D_180:
							// Check bounds
							if (x_min <= 1 || x_max >= FIELD_X-1 || y_max == FIELD_Y-1)
								legal_move = false;

							for (int i = 0; i < TETRA; i++){
								// Ignore the overlapping piece
								if (i != 2){
									x = origin.x + i - 3;
									y = origin.y + 2;
									if (_play_field[y][x]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						//
						//
						// 0 1 2 3
						//
						case D_90: case D_270:
							for (int i = 0; i < TETRA; i++){
								if (i != 2){
									x = origin.x - 1;
									y = origin.y + i;
									if (_play_field[y][x]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						default:
							fprintf(stderr, "Error: Unknown rotation\n");
							break;
					}
					
					// We are technically rotating it right for both left and right, but it doesn't matter for the I-Piece.
					if (legal_move)
						tetronimo->d_rot = rotate_Tetronimo(tetronimo, move);
					break;
				case M_DOWN:
					switch(d_rot){
						//     0
						//     1
						//     2
						//     3
						case D_0: case D_180:
							if (y_max == FIELD_Y-1 || _play_field[y_max+1][x_max])
								legal_move = false;
							break;
						//
						//
						// 0 1 2 3
						//
						case D_90: case D_270:
							for (int i = 0; i < TETRA; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (y == FIELD_Y-1 || _play_field[y+1][x]){
									legal_move = false;
									break;
								}
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
					switch(d_rot){
						//     0
						//     1
						//     2
						//     3
						case D_0: case D_180:
							for (int i = 0; i < TETRA; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (x == 0 || _play_field[y][x-1]){
									legal_move = false;
									break;
								}
							}
							break;
						//
						//
						// 0 1 2 3
						//
						case D_90: case D_270:
							if (x_min == 0 || _play_field[y_max][x_min-1])
								legal_move = false;
							break;
						default:
							fprintf(stderr, "Error: Unknown rotation\n");
							break;
					}
					if (legal_move){
						tetronimo->origin.x -= 1;
					}
					break;
				case M_RIGHT:
					switch(d_rot){
						//     0
						//     1
						//     2
						//     3
						case D_0: case D_180:
							// Unset the move flag if it is illegal and break
							for (int i = 0; i < TETRA; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (x == FIELD_X-1 || _play_field[y][x+1]){
									legal_move = false;
									break;
								}
							}
							break;
						//
						//
						// 0 1 2 3
						//
						case D_90: case D_270:
							if (x_max == FIELD_X-1 || _play_field[y_max][x_max+1])
								legal_move = false;
							break;
						default:
							fprintf(stderr, "Error: Unknown rotation\n");
							break;
					}
					if (legal_move){
						tetronimo->origin.x += 1;
					}
					break;
				case M_UP:
					switch(d_rot){
						//     0
						//     1
						//     2
						//     3
						case D_0: case D_180:
							if (y_min == FIELD_Y/2 || _play_field[y_min-1][x_max])
								legal_move = false;
							break;
						//
						//
						// 0 1 2 3
						//
						case D_90: case D_270:
							for (int i = 0; i < TETRA; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (y == (FIELD_Y/2)-1 || _play_field[y-1][x]){
									legal_move = false;
									break;
								}
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
							if (_play_field[y+1][x] || y == FIELD_Y-1){
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
							if (x == 0 || _play_field[y][x-1]){
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
							if (x == FIELD_X-1 || _play_field[y][x+1]){
								legal_move = false;
								break;
							}
						}
					}
					if (legal_move)
						tetronimo->origin.x += 1;

					break;
				case M_UP:
					if (d_rot == D_0 || d_rot == D_180){
						if (y_min == FIELD_Y/2 || _play_field[y_min-1][x_max])
							legal_move = false;
					} else {
						for (int i = 0; i < TETRA; i++){
							x = tetronimo->pieces[i].x;
							y = tetronimo->pieces[i].y;
							if (y == (FIELD_Y/2)-1 || _play_field[y-1][x]){
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
							if (_play_field[y+2][x-2] || 
									_play_field[y+2][x-1]){
								legal_move = false;
							}
							break;
						//
						//   1 0
						// 3 2 
						case D_90: case D_270:
							if (_play_field[y][x-1] || 
									_play_field[y+2][x]){
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
							if (_play_field[y+2][x-1] ||
									_play_field[y+3][x]){
								legal_move = false;
							}
							break;
						//
						//   1 0
						// 3 2 
						case D_90: case D_270:
							if (_play_field[y+3][x-2] ||
									_play_field[y+3][x-1] ||
									_play_field[y+2][x]){
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
							if (_play_field[y][x-2] ||
									_play_field[y+1][x-2] ||
									_play_field[y+2][x-1]){
								legal_move = false;
							}
							break;
						//
						//   1 0
						// 3 2 
						case D_90: case D_270:
							if (_play_field[y+1][x-2] ||
									_play_field[y+2][x-3]){
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
							if (_play_field[y][x] ||
									_play_field[y+1][x+1] ||
									_play_field[y+2][x+1]){
								legal_move = false;
							}
							break;
						//
						//   1 0
						// 3 2 
						case D_90: case D_270:
							if (_play_field[y+1][x+1] ||
									_play_field[y+2][x]){
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
							if (_play_field[y-1][x-1] ||
									_play_field[y][x]){
								legal_move = false;
							}
							break;
						//
						//   1 0
						// 3 2 
						case D_90: case D_270:
							if (_play_field[y][x] ||
									_play_field[y][x-1] ||
									_play_field[y+1][x-1]){
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
							if (_play_field[y+1][x-2] ||
									_play_field[y+2][x]){
								legal_move = false;
							}

							break;
						//
						// 3 2 
						//   1 0
						case D_90: case D_270:
							if (_play_field[y][x] ||
									_play_field[y+1][x]){
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
							if (_play_field[y+2][x] ||
									_play_field[y+3][x-1]){
								legal_move = false;
							}
							break;
						//
						// 3 2 
						//   1 0
						case D_90: case D_270:
							if (_play_field[y+2][x-2] ||
									_play_field[y+3][x-1] ||
									_play_field[y+3][x]){
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
							if (_play_field[y][x-1] ||
									_play_field[y+1][x-2] ||
									_play_field[y+2][x-2]){
								legal_move = false;
							}
							break;
						//
						// 3 2 
						//   1 0
						case D_90: case D_270:
							if (_play_field[y+1][x-3] ||
									_play_field[y+2][x-2]){
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
							if (_play_field[y][x+1] ||
									_play_field[y+1][x+1] ||
									_play_field[y+2][x]){
								legal_move = false;
							}
							break;
						//
						// 3 2 
						//   1 0
						case D_90: case D_270:
							if (_play_field[y+1][x] ||
									_play_field[y+2][x+1]){
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
							if (_play_field[y-1][x] ||
									_play_field[y][x-1]){
								legal_move = false;
							}
							break;
						//
						// 3 2 
						//   1 0
						case D_90: case D_270:
							if (_play_field[y+1][x] ||
									_play_field[y][x-1] ||
									_play_field[y][x-2]){
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
							(d_rot == D_0 && x_max == FIELD_X-1) ||
							(d_rot == D_90 && y_max == FIELD_Y-1)){
						legal_move = false;
					}

					x = origin.x;
					y = origin.y;
					switch(d_rot){
						case D_0:
							// 3 0
							//   1
							//   2
							if (_play_field[y+1][x] ||
									_play_field[y+1][x-2] ||
									_play_field[y+2][x-2]){
								legal_move = false;
							}
							break;
						case D_90:
							//     3
							// 2 1 0
							// 
							if (_play_field[y][x-1] ||
									_play_field[y][x-2] ||
									_play_field[y+2][x-1]){
								legal_move = false;
							}
							break;
						case D_180:
							//   1 
							//   2
							//   3 0
							if (_play_field[y][x] ||
									_play_field[y+1][x] ||
									_play_field[y+1][x-2]){
								legal_move = false;
							}
							break;
						case D_270:
							//
							// 0 1 2
							// 3    
							if (_play_field[y][x-1] ||
									_play_field[y+2][x] ||
									_play_field[y+2][x-1]){
								legal_move = false;
							}
							break;

					}
					if (legal_move)
						tetronimo->d_rot = rotate_Tetronimo(tetronimo, move);
					break;
				case M_ROT_RIGHT:
					if ((d_rot == D_180 && x_min == 0) || 
							(d_rot == D_0 && x_max == FIELD_X-1) ||
							(d_rot == D_90 && y_max == FIELD_Y-1)){
						legal_move = false;
					}
					x = origin.x;
					y = origin.y;
					switch(d_rot){
						case D_0:
							// 3 0
							//   1
							//   2
							if (_play_field[y][x] ||
									_play_field[y+1][x] ||
									_play_field[y+1][x-2]){
								legal_move = false;
							}
							break;
						case D_90:
							//     3
							// 2 1 0
							// 
							if (_play_field[y][x-1] ||
									_play_field[y+2][x] ||
									_play_field[y+2][x-1]){
								legal_move = false;
							}
							break;
						case D_180:
							//   1 
							//   2
							//   3 0
							if (_play_field[y+1][x] ||
									_play_field[y+1][x-2] ||
									_play_field[y+2][x-2]){
								legal_move = false;
							}
							break;
						case D_270:
							//
							// 0 1 2
							// 3    
							if (_play_field[y][x-1] ||
									_play_field[y][x-2] ||
									_play_field[y+2][x-1]){
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
							// 3 0
							//   1
							//   2
							for (int i = 2; i <= 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y+1][x])
									legal_move = false;
							}
							break;
						case D_90:
							//     3
							// 2 1 0
							// 
							for (int i = 0; i <= 2; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y+1][x])
									legal_move = false;
							}
							break;
						case D_180:
							//   1 
							//   2
							//   3 0
							x = origin.x;
							y = origin.y;
							if (_play_field[y+3][x] ||
									_play_field[y+3][x-1]){
								legal_move = false;
							}
							break;
						case D_270:
							//
							// 0 1 2
							// 3    
							for (int i = 1; i <= 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y+1][x])
									legal_move = false;
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
							// 3 0
							//   1
							//   2
							for (int i = 1; i <= 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y][x-1]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_90:
							//     3
							// 2 1 0
							// 
							for (int i = 2; i <= 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y][x-1]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_180:
							//   1 
							//   2
							//   3 0
							for (int i = 1; i <= 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y][x-1]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_270:
							//
							// 0 1 2
							// 3    
							x = origin.x;
							y = origin.y;
							if (_play_field[y+1][x-3] ||
									_play_field[y+2][x-3]){
								legal_move = false;
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
							// 3 0
							//   1
							//   2
							for (int i = 0; i <= 2; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y][x+1]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_90:
							//     3
							// 2 1 0
							// 
							for (int i = 0; i < TETRA; i++){
								if (i == 0 || i == 3){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (_play_field[y][x+1]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_180:
							//   1 
							//   2
							//   3 0
							for (int i = 0; i < TETRA; i++){
								if (i != 3){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (_play_field[y][x+1]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_270:
							//
							// 0 1 2
							// 3    
							for (int i = 2; i <= 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y][x+1]){
									legal_move = false;
									break;
								}
							
							}
							break;
					}
					if (legal_move)
						tetronimo->origin.x += 1;

					break;
				case M_UP:
					switch(d_rot){
						case D_0:
							// 3 0
							//   1
							//   2
							for (int i = 0; i < 3; i++){
								if (i == 0 || i == 3){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (_play_field[y-1][x]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_90:
							//     3
							// 2 1 0
							// 
							for (int i = 1; i <= 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y-1][x]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_180:
							//   1 
							//   2
							//   3 0
							for (int i = 0; i <= 1; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y-1][x]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_270:
							//
							// 0 1 2
							// 3    
							for (int i = 0; i <= 2; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y-1][x]){
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
		// J-Piece
		case T_J:
		{
			switch(move){
				case M_ROT_LEFT: 
					if ((d_rot == D_180 && x_min == 0) || 
							(d_rot == D_0 && x_max == FIELD_X-1) ||
							(d_rot == D_90 && y_max == FIELD_Y-1)){
						legal_move = false;
					}

					x = origin.x;
					y = origin.y;
					switch(d_rot){
						case D_0:
							//   0
							//   1
							// 3 2
							if (_play_field[y+1][x] ||
									_play_field[y+2][x] ||
									_play_field[y+1][x-2]){
								legal_move = false;
							}
							break;
						case D_90:
							// 3
							// 2 1 0
							// 
							if (_play_field[y][x-1] ||
									_play_field[y+2][x-2] ||
									_play_field[y+2][x-1]){
								legal_move = false;
							}
							break;
						case D_180:
							//   1 0
							//   2
							//   3
							if (_play_field[y][x-2] ||
									_play_field[y+1][x-2] ||
									_play_field[y+1][x]){
								legal_move = false;
							}
							break;
						case D_270:
							//
							// 0 1 2
							//     3
							if (_play_field[y][x] ||
									_play_field[y][x-1] ||
									_play_field[y+2][x-1]){
								legal_move = false;
							}
							break;

					}
					if (legal_move)
						tetronimo->d_rot = rotate_Tetronimo(tetronimo, move);
					break;
				case M_ROT_RIGHT:
					if ((d_rot == D_180 && x_min == 0) || 
							(d_rot == D_0 && x_max == FIELD_X-1) ||
							(d_rot == D_90 && y_max == FIELD_Y-1)){
						legal_move = false;
					}
					x = origin.x;
					y = origin.y;
					switch(d_rot){
						case D_0:
							//   0
							//   1
							// 3 2
							if (_play_field[y][x-2] ||
									_play_field[y+1][x-2] ||
									_play_field[y+1][x]){
								legal_move = false;
							}
							break;
						case D_90:
							// 3
							// 2 1 0
							// 
							if (_play_field[y][x] ||
									_play_field[y][x-1] ||
									_play_field[y+2][x-1]){
								legal_move = false;
							}
							break;
						case D_180:
							//   1 0
							//   2
							//   3
							if (_play_field[y+1][x-2] ||
									_play_field[y+1][x] ||
									_play_field[y+2][x]){
								legal_move = false;
							}
							break;
						case D_270:
							//
							// 0 1 2
							//     3
							if (_play_field[y][x-1] ||
									_play_field[y+2][x-1] ||
									_play_field[y+2][x-2]){
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
								if (_play_field[y+1][x])
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
								if (_play_field[y+1][x])
									legal_move = false;
							}
							break;
						case D_180:
							//   1 0
							//   2
							//   3
							x = origin.x;
							y = origin.y;
							if (_play_field[y+3][x-1] ||
									_play_field[y+1][x]){
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
									if (_play_field[y+1][x])
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
									if (_play_field[y][x-1]){
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
								if (_play_field[y][x-1]){
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
								if (_play_field[y][x-1]){
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
									if (_play_field[y][x-1]){
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
								if (_play_field[y][x+1]){
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
									if (_play_field[y][x+1]){
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
									if (_play_field[y][x+1]){
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
								if (_play_field[y][x+1]){
									legal_move = false;
									break;
								}
							
							}
							break;
					}
					if (legal_move)
						tetronimo->origin.x += 1;

					break;
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
									if (_play_field[y-1][x]){
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
									if (_play_field[y-1][x]){
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
								if (_play_field[y-1][x]){
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
								if (_play_field[y-1][x]){
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
		case T_T:
		{
			switch(move){
				case M_ROT_LEFT: // Fall through case
				case M_ROT_RIGHT:
					if ((d_rot == D_180 && x_min == 0) || 
							(d_rot == D_0 && x_max == FIELD_X-1) ||
							(d_rot == D_90 && y_max == FIELD_Y-1)){
						legal_move = false;
					}
					x = origin.x;
					y = origin.y;
					switch(d_rot){
						case D_0:
							//   0
							// 3 1
							//   2
							if (_play_field[y+1][x])
								legal_move = false;
							break;
						case D_90:
							//   3
							// 0 1 2
							// 
							if (_play_field[y+2][x-1])
								legal_move = false;
							break;
						case D_180:
							//   0 
							//   1 3
							//   2
							if (_play_field[y+1][x-2])
								legal_move = false;
							break;
						case D_270:
							//
							// 0 1 2
							//   3
							if (_play_field[y][x-1])
								legal_move = false;
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
							// 3 1
							//   2
							for (int i = 2; i <= 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y+1][x]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_90:
							//   3
							// 0 1 2
							// 
							for (int i = 0; i <= 2; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y+1][x]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_180:
							//   0 
							//   1 3
							//   2
							for (int i = 2; i <= 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y+1][x]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_270:
							//
							// 0 1 2
							//   3
							for (int i = 2; i <= 3; i++){
								if (i != 1){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (_play_field[y+1][x]){
										legal_move = false;
										break;
									}
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
							// 3 1
							//   2
							for (int i = 0; i < TETRA; i++){
								if (i != 1){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (_play_field[y][x-1]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_90:
							//   3
							// 0 1 2
							// 
							for (int i = 0; i < TETRA; i++){
								if (i != 1 || i != 2){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (_play_field[y][x-1]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_180:
							//   0 
							//   1 3
							//   2
							for (int i = 0; i <= 2; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y][x-1]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_270:
							//
							// 0 1 2
							//   3
							for (int i = 0; i < TETRA; i++){
								if (i != 1 || i != 2){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (_play_field[y][x-1]){
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
							// 3 1
							//   2
							for (int i = 0; i <= 2; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y][x+1]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_90:
							//   3
							// 0 1 2
							// 
							for (int i = 2; i <= 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y][x+1]){
									legal_move = false;
									break;
								}
							}
							break;
						case D_180:
							//   0 
							//   1 3
							//   2
							for (int i = 0; i < TETRA; i++){
								if (i != 1){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (_play_field[y][x+1]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_270:
							//
							// 0 1 2
							//   3
							for (int i = 2; i <= 3; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y][x+1]){
									legal_move = false;
									break;
								}
							}
							break;
					}
					if (legal_move)
						tetronimo->origin.x += 1;

					break;
				case M_UP:
					switch(d_rot){
						case D_0:
							//   0
							// 3 1
							//   2
							for (int i = 0; i <= 3; i++){
								if (i != 1){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (_play_field[y-1][x]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_90:
							//   3
							// 0 1 2
							// 
							for (int i = 0; i < TETRA; i++){
								if (i != 1){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (_play_field[y-1][x]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_180:
							//   0 
							//   1 3
							//   2
							for (int i = 0; i <= 3; i++){
								if (i != 1){
									x = tetronimo->pieces[i].x;
									y = tetronimo->pieces[i].y;
									if (_play_field[y-1][x]){
										legal_move = false;
										break;
									}
								}
							}
							break;
						case D_270:
							//
							// 0 1 2
							//   3
							for (int i = 0; i <= 2; i++){
								x = tetronimo->pieces[i].x;
								y = tetronimo->pieces[i].y;
								if (_play_field[y-1][x]){
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
	set_Tetronimo(tetronimo); 
	// Set tetronimo to _play_field
	set_To_Field(tetronimo);
	return is_falling;
}
