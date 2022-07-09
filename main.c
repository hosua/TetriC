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
	SDL_Surface* surface = NULL;

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
	
	SDL_Rect rect;
	SDL_Texture *texture = NULL;
	// init_test_2();	
	ClearScreen(window, renderer);
	Tetronimo* tetronimo = rand_Piece();
	// Tetronimo* tetronimo = new_Piece(T_I);
	for ( ;  ; ){
		// Get user input
		SDL_Event event; 
		SDL_PollEvent(&event);
		SetKeyArray(event, window);

		RotationHandler(event, window, renderer, tetronimo);

		ClearScreen(window, renderer);
		RenderPlayField(window, renderer);
		
		if (InputTimer()){
			MovementHandler(window, renderer, tetronimo);
		}

		if (TickTimer()){
			is_falling = move_Tetronimo(window, renderer, tetronimo, M_DOWN);
			if (!is_falling){
				CheckLines();
				free(tetronimo);
				tetronimo = rand_Piece();
				// tetronimo = new_Piece(T_I);
			}
			// PrintPlayField();
		}

		RenderBlocks(window, renderer);
		RenderText(renderer, (BLOCK_SIZE * 12), BLOCK_SIZE, "Lines cleared: ", font, &texture, &rect);
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		RenderNumber(renderer, (BLOCK_SIZE * 18), BLOCK_SIZE, _lines_cleared, font, &texture, &rect);
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		SDL_RenderPresent(renderer); // Only call RenderPresent once per cycle!
	
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
}
