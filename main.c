#include "graphics.h"
#include "tetris.h"
#include "input.h"
#include "clock.h"

int main(){
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Surface* surface = NULL;

	window = SDL_CreateWindow("Tetris", 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			SCREEN_X, SCREEN_Y, 0);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	TTF_Init();
	TTF_Font* font = TTF_OpenFont(FONT_PATH, 25);
	if (!font){
		fprintf(stderr, "Error: Font not found\n");
		exit(EXIT_FAILURE);
	}
	if (SDL_Init(SDL_INIT_EVERYTHING)){
		printf("Error initializing SDL: %s\n", SDL_GetError());
		exit(1);
	}
	
	ClearScreen(window, renderer);
	RenderPlayField(window, renderer);
	I_Piece i_piece = *new_I_Piece();
	Tetronimo* tetronimo = &i_piece.tetronimo;
	RenderPlayField(window, renderer);
	RenderBlocksByLine(window, renderer);
	SDL_RenderPresent(renderer); // Only call RenderPresent once per cycle!
								 
	for ( ;  ; ){
		// Get user input
		InputController(window, renderer, tetronimo);
		ClearScreen(window, renderer);
		RenderPlayField(window, renderer);

		// PrintPlayField();
		if (TickCounter()){
			move_Tetronimo(window, renderer, tetronimo, M_DOWN);
		}
		RenderBlocksByLine(window, renderer);
		SDL_RenderPresent(renderer); // Only call RenderPresent once per cycle!
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
}
