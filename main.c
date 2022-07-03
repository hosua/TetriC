#include "graphics.h"
#include "tetris.h"

int main(){
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Surface* surface = NULL;
	SDL_Color color = { 255, 255, 255};
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

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	// SDL_RenderPresent(renderer);
	RenderPlayField(window, renderer);
	I_Piece i_piece = *new_I_Piece();

	// RenderBlocksByBlock(window, renderer);
	RenderPlayField(window, renderer);
	RenderBlocksByLine(window, renderer);
	for (int i = 0; i < 20; i++){
		ClearScreen(window, renderer);
		RenderPlayField(window, renderer);
		move_I_Piece(window, renderer, &i_piece, M_DOWN);
		PrintPlayField();
		// RenderBlocksByBlock(window, renderer);
		RenderBlocksByLine(window, renderer);
		SDL_Delay(500);
		// RenderPlayField(window, renderer);
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
}
