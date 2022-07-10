#include "input.h"

// If keys[i] = 1, key is down, else it is up

// Key enums
// K_LEFT, K_DOWN, K_UP, KEY_RIGHT, K_Z, K_X 
bool keys[NUM_KEYS] = {0};

void SetKeyArray(SDL_Event event, SDL_Window* window, SDL_Renderer* renderer){
	const uint8_t *kb_state = SDL_GetKeyboardState(NULL);

	if (kb_state[SDL_SCANCODE_ESCAPE]){
		printf("Player quit the game.\n");
		QuitGame(window, renderer);
		exit(0);
	}

	if (kb_state[SDL_SCANCODE_LEFT]) keys[K_LEFT] = true;
	else keys[K_LEFT] = false;

	if (kb_state[SDL_SCANCODE_DOWN]) keys[K_DOWN] = true;
	else keys[K_DOWN] = false;

	if (kb_state[SDL_SCANCODE_UP]) keys[K_UP] = true;
	else keys[K_UP] = false;

	if (kb_state[SDL_SCANCODE_RIGHT]) keys[K_RIGHT] = true;
	else keys[K_RIGHT] = false;

	if (kb_state[SDL_SCANCODE_Z]) keys[K_Z] = true;
	else keys[K_Z] = false;

	if (kb_state[SDL_SCANCODE_X]) keys[K_X] = true;
	else keys[K_X] = false;
}

void MusicHandler(SDL_Event event){
	const uint8_t *kb_state = SDL_GetKeyboardState(NULL);
	if (event.type == SDL_KEYDOWN && !event.key.repeat){
		if (kb_state[SDL_SCANCODE_M]){
			if (_sfx_muted){
				_sfx_muted = false;
				SFX_ResumeBGM();
				printf("Muted SFX\n");
			} else {
				_sfx_muted = true;
				SFX_MuteBGM();
				printf("Unmuted SFX\n");
			}
		}
	}
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
void DownwardMovementHandler(uint8_t* down_points, SDL_Window* window, SDL_Renderer* renderer, Tetronimo* tetronimo){
	if(keys[K_DOWN]){ 
		move_Tetronimo(window, renderer, tetronimo, M_DOWN);
		(*down_points)++;
	}
}

// All other movement will be 1:1
void MovementHandler(SDL_Event event, SDL_Window* window, SDL_Renderer* renderer, Tetronimo* tetronimo){
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
