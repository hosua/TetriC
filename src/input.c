#include "input.h"
#include "graphics.h"

// If keys[i] = 1, key is down, else it is up

// Key enums
// K_LEFT, K_DOWN, K_UP, KEY_RIGHT, K_Z, K_X 
bool keys[NUM_KEYS] = {0};

void SetKeyArray(SDL_Event event){
	const uint8_t *kb_state = SDL_GetKeyboardState(NULL);

	if (kb_state[SDL_SCANCODE_ESCAPE]){
		printf("Player quit the game.\n");
		QuitGame(_gfx->window, _gfx->renderer);
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

void VolumeController(SDL_Event event){
	const uint8_t *kb_state = SDL_GetKeyboardState(NULL);
	if (event.type == SDL_KEYDOWN && !event.key.repeat){
		if (kb_state[SDL_SCANCODE_M]){
			if (_sfx->muted){
				_sfx->muted = false;
				SFX_ResumeBGM();
				printf("Unmuted SFX\n");
			} else {
				_sfx->muted = true;
				SFX_MuteBGM();
				printf("Muted SFX\n");
			}
		}
	}
}

const char* K_Name_to_str(K_Name k_name){
	switch(k_name){
		case K_LEFT: return "left";
		case K_DOWN: return "down";
		case K_UP: return "up";
		case K_RIGHT: return "right";
		case K_Z: return "z";
		case K_X: return "x"; 
		default: return "Unknown K_Name"; 
	}
}

// Checks the keys array and moves the piece depending on if the key is set or not
void DownwardMovementHandler(Tetronimo* tetronimo){
	if(keys[K_DOWN]){ 
		move_Tetronimo(tetronimo, M_DOWN);
		_game_data->down_points++;
	}
}

// All other movement will be 1:1
void MovementHandler(SDL_Event event, Tetronimo* tetronimo){
	const uint8_t *kb_state = SDL_GetKeyboardState(NULL);

	if (event.type == SDL_KEYDOWN && !event.key.repeat){
		// if (kb_state[SDL_SCANCODE_UP]) move_Tetronimo(_gfx->window, _gfx->renderer, tetronimo, M_UP);
		if (kb_state[SDL_SCANCODE_LEFT]) move_Tetronimo(tetronimo, M_LEFT);
		if (kb_state[SDL_SCANCODE_RIGHT]) move_Tetronimo(tetronimo, M_RIGHT);
		if (kb_state[SDL_SCANCODE_Z]) move_Tetronimo(tetronimo, M_ROT_LEFT);
		if (kb_state[SDL_SCANCODE_X]) move_Tetronimo(tetronimo, M_ROT_RIGHT);
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
