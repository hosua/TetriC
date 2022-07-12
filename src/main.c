#include "graphics.h"
#include "tetris.h"
#include "input.h"
#include "clock.h"
#include "queue.h"
#include "sounds.h"

#include "test.h"

// Variable to determine if a piece is still falling. If one is not falling, a new piece is spawned.
// and then this variable is set to true again.
bool is_falling = false;


int main(int argc, char **argv){
	// seed the RNG
	srand(time(0));

	int sound_res = 0; 
	int flags = MIX_INIT_MP3;

	// TODO: Currently, we always start the game at level 0. I need to implement a way to start at a different level.
	init_GameData(0);
	// tick = 17, input_tick = 50, nudge delay = 1000, 
	init_Clock(17, 50, 1000);

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture *texture = NULL;

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

	if (flags != (sound_res = Mix_Init(flags))) {
        printf("Could not initialize mixer (result: %d).\n", sound_res);
        printf("Mix_Init: %s\n", Mix_GetError());
        exit(1);
    }

	// The points accrued from holding down
	uint8_t down_points = 0;
	GFX_ClearScreen(window, renderer);
	
	/* Sound stuff */
	// Loud sound effects into memory
	init_SFX(50);
	SFX_PlayBGM("sfx/trepak.mp3", 50);
	/* End sound stuff */

	/* Queue stuff */
	Queue queue = init_Queue();
	// Fill queue with pieces
	for (int i = 0; i < _queue_limit; i++)
		enqueue(rand_T_Type(), &queue);
	// Get first piece from queue
	Tetronimo* tetronimo = new_Tetronimo(peek(queue));
	dequeue(&queue);
	enqueue(rand_T_Type(), &queue);
	/* End queue stuff */
	
	// Char buffer for rendering text
	char buf[128];
	uint8_t buf_max = sizeof(buf);
	uint8_t lines_cleared_this_turn = 0;

	float elapsed = 0.0f;
	_game_data->fps = elapsed;
	GetLinesUntilNextLevel(_game_data->level);

	for ( ; ; ){
		uint64_t start = SDL_GetPerformanceCounter();
		SDL_Event event; 
		SDL_PollEvent(&event);
		GFX_ClearScreen(window, renderer);
		// Get user input
		SetKeyArray(event, window, renderer);
		MovementHandler(event, window, renderer, tetronimo);
		
		VolumeController(event);
		
		if (InputTimer()){
			DownwardMovementHandler(&down_points, window, renderer, tetronimo);
		}
		// NudgeTimer() prevents the nudge sfx from being spammed
		if (NudgeTimer()){
			_sfx->should_nudge = true;
		}
		if (LevelTimer(_game_data->level)){
			is_falling = move_Tetronimo(window, renderer, tetronimo, M_DOWN);
			if (!is_falling){
				if (IsPlayerDead())
					QuitGame(window, renderer);
				

				lines_cleared_this_turn = CheckLines();
				if (!_sfx->muted){
					if (lines_cleared_this_turn == 4)
						SFX_PlaySFX(_sfx->tetris);
					else if (lines_cleared_this_turn == 0)
						SFX_PlaySFX(_sfx->lock_piece);
					else
						SFX_PlaySFX(_sfx->line_clear);

				}
				// The number of lines to carry over to the next level
				uint8_t carry = lines_cleared_this_turn - _game_data->lines_until_level;
				_game_data->lines_until_level -= lines_cleared_this_turn;
				free(tetronimo);

				// dequeue a new piece from queue to spawn it, then enqueue another 
				T_Type t_type = peek(queue);
				tetronimo = new_Tetronimo(t_type);
				_game_data->tetronimo_counter[t_type]++;
				dequeue(&queue);
				enqueue(rand_T_Type(), &queue);
				// print_Queue(queue.head);
				
				_game_data->player_score += CalcScore(lines_cleared_this_turn, _game_data->level);
				_game_data->player_score += down_points;

				if (_game_data->lines_until_level <= 0){
					if (!_sfx->muted)
						SFX_PlaySFX(_sfx->level_up);
					_game_data->level++;
					// Set _game_data->lines_until_level
					GetLinesUntilNextLevel(_game_data->level);
					// Carry over extra lines
					if (carry){
						_game_data->lines_until_level -= carry;
					}
				}
				down_points = 0;
			}
		}

		_game_data->fps = elapsed;
		// Render Tetronimos	
		GFX_RenderBlocks(8, 0, window, renderer);
		// Render the UI elements
		GFX_RenderUI(20, 0, BLOCK_SIZE, buf, buf_max, _game_data->level, 
				window, renderer, texture, font);
		
		GFX_RenderStatsUI(3, 1, BLOCK_SIZE/1.8f, buf, buf_max,
				font, texture, window, renderer);
		GFX_RenderQueue(30, 5, BLOCK_SIZE/1.8f, buf, buf_max, 3,
				queue, font, texture, window, renderer);
		// Present the renderings to the screen
		SDL_RenderPresent(renderer); 
		// Calculate FPS
		uint64_t end = SDL_GetPerformanceCounter();
		elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
	}

	QuitGame(window, renderer);
}
