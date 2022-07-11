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

	if (flags != (sound_res = Mix_Init(flags))) {
        printf("Could not initialize mixer (result: %d).\n", sound_res);
        printf("Mix_Init: %s\n", Mix_GetError());
        exit(1);
    }
	
	SDL_Texture *texture = NULL;
	// init_test_2();	
	// The points accrued from holding down
	uint8_t down_points = 0;
	uint8_t curr_level = 0;
	ClearScreen(window, renderer);
	
	/* Sound stuff */
	// Loud sound effects into memory
	SFX_LoadSoundBank();
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
	_fps = elapsed;
	GetLinesUntilNextLevel(curr_level);

	for ( ; ; ){
		uint64_t start = SDL_GetPerformanceCounter();
		// Get user input
		SDL_Event event; 
		SDL_PollEvent(&event);
		ClearScreen(window, renderer);
		SetKeyArray(event, window, renderer);
		MovementHandler(event, window, renderer, tetronimo);
		
		VolumeController(event);
		
		if (InputTimer()){
			DownwardMovementHandler(&down_points, window, renderer, tetronimo);
		}
		// NudgeTimer() prevents the nudge sfx from being spammed
		if (NudgeTimer()){
			_should_nudge = true;
		}
		if (LevelTimer(curr_level)){
			is_falling = move_Tetronimo(window, renderer, tetronimo, M_DOWN);
			if (!is_falling){
				if (IsPlayerDead())
					QuitGame(window, renderer);
				

				lines_cleared_this_turn = CheckLines();
				if (!_sfx_muted){
					if (lines_cleared_this_turn == 4)
						SFX_PlaySFX(_sfx_tetris);
					else if (lines_cleared_this_turn == 0)
						SFX_PlaySFX(_sfx_lockpiece);
					else
						SFX_PlaySFX(_sfx_lineclear);
				}
				// The number of lines to carry over to the next level
				uint8_t carry = lines_cleared_this_turn - _lines_until_level;
				_lines_until_level -= lines_cleared_this_turn;
				free(tetronimo);

				// dequeue a new piece from queue to spawn it, then enqueue another 
				T_Type t_type = peek(queue);
				tetronimo = new_Tetronimo(t_type);
				_tetronimo_counter[t_type]++;
				dequeue(&queue);
				enqueue(rand_T_Type(), &queue);
				// print_Queue(queue.head);
				
				_player_score += CalcScore(lines_cleared_this_turn, curr_level);
				_player_score += down_points;

				if (_lines_until_level <= 0){
					if (!_sfx_muted)
						SFX_PlaySFX(_sfx_levelup);
					curr_level++;
					// Set _lines_until_level
					GetLinesUntilNextLevel(curr_level);
					// Carry over extra lines
					if (carry){
						_lines_until_level -= carry;
					}
				}
				down_points = 0;
			}
		}

		_fps = elapsed;
		// Render Tetronimos	
		RenderBlocks(8, 0, window, renderer);
		// Render the UI elements
		RenderUI(20, 0, BLOCK_SIZE, buf, buf_max, curr_level, 
				window, renderer, texture, font);
		
		RenderStatsUI(3, 1, BLOCK_SIZE/1.8f, buf, buf_max,
				font, texture, window, renderer);
		RenderQueue(30, 5, BLOCK_SIZE/1.8f, buf, buf_max, 3,
				queue, font, texture, window, renderer);
		// Present the renderings to the screen
		SDL_RenderPresent(renderer); 
		// Calculate FPS
		uint64_t end = SDL_GetPerformanceCounter();
		elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
	}
	// Mix_FreeChunk(sfx_line_clear);
	QuitGame(window, renderer);
}
