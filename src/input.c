#include "input.h"
#include "graphics.h"

// If keys[i] = 1, key is down, else it is up

// Key enums
// K_LEFT, K_DOWN, K_UP, KEY_RIGHT, K_Z, K_X 
bool keys[NUM_KEYS] = {0};

void Input_SetKeyArray(SDL_Event event){
	const uint8_t *kb_state = SDL_GetKeyboardState(NULL);

	if (kb_state[SDL_SCANCODE_ESCAPE]){
		printf("Player quit the game.\n");
		QuitGame(E_INGAME);
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

void Input_VolumeController(SDL_Event event){
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
void Input_DownwardMovementHandler(Tetronimo* tetronimo){
	if(keys[K_DOWN]){ 
		move_Tetronimo(tetronimo, M_DOWN);
		_game_data->down_points++;
	}
}

// All other movement will be 1:1
void Input_MovementHandler(SDL_Event event, Tetronimo* tetronimo){
	const uint8_t *kb_state = SDL_GetKeyboardState(NULL);

	if (event.type == SDL_KEYDOWN && !event.key.repeat){
		// if (kb_state[SDL_SCANCODE_UP]) move_Tetronimo(_gfx->window, _gfx->renderer, tetronimo, M_UP);
		if (kb_state[SDL_SCANCODE_LEFT]) move_Tetronimo(tetronimo, M_LEFT);
		if (kb_state[SDL_SCANCODE_RIGHT]) move_Tetronimo(tetronimo, M_RIGHT);
		if (kb_state[SDL_SCANCODE_Z]) move_Tetronimo(tetronimo, M_ROT_LEFT);
		if (kb_state[SDL_SCANCODE_X]) move_Tetronimo(tetronimo, M_ROT_RIGHT);
	}
}
void Input_PrintKeys(){
	printf("Keys:\n");
	for (int key = 0; key < NUM_KEYS; key++){
		printf("%s : %i\t", K_Name_to_str(key), keys[key]);
		if (key == K_RIGHT)
			printf("\n");
	}
	printf("\n");
}

// Handles events and set the button's sprite region
void Input_HandleButtonEvents(SDL_Event *e, Button* button){
//If mouse event happened
    if(e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP){
        // Get mouse position
        int mx, my;
        SDL_GetMouseState(&mx, &my);
		// Check if mouse is in button
        bool inside = true;
		// printf("mouse: (%i,%i)\n", mx, my);
        // Mouse is left of the button
        if (mx < button->origin.x){
            inside = false;
        // Mouse is right of the button
        } else if (mx > button->origin.x + button->box.w){
            inside = false;
        // Mouse above the button
        } else if (my < button->origin.y ){
            inside = false;
        // Mouse below the button
        } else if (my > button->origin.y + button->box.h){
            inside = false;
        }
		 //Mouse is outside button
        if(!inside){
			SDL_Color color = {128, 128, 50, 255};
			button->color = color;
        //Mouse is inside button
        } else {
            switch( e->type ){
                case SDL_MOUSEMOTION:
                	break;
            
                case SDL_MOUSEBUTTONDOWN:
					switch (button->action){
						case B_PLAY:
							printf("Starting the game\n");
							_game_state = G_PLAY;
							break;
						case B_MAINMENU:
							_game_state = G_MAINMENU;
							printf("Entering main menu\n");
							break;
						case B_LEVELSELECT:
							printf("Entering level select\n");
							_game_state = G_LEVELSELECT;
							break;
						case B_LEVEL:
						{
							uint8_t level = GFX_GetLevelButtonLevel(button);
							_start_level = level;
							_game_state = G_PLAY;
							break;
						}
						case B_SETTINGS:
							printf("Entering settings menu\n");
							break;
						case B_QUIT:
							printf("Quitting the game...\n");
							QuitGame(E_MAINMENU);
							break;
						default:
							fprintf(stderr, "Error: Invalid button action\n");
							break;

					}
                	break;
                
                case SDL_MOUSEBUTTONUP:
                	break;
            }
        }
	}
}

