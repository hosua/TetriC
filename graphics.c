#include "tetris.h"
#include "graphics.h"

// Create and return a single rect
SDL_Rect GetRect(uint16_t x, uint16_t y, uint8_t block_size){
	SDL_Rect rect;
	rect.x = x * block_size;
	rect.y = y * block_size;

	rect.w = block_size;
	rect.h = block_size;
	return rect;
}

SDL_Rect* GetFieldLine(uint8_t x, uint8_t y, SDL_Window* window, SDL_Renderer* renderer){
	SDL_Rect* rect_arr = (SDL_Rect*)malloc(sizeof(SDL_Rect) * FIELD_X);
	for (int i = 0; i < FIELD_X; i++){
		rect_arr[i] = GetRect(x + (i+1), y, BLOCK_SIZE);	
	}
	return rect_arr;
}

void RenderPlayField(SDL_Window* window, SDL_Renderer* renderer){
	SDL_Rect* line;
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (int i = 0; i < FIELD_Y/2; i++){
		line = GetFieldLine(0, (i+1), window, renderer);
		SDL_RenderDrawRects(renderer, line, FIELD_X);
		free(line);
	}
	// SDL_RenderPresent(renderer);
}

void PrintPlayField(){
	for (int i = FIELD_Y/2; i < FIELD_Y; i++){
		for (int j = 0; j < FIELD_X; j++)
			printf("%01x", _play_field[i][j]);
		printf("\n");
	}
}

SDL_Rect* GetBlocksInLine(uint8_t y, size_t* num_blocks, SDL_Window* window, SDL_Renderer* renderer){
	SDL_Rect* blocks = (SDL_Rect*)malloc(sizeof(SDL_Rect) * FIELD_X);
	for (int i = 0; i < FIELD_X; i++){
		blocks[(*num_blocks)++] = GetRect((i+1), (y+1) - FIELD_Y/2, BLOCK_SIZE);	
	}
	return blocks;
}

void RenderBlocks(SDL_Window* window, SDL_Renderer* renderer){
	SDL_Rect* line;
	size_t num_blocks = 0;
	// Lines 0-19 inclusive in the _play_field are off-screen and do not get rendered.
	for (int y = FIELD_Y/2; y < FIELD_Y; y++){
		line = GetBlocksInLine(y, &num_blocks, window, renderer);
		for (int x = 0; x < num_blocks; x++){
			T_Type t_type = _play_field[y][x];
			SetRenderColorByType(t_type, renderer);
			if (t_type == T_NONE)
				SDL_RenderDrawRect(renderer, &line[x]);
			else
				SDL_RenderFillRect(renderer, &line[x]);
		}
		num_blocks = 0;
		free(line);
	}
}

void SetRenderColorByType(T_Type t_type, SDL_Renderer* renderer){
	switch (t_type){
		case T_NONE:
			// printf("T_NONE\n");
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			break;
		case T_I:
			// Blue
			SDL_SetRenderDrawColor(renderer, 24, 102, 204, 255);
			break;
		case T_O:
			// Yellow
			SDL_SetRenderDrawColor(renderer, 208, 255, 0, 255);
			break;
		case T_S:
			// Green
			SDL_SetRenderDrawColor(renderer, 47, 247, 32, 255);
			break;
		case T_Z:
			// Red
			SDL_SetRenderDrawColor(renderer, 179, 2, 2, 255);
			break;
		case T_L:
			// Orange
			SDL_SetRenderDrawColor(renderer, 255, 187, 0, 255);
			break;
		case T_J:
			// Teal 
			SDL_SetRenderDrawColor(renderer, 4, 224, 224, 255);
			break;
		case T_T:
			// Pink
			SDL_SetRenderDrawColor(renderer, 255, 71, 252, 255);
			break;
		default:
			// printf("INVALID\n");
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			fprintf(stderr, "Error: Invalid or unimplemented T_Type\n");
		break;
	}
}

void ClearScreen(SDL_Window* window, SDL_Renderer* renderer){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}


// Text stuff
void RenderText(SDL_Renderer *renderer, int x, int y, char *text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect) {
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}


void RenderStats(char* buf, uint8_t buf_max, uint16_t x, uint16_t y, SDL_Renderer* renderer,
		SDL_Texture* texture, TTF_Font* font){
	SDL_Rect rect;
	snprintf(buf, buf_max,
			"Tetronimo Counter");
	RenderText(renderer, x, y, buf, font, &texture, &rect);
	SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_DestroyTexture(texture);

	for (uint8_t t_type = T_O; t_type <= T_T; t_type++){
		snprintf(buf, buf_max,
				"%s:", T_Type_to_str(t_type));
		RenderText(renderer, x, y + (BLOCK_SIZE * t_type), buf, font, &texture, &rect);
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		SDL_DestroyTexture(texture);

		snprintf(buf, buf_max,
				"%i", _piece_counter[t_type]);
		RenderText(renderer, (x + BLOCK_SIZE), y + (BLOCK_SIZE * t_type), buf, font, &texture, &rect);
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		SDL_DestroyTexture(texture);
	}
}

void RenderUI(char* buf, uint8_t buf_max, SDL_Window* window, SDL_Renderer* renderer, 
		SDL_Texture* texture, TTF_Font* font){
	SDL_Rect rect;
	// After rendering a texture, you must always destroy it, otherwise it will leak memory
	snprintf(buf, buf_max,
			"Level: %i", _curr_level);
	RenderText(renderer, (BLOCK_SIZE * 12), (BLOCK_SIZE * 1), buf, font, &texture, &rect);
	SDL_DestroyTexture(texture);

	snprintf(buf, buf_max,
			"Score: %i", _player_score);
	RenderText(renderer, (BLOCK_SIZE * 12), (BLOCK_SIZE * 2), buf, font, &texture, &rect);
	SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_DestroyTexture(texture);

	snprintf(buf, buf_max,
			"Lines until next level: %i", _lines_until_level);
	RenderText(renderer, (BLOCK_SIZE * 12), (BLOCK_SIZE * 3), buf, font, &texture, &rect);
	SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_DestroyTexture(texture);

	snprintf(buf, buf_max,
			"Total lines cleared: %i", _lines_cleared);
	RenderText(renderer, (BLOCK_SIZE * 12), (BLOCK_SIZE * 4), buf, font, &texture, &rect);
	SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_DestroyTexture(texture);

	snprintf(buf, buf_max,
			"FPS: %f", 1.0f/_fps);
	RenderText(renderer, (BLOCK_SIZE * 12), (BLOCK_SIZE * 20), buf, font, &texture, &rect);
	SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_DestroyTexture(texture);
}
