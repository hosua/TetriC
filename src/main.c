#include "graphics.h"
#include "tetris.h"
#include "input.h"
#include "clock.h"
#include "queue.h"

#include "test.h"

// Variable to determine if a piece is still falling. If one is not falling, a new piece is spawned.
// and then this variable is set to true again.
bool is_falling = false;

int main(int argc, char **argv){
	// seed the RNG
	srand(time(0));

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
	
	SDL_Texture *texture = NULL;
	// init_test_2();	
	// The points accrued from holding down
	uint8_t down_points = 0;
	ClearScreen(window, renderer);
	// Tetronimo* tetronimo = new_Tetronimo(rand_T_Type());
	// Tetronimo* tetronimo = new_Tetronimo(T_I);
	
	// Queue
	Queue queue = init_Queue();

	// Fill queue with pieces
	for (int i = 0; i < _queue_limit; i++){
		enqueue(rand_T_Type(), &queue);
	}
	Tetronimo* tetronimo = new_Tetronimo(peek(queue));
	dequeue(&queue);
	enqueue(rand_T_Type(), &queue);

	// enqueue(T_O, &queue);
	// dequeue(&queue);
	// printf("QUEUE\n");
	
	// Char buffer for rendering text
	char buf[128];
	uint8_t buf_max = sizeof(buf);
	uint8_t lines_cleared_this_turn = 0;
	float elapsed = 0.0f;
	_fps = elapsed;

	GetLinesUntilNextLevel(_curr_level);
	for ( ; ; ){
		uint64_t start = SDL_GetPerformanceCounter();
		// Get user input
		SDL_Event event; 
		SDL_PollEvent(&event);
		ClearScreen(window, renderer);
		SetKeyArray(event, window);
		MovementHandler(event, window, renderer, tetronimo);

		
		if (InputTimer()){
			DownwardMovementHandler(&down_points, window, renderer, tetronimo);
		}

		if (LevelTimer(_curr_level)){
			is_falling = move_Tetronimo(window, renderer, tetronimo, M_DOWN);
			if (!is_falling){
				if (IsPlayerDead()){
					QuitGame(window);
				}
				lines_cleared_this_turn = CheckLines();
				_lines_until_level -= lines_cleared_this_turn;
				free(tetronimo);

				// dequeue a new piece from queue to spawn it, then enqueue another 
				T_Type t_type = peek(queue);
				tetronimo = new_Tetronimo(t_type);
				_tetronimo_counter[t_type]++;
				dequeue(&queue);
				enqueue(rand_T_Type(), &queue);
				// print_Queue(queue.head);
				
				_player_score += CalcScore(lines_cleared_this_turn, _curr_level);
				_player_score += down_points;

				if (_lines_until_level == 0){
					// Level up
					_curr_level++;
					GetLinesUntilNextLevel(_curr_level);
				}
				down_points = 0;
			}
		}

		_fps = elapsed;
		// Render the field
		// RenderPlayField(5, 1, window, renderer);
		// Render Tetronimos	
		RenderBlocks(8, 0, window, renderer);
		// Render the UI elements
		RenderUI(20, 0, BLOCK_SIZE, buf, buf_max, window,
				renderer, texture, font);
		
		RenderStatsUI(3, 1, BLOCK_SIZE/1.8f, buf, buf_max,
				font, texture, window, renderer);
		RenderQueue(30, 5, BLOCK_SIZE/1.8f, buf, buf_max, 3,
				queue, font, texture, window, renderer);
		// Present the renderings to the screen
		SDL_RenderPresent(renderer); 

		// This causes issues when unfocusing on Linux	
		// SDL_Delay(_tick);
		// sleep_us(16667);
		// Calculate FPS
		uint64_t end = SDL_GetPerformanceCounter();
		elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
}
