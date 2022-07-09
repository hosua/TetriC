#include "input.h"
#include "tetris.h"

// If keys[i] = 1, key is down, else it is up

// Key enums
// K_LEFT, K_DOWN, K_UP, KEY_RIGHT, K_Z, K_X 
bool keys[NUM_KEYS] = {0};

void SetKeyArray(SDL_Event event, SDL_Window* window){
	const uint8_t *kb_state = SDL_GetKeyboardState(NULL);

	if (kb_state[SDL_SCANCODE_ESCAPE]){
		printf("Exiting... Goodbye!\n");
		SDL_DestroyWindow(window);
		SDL_Quit();
		exit(0);
	}

	if (kb_state[SDL_SCANCODE_LEFT]) keys[0] = true;
	else keys[0] = false;

	if (kb_state[SDL_SCANCODE_DOWN]) keys[1] = true;
	else keys[1] = false;

	if (kb_state[SDL_SCANCODE_UP]) keys[2] = true;
	else keys[2] = false;

	if (kb_state[SDL_SCANCODE_RIGHT]) keys[3] = true;
	else keys[3] = false;

	if (kb_state[SDL_SCANCODE_Z]) keys[4] = true;
	else keys[4] = false;

	if (kb_state[SDL_SCANCODE_X]) keys[5] = true;
	else keys[5] = false;
}

const char* K_Name_to_str(K_Name k_name){
	switch(k_name){
		case K_LEFT: return "←";
		case K_DOWN: return "↓";
		case K_UP: return "↑";
		case K_RIGHT: return "→";
		case K_Z: return "z";
		case K_X: return "x"; 
		default: return "Unknown K_Name"; 
	}
}

// Checks the keys array and moves the piece depending on if the key is set or not
// Holding down is the only key that we should handle repeats for. All other keys will happen 1:1 with key presses.
void MovementHandler(SDL_Window* window, SDL_Renderer* renderer, Tetronimo* tetronimo){
	if(keys[K_DOWN]) move_Tetronimo(window, renderer, tetronimo, M_DOWN);
	// if(keys[K_LEFT]) move_Tetronimo(window, renderer, tetronimo, M_LEFT);
	// if(keys[K_UP]) move_Tetronimo(window, renderer, tetronimo, M_UP);
	// if(keys[K_RIGHT]) move_Tetronimo(window, renderer, tetronimo, M_RIGHT);
}

// Since we don't want repeated movement for rotations, they must be implemented separately.
void RotationHandler(SDL_Event event, SDL_Window* window, SDL_Renderer* renderer, Tetronimo* tetronimo){
	const uint8_t *kb_state = SDL_GetKeyboardState(NULL);

	if (event.type == SDL_KEYDOWN && !event.key.repeat){
		// if (kb_state[SDL_SCANCODE_UP]) move_Tetronimo(window, renderer, tetronimo, M_UP);
		if (kb_state[SDL_SCANCODE_LEFT]) move_Tetronimo(window, renderer, tetronimo, M_LEFT);
		if (kb_state[SDL_SCANCODE_RIGHT]) move_Tetronimo(window, renderer, tetronimo, M_RIGHT);
		if (kb_state[SDL_SCANCODE_Z]) move_Tetronimo(window, renderer, tetronimo, M_ROT_LEFT);
		if (kb_state[SDL_SCANCODE_X]) move_Tetronimo(window, renderer, tetronimo, M_ROT_RIGHT);
	}
}
void PrintKeys(){
	printf("Keys:\n");
	for (int key = 0; key < NUM_KEYS; key++){
		printf("%s : %i\t", K_Name_to_str(key), keys[key]);
		if (key == K_RIGHT)
			printf("\n");
	}
	printf("\n");
}
