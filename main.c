#include "graphics.h"
#include "tetris.h"
#include "input.h"
#include "clock.h"

#include "test.h"

// Variable to determine if a piece is still falling. If one is not falling, a new piece is spawned.
// and then this variable is set to true again.
bool is_falling = false;

int main(int argc, char **argv){
	// seed the RNG
	srand(time(0));

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	window = SDL_CreateWindow("TetriC", 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			SCREEN_X, SCREEN_Y, 0);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	TTF_Init();
	TTF_Font* font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
	if (!font){
		fprintf(stderr, "Error: Font not found\n");
		exit(EXIT_FAILURE);
	}
	if (SDL_Init(SDL_INIT_EVERYTHING)){
		fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
		exit(1);
	}
	
	SDL_Texture *texture = NULL;
	// init_test_2();	
	// The points accrued from holding down
	uint8_t down_points = 0;
	ClearScreen(window, renderer);
	Tetronimo* tetronimo = rand_Piece();
	// Tetronimo* tetronimo = new_Piece(T_I);
	
	// Char buffer for rendering text
	char buf[128];
	uint8_t buf_max = sizeof(buf);
	uint8_t lines_cleared_this_turn = 0;

	GetLinesUntilNextLevel(_curr_level);

	for ( ; ; ){
		uint64_t start = SDL_GetPerformanceCounter();
		// Get user input
		SDL_Event event; 
		SDL_PollEvent(&event);
		ClearScreen(window, renderer);
		SetKeyArray(event, window);
		MovementHandler(event, window, renderer, tetronimo);

		
		if (InputTimer()){
			DownwardMovementHandler(&down_points, window, renderer, tetronimo);
		}

		if (LevelTimer(_curr_level)){
			is_falling = move_Tetronimo(window, renderer, tetronimo, M_DOWN);
			if (!is_falling){
				if (IsPlayerDead()){
					QuitGame(window);
				}
				lines_cleared_this_turn = CheckLines();
				_lines_until_level -= lines_cleared_this_turn;
				free(tetronimo);
				tetronimo = rand_Piece();
				// tetronimo = new_Piece(T_I);
				
				_player_score += CalcScore(lines_cleared_this_turn, _curr_level);
				_player_score += down_points;

				if (_lines_until_level == 0){
					// Level up
					_curr_level++;
					GetLinesUntilNextLevel(_curr_level);
				}
				down_points = 0;
			}
		}

		// Render the field
		RenderPlayField(window, renderer);
		// Render Tetronimos	
		RenderBlocks(window, renderer);
		// Render the UI elements
		RenderUI(buf, buf_max, window, renderer, texture, font);
		// Present the renderings to the screen
		SDL_DestroyTexture(texture);
		SDL_RenderPresent(renderer); 
		sleep_us(16667);
		// Calculate FPS
		uint64_t end = SDL_GetPerformanceCounter();
		_fps = (end - start) / (float)SDL_GetPerformanceFrequency();
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
}
