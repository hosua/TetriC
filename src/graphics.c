#include "graphics.h"

GFX* _gfx = NULL;

void init_Font(){
	TTF_Init();
	_gfx->font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
	if (!_gfx->font){
		fprintf(stderr, "Error: Font not found\n");
		exit(EXIT_FAILURE);
	}
}

// Since init_Font() must occur after init_GFX(), we will just do it in here so we don't fuck it up later on
void init_GFX(){
	_gfx = (GFX*)malloc(sizeof(GFX));
	_gfx->window = SDL_CreateWindow("TetriC", 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			SCREEN_X, SCREEN_Y, 0);
	_gfx->renderer = SDL_CreateRenderer(_gfx->window, -1, SDL_RENDERER_ACCELERATED);

	_gfx->rect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	_gfx->texture = NULL;

	init_Font();
}

// Create and return a single rect
SDL_Rect GFX_GetRect(uint16_t x, uint16_t y, uint8_t block_size){
	 SDL_Rect rect;
	rect.x = x * block_size;
	rect.y = y * block_size;

	rect.w = block_size;
	rect.h = block_size;
	return rect;
}

SDL_Rect* GFX_GetFieldLine(uint8_t x, uint8_t y){
	SDL_Rect* rect_arr = (SDL_Rect*)malloc(sizeof(SDL_Rect) * FIELD_X);
	for (int x = 0; x < FIELD_X; x++){
		rect_arr[x] = GFX_GetRect(x + (x+1), y, BLOCK_SIZE);	
	}
	return rect_arr;
}

void PrintPlayField(){
	for (int y = FIELD_Y/2; y < FIELD_Y; y++){
		for (int x = 0; x < FIELD_X; x++)
			printf("%01x", _game_data->play_field[y][x]);
		printf("\n");
	}
}

SDL_Rect* GFX_GetBlocksInLine(uint16_t dx, uint16_t dy, uint16_t y, size_t* num_blocks){
	SDL_Rect* blocks = (SDL_Rect*)malloc(sizeof(SDL_Rect) * FIELD_X);
	for (int i = 0; i < FIELD_X; i++)
		blocks[(*num_blocks)++] = GFX_GetRect((dx+i+1), (dy+y+1) - FIELD_Y/2, BLOCK_SIZE);	
	return blocks;
}

SDL_Rect* GFX_GetTetronimoByOrigin(uint16_t x, uint16_t y, uint8_t block_size, Tetronimo* tetronimo){
	SDL_Rect *blocks = (SDL_Rect*)malloc(sizeof(SDL_Rect) * TETRA);
	for (int i = 0; i < TETRA; i++)
		blocks[i] = GFX_GetRect(x + tetronimo->pieces[i].x, y + tetronimo->pieces[i].y, block_size);
	return blocks;
}

void GFX_SetRenderColorByType(T_Type t_type){
	switch (t_type){
		case T_NONE:
			// printf("T_NONE\n");
			SDL_SetRenderDrawColor(_gfx->renderer, 255, 255, 255, 255);
			break;
		case T_I:
			// Blue
			SDL_SetRenderDrawColor(_gfx->renderer, 24, 102, 204, 255);
			break;
		case T_O:
			// Yellow
			SDL_SetRenderDrawColor(_gfx->renderer, 208, 255, 0, 255);
			break;
		case T_S:
			// Green
			SDL_SetRenderDrawColor(_gfx->renderer, 47, 247, 32, 255);
			break;
		case T_Z:
			// Red
			SDL_SetRenderDrawColor(_gfx->renderer, 179, 2, 2, 255);
			break;
		case T_L:
			// Orange
			SDL_SetRenderDrawColor(_gfx->renderer, 255, 187, 0, 255);
			break;
		case T_J:
			// Teal 
			SDL_SetRenderDrawColor(_gfx->renderer, 4, 224, 224, 255);
			break;
		case T_T:
			// Pink
			SDL_SetRenderDrawColor(_gfx->renderer, 255, 71, 252, 255);
			break;
		default:
			// printf("INVALID\n");
			SDL_SetRenderDrawColor(_gfx->renderer, 255, 255, 255, 255);
			fprintf(stderr, "Error: Invalid or unimplemented T_Type\n");
		break;
	}
}

void GFX_ClearScreen(){
	SDL_SetRenderDrawColor(_gfx->renderer, 0, 0, 0, 255);
	SDL_RenderClear(_gfx->renderer);
}


void GFX_RenderBlocks(uint16_t dx, uint16_t dy){
	SDL_Rect* line;
	size_t num_blocks = 0;
	// Lines 0-19 inclusive in the _game_data->play_field are off-screen and do not get rendered.
	for (int y = FIELD_Y/2; y < FIELD_Y; y++){
		line = GFX_GetBlocksInLine(dx, dy, y, &num_blocks);
		for (int x = 0; x < num_blocks; x++){
			T_Type t_type = _game_data->play_field[y][x];
			GFX_SetRenderColorByType(t_type);
			if (t_type == T_NONE)
				SDL_RenderDrawRect(_gfx->renderer, &line[x]);
			else
				SDL_RenderFillRect(_gfx->renderer, &line[x]);
		}
		num_blocks = 0;
		free(line);
	}
}

// Text stuff
void GFX_RenderText(int x, int y, char *text, SDL_Rect* rect){
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    surface = TTF_RenderText_Solid(_gfx->font, text, textColor);
    _gfx->texture = SDL_CreateTextureFromSurface(_gfx->renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

Tetronimo* GFX_DrawTetronimo(T_Type t_type, uint16_t x, uint16_t y){
	Tetronimo* new_piece = (Tetronimo*)malloc(sizeof(Tetronimo));
	new_piece->t_type = t_type;
	new_piece->d_rot = D_90;
	new_piece->origin.x = x;
	new_piece->origin.y = y;
	set_Tetronimo(new_piece);
	return new_piece;
}

// If num_to_display = 0, render all in queue
void GFX_RenderQueue(uint8_t dx, uint8_t dy, uint8_t block_size, char* buf, size_t buf_max, uint8_t num_to_display, Queue queue){ 	
	Tetronimo* tetronimo = NULL;
	SDL_Rect rect;
	SDL_Rect* rects = NULL;
	SDL_SetRenderDrawColor(_gfx->renderer, 255, 255, 255, 255);
	// Lines 0-19 inclusive in the _game_data->play_field are off-screen and do not get rendered.
	snprintf(buf, buf_max, "Next");
	GFX_RenderText((dx + 8) * block_size, (dy + 5.5f) * block_size, buf, &rect);
	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
	SDL_DestroyTexture(_gfx->texture);

	snprintf(buf, buf_max, "Piece");
	GFX_RenderText((dx + 8) * block_size, (dy + 7) * block_size, buf, &rect);
	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
	SDL_DestroyTexture(_gfx->texture);

	uint8_t y;
	T_Type t_type;
	if (num_to_display){ // show n pieces in the queue
		for (int i = 0; i < num_to_display; i++, queue.head = queue.head->next){
			y = ((i+1) * 2);
			t_type = queue.head->t_type;
			tetronimo = GFX_DrawTetronimo(t_type, (dx * 2)/ 6, dy + y);
			GFX_SetRenderColorByType(t_type);
			rects = GFX_GetTetronimoByOrigin(dx, dy + y, block_size, tetronimo);
			SDL_RenderFillRects(_gfx->renderer, rects, TETRA);
			free(tetronimo);
			free(rects);
		}
	} else { // Show all in queue
		for (int i = 0 ; queue.head; queue.head = queue.head->next, i++){
			y = ((i+1) * 2);
			t_type = queue.head->t_type;
			tetronimo = GFX_DrawTetronimo(t_type, dx, dy + y);
			GFX_SetRenderColorByType(t_type);
			rects = GFX_GetTetronimoByOrigin(dx, dy + y, block_size, tetronimo);
			SDL_RenderFillRects(_gfx->renderer, rects, TETRA);
			free(tetronimo);
			free(rects);
		}
	}
}

void GFX_RenderStatsUI(uint8_t dx, uint8_t dy, uint8_t block_size, char* buf, size_t buf_max){
	Tetronimo* tetronimo = NULL;
	SDL_Rect rect;
	SDL_Rect* rects = NULL;
	SDL_SetRenderDrawColor(_gfx->renderer, 255, 255, 255, 255);
	
	snprintf(buf, buf_max, "Tetronimo");
	GFX_RenderText(dx * block_size, dy * block_size * 1.5f, buf, &rect);	
	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
	SDL_DestroyTexture(_gfx->texture);

	snprintf(buf, buf_max, "Counter");
	GFX_RenderText(dx * block_size, dy * 3.5f * block_size, buf, &rect);
	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
	SDL_DestroyTexture(_gfx->texture);

	uint8_t y;
	for (int t_type = T_O; t_type <= T_T; t_type++){
		y = (t_type * 2);
		tetronimo = GFX_DrawTetronimo(t_type, dx, dy + y);
		GFX_SetRenderColorByType(t_type);
		rects = GFX_GetTetronimoByOrigin(dx, dy + y, block_size, tetronimo);
		SDL_RenderFillRects(_gfx->renderer, rects, TETRA);
		snprintf(buf, buf_max,
				"%i", _game_data->tetronimo_counter[t_type]);

		GFX_RenderText((dx * 3) * block_size, (dy + t_type - 0.4f) * block_size * 4.0f, buf, &rect);
		SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
		SDL_DestroyTexture(_gfx->texture);
		free(tetronimo);
		free(rects);
	}
}

// void RenderGhost(uint8_t dx, uint8_t dy, 
// 		SDL_Window* _gfx->window, SDL_Renderer* _gfx->renderer);  
void GFX_RenderUI(uint16_t dx, uint16_t dy, uint8_t block_size, char* buf, uint8_t buf_max, uint8_t curr_level){
	SDL_Rect rect;
	// After rendering a _gfx->texture, you must always destroy it, otherwise it will leak memory
	snprintf(buf, buf_max, "Level: %i", curr_level);
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 1), buf, &rect);
	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
	SDL_DestroyTexture(_gfx->texture);

	snprintf(buf, buf_max, "Score: %i", _game_data->player_score);
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 2), buf, &rect);
	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
	SDL_DestroyTexture(_gfx->texture);

	snprintf(buf, buf_max, "Lines until next level: %i", _game_data->lines_until_level);
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 3), buf, &rect);
	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
	SDL_DestroyTexture(_gfx->texture);

	snprintf(buf, buf_max,
			"Total lines cleared: %i", _game_data->lines_cleared);
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 4), buf, &rect);
	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
	SDL_DestroyTexture(_gfx->texture);

	snprintf(buf, buf_max,
			"FPS: %f", 1.0f/_game_data->fps);
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 20), buf, &rect);
	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
	SDL_DestroyTexture(_gfx->texture);
}

void GFX_RenderHelp(uint16_t dx, uint16_t dy, uint8_t block_size, char* buf, uint8_t buf_max){
	SDL_Rect rect;
	// After rendering a _gfx->texture, you must always destroy it, otherwise it will leak memory
	snprintf(buf, buf_max, "CONTROLS");
	GFX_RenderText((dx * block_size), (dy * block_size), buf, &rect);
	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
	SDL_DestroyTexture(_gfx->texture);

	snprintf(buf, buf_max, "arrow keys: movement");
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 1), buf, &rect);
	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
	SDL_DestroyTexture(_gfx->texture);

	snprintf(buf, buf_max, "z x: rotate");
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 2), buf, &rect);
	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
	SDL_DestroyTexture(_gfx->texture);

	if (_sfx->muted){
		snprintf(buf, buf_max, "m: unmute sound");
	} else {
		snprintf(buf, buf_max, "m: mute sound");
	}
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 3),  buf, &rect);
	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, &rect);
	SDL_DestroyTexture(_gfx->texture);
}
