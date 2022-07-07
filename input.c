#include "input.h"
#include "tetris.h"

void InputController(SDL_Window* window, SDL_Renderer* renderer, Tetronimo* tetronimo){
	const uint8_t *kb_state = SDL_GetKeyboardState(NULL);
	SDL_Event event; 
	SDL_PollEvent(&event);
	if (event.type == SDL_KEYDOWN){
		if (kb_state[SDL_SCANCODE_ESCAPE]){
			printf("Exiting... Goodbye!\n");
			SDL_DestroyWindow(window);
			SDL_Quit();
			exit(0);
		}
		if (kb_state[SDL_SCANCODE_DOWN]){
			if (VERBOSE) printf("Pressed down\n");
			move_Tetronimo(window, renderer, tetronimo, M_DOWN);
		}
		if (kb_state[SDL_SCANCODE_UP]){
			if (VERBOSE) printf("Pressed up\n");
			move_Tetronimo(window, renderer, tetronimo, M_UP);
		}
		if (kb_state[SDL_SCANCODE_LEFT]){
			if (VERBOSE) printf("Pressed left\n");
			move_Tetronimo(window, renderer, tetronimo, M_LEFT);
		}
		if (kb_state[SDL_SCANCODE_RIGHT]){
			if (VERBOSE) printf("Pressed right\n");
			move_Tetronimo(window, renderer, tetronimo, M_RIGHT);
		}
		if (kb_state[SDL_SCANCODE_Z]){
			if (VERBOSE) printf("Pressed z\n");
			move_Tetronimo(window, renderer, tetronimo, M_ROT_LEFT);
		}
		if (kb_state[SDL_SCANCODE_X]){
			if (VERBOSE) printf("Pressed x\n");
			move_Tetronimo(window, renderer, tetronimo, M_ROT_RIGHT);
		}
		
	}
}
