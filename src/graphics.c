#include "graphics.h"
#include "input.h"

// TODO: https://lazyfoo.net/SDL_tutorials/lesson09/index.php

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

void PrintPlayField(){
	for (int y = FIELD_Y/2; y < FIELD_Y; y++){
		for (int x = 0; x < FIELD_X; x++)
			printf("%01x", _game_data->play_field[y][x]);
		printf("\n");
	}
}

SDL_Rect* GFX_GetPlayfieldLine(uint16_t dx, uint16_t dy, uint16_t y, size_t* num_blocks){
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

// Set render color by tetronimo type
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
			// fprintf(stderr, "Error: Invalid or unimplemented T_Type\n");
			break;
	}
}

const char* BMOUSE_Event_to_str(BMOUSE_Event bm_event){
	switch(bm_event){
		case BMOUSE_OUT: return "BMOUSE_OUT"; break;
		case BMOUSE_OVER: return "BMOUSE_OVER"; break;
		case BMOUSE_DOWN: return "BMOUSE_DOWN"; break;
		case BMOUSE_UP: return "BMOUSE_DOWN"; break;
	}
	return "BMOUSE_ERROR";
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
		line = GFX_GetPlayfieldLine(dx, dy, y, &num_blocks);
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
void GFX_RenderText(int x, int y, int *tw, int *th, char *text, 
		SDL_Color color, SDL_Rect* rect){
	SDL_Surface *surface;
	SDL_SetRenderDrawColor(_gfx->renderer, color.r, color.g, color.b, color.a);
	surface = TTF_RenderText_Solid(_gfx->font, text, color);
	_gfx->texture = SDL_CreateTextureFromSurface(_gfx->renderer, surface);
	*tw = surface->w;
	*th = surface->h;
	SDL_FreeSurface(surface);
	rect->x = x;
	rect->y = y;
	rect->w = *tw;
	rect->h = *th;

	SDL_RenderCopy(_gfx->renderer, _gfx->texture, NULL, rect);
	SDL_DestroyTexture(_gfx->texture);
}

void GFX_RenderImage(uint16_t x, uint16_t y, const char* img_path){
	SDL_Texture* img = NULL;
	img = IMG_LoadTexture(_gfx->renderer, img_path);
	int w, h;
	SDL_QueryTexture(img, NULL, NULL, &w, &h); // get the width and height of the texture
	SDL_Rect rect; rect.x = x; rect.y = y; 
	rect.w = w; rect.h = h;
	SDL_RenderCopy(_gfx->renderer, img, NULL, &rect);
	SDL_DestroyTexture(img);
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
	int tw, th; // text width & height
	SDL_Rect rect;
	SDL_Color color = {255, 255, 255, 255};
	SDL_Rect* rects = NULL;
	SDL_SetRenderDrawColor(_gfx->renderer, 255, 255, 255, 255);
	// Lines 0-19 inclusive in the _game_data->play_field are off-screen and do not get rendered.
	snprintf(buf, buf_max, "Next");
	GFX_RenderText((dx + 8) * block_size, (dy + 5.5f) * block_size, 
			&tw, &th,
			buf, 
			color, &rect);

	snprintf(buf, buf_max, "Piece");
	GFX_RenderText((dx + 8) * block_size, (dy + 7) * block_size, 
			&tw, &th,
			buf, 
			color, &rect);

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
	SDL_Color color = {255, 255, 255, 255};
	int tw, th;
	SDL_Rect rect;
	SDL_Rect* rects = NULL;
	SDL_SetRenderDrawColor(_gfx->renderer, 255, 255, 255, 255);
	snprintf(buf, buf_max, "Tetronimo");
	GFX_RenderText(dx * block_size, dy * block_size * 1.5f, 
			&tw, &th,
			buf, 
			color, &rect);	

	snprintf(buf, buf_max, "Counter");
	GFX_RenderText(dx * block_size, dy * 3.5f * block_size, 
			&tw, &th,
			buf, 
			color, &rect);

	uint8_t y;
	for (int t_type = T_O; t_type <= T_T; t_type++){
		y = (t_type * 2);
		tetronimo = GFX_DrawTetronimo(t_type, dx, dy + y);
		GFX_SetRenderColorByType(t_type);
		rects = GFX_GetTetronimoByOrigin(dx, dy + y, block_size, tetronimo);
		SDL_RenderFillRects(_gfx->renderer, rects, TETRA);
		snprintf(buf, buf_max,
				"%i", _game_data->tetronimo_counter[t_type]);

		GFX_RenderText((dx * 3) * block_size, (dy + t_type - 0.4f) * block_size * 4.0f, 
				&tw, &th,
				buf, 
				color, &rect);
		free(tetronimo);
		free(rects);
	}
}

// void RenderGhost(uint8_t dx, uint8_t dy, 
// 		SDL_Window* _gfx->window, SDL_Renderer* _gfx->renderer);  
void GFX_RenderUI(uint16_t dx, uint16_t dy, uint8_t block_size, char* buf, uint8_t buf_max, uint8_t curr_level){
	SDL_Rect rect;
	SDL_Color color = {255, 255, 255, 255};
	int tw, th;
	// After rendering a _gfx->texture, you must always destroy it, otherwise it will leak memory
	snprintf(buf, buf_max, "Level: %i", curr_level);
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 1), 
			&tw, &th,
			buf, 
			color, &rect);

	snprintf(buf, buf_max, "Score: %i", _game_data->player_score);
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 2), 
			&tw, &th,
			buf, 
			color, &rect);

	snprintf(buf, buf_max, "Lines until next level: %i", _game_data->lines_until_level);
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 3), 
			&tw, &th,
			buf, 
			color, &rect);

	snprintf(buf, buf_max,
			"Total lines cleared: %i", _game_data->lines_cleared);
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 4), 
			&tw, &th,
			buf, 
			color, &rect);

	snprintf(buf, buf_max,
			"FPS: %f", 1.0f/_game_data->fps);
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 20), 
			&tw, &th,
			buf, 
			color, &rect);
}

void GFX_RenderHelp(uint16_t dx, uint16_t dy, uint8_t block_size, char* buf, uint8_t buf_max){
	int tw, th;
	SDL_Rect rect;
	SDL_Color color = {255, 255, 255, 255};
	// After rendering a _gfx->texture, you must always destroy it, otherwise it will leak memory
	snprintf(buf, buf_max, "CONTROLS");
	GFX_RenderText((dx * block_size), (dy * block_size), 
			&tw, &th,
			buf, 
			color, &rect);

	snprintf(buf, buf_max, "arrow keys: movement");
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 1), 
			&tw, &th,
			buf, 
			color, &rect);

	snprintf(buf, buf_max, "z x: rotate");
	GFX_RenderText((dx * block_size), (dy * block_size) + (block_size * 2), 
			&tw, &th,
			buf, 
			color, &rect);

	if (_sfx->muted){
		snprintf(buf, buf_max, "m: unmute sound");
	} else {
		snprintf(buf, buf_max, "m: mute sound");
	}
	GFX_RenderText((dx  * block_size), (dy * block_size) + (block_size * 3),  
			&tw, &th,
			buf, 
			color, &rect);
}

/* All code below this point is Menu rendering related stuff */

// Set the position of the button
void set_Button(int x, int y, Button* button){
	button->origin.x = x, button->origin.y = y;
}

Button init_Button(int x, int y,
		// int w, int h, // width and height
		int ox, int oy, // Origin's x and y
		char* buf, size_t buf_max, // our message buffer and its length
		SDL_Event* event, B_Type b_type){ // The button type
	// Default font color is white
	// SDL_Color color = {255, 255, 255, 255};
	Button button;
	button.box.x = x, button.box.y = y;
	button.origin.x = ox, button.origin.y = oy;
	set_Button(x, y, &button);
	button.buf = buf;
	button.buf_max = buf_max;
	button.color.r = 255;
	button.color.g = 255;
	button.color.b = 255;
	button.color.a = 255;
	// TODO: Need to set button mouse_event appropriately here
	// Input_HandleInitButtonEvents(event, &button);
	// GFX_SetButtonColor(&button);
	// button.color = color;
	// Though calling render text in the initialization will only display it for a negligible amount of time, the main purpose
	// for doing this in the constructor is to automatically set the width and the height to the Button's box attribute.
	// This lets us initialize buttons automatically to the size of the text instead having to hard-code the width and the height.
	GFX_RenderText(button.origin.x, button.origin.y,
					&button.box.w, &button.box.h,
					button.buf,
					button.color,
					&button.box);
	// button.box.w = w, button.box.h = h;
	button.type = b_type;	
	button.clip = (SDL_Rect*)malloc(sizeof(SDL_Rect));

	return button;
}

void GFX_SetButtonColor(Button* button){
	BMOUSE_Event button_mouse_event = button->mouse_event;
	switch(button_mouse_event){
		case BMOUSE_OUT:
			button->color.r = 255;
			button->color.g = 255;
			button->color.b = 255;
			button->color.a = 255;
			break;
		case BMOUSE_OVER:
			button->color.r = 128;
			button->color.g = 128;
			button->color.b = 128;
			button->color.a = 255;
			break;
		case BMOUSE_UP:
			button->color.r = 128;
			button->color.g = 128;
			button->color.b = 128;
			button->color.a = 255;
			break;
		case BMOUSE_DOWN:
			button->color.r = 69;
			button->color.g = 69;
			button->color.b = 69;
			button->color.a = 255;
			break;
	}
}

// Shows the button on the screen
void GFX_RenderButton(Button* button){
	int tw = 0, th = 0;
	BMOUSE_Event button_mouse_event = button->mouse_event;
	// if (button->mouse_event == BMOUSE_OVER){
	// printf("Color: (%i,%i,%i)\n", button->color.r, button->color.g, button->color.b);
	// }
	// GFX_SetButtonColor(button);
	SDL_SetRenderDrawColor(_gfx->renderer, button->color.r, button->color.g, button->color.b, button->color.a);
	GFX_RenderText(button->origin.x, button->origin.y,
			&tw, &th,
			button->buf,
			button->color,
			&button->box);
	SDL_RenderDrawRect(_gfx->renderer, &button->box);
}

void GFX_RenderMainMenu(SDL_Event event, char* buf, size_t buf_max){
	SDL_Rect rect;
	SDL_Color color = {255, 255, 255, 255};

	GFX_RenderImage(SCREEN_X/4, 0, 
			"img/TetriC-logo.png");

	// Clicking play brings us to the G_LEVELSELECT state.
	// Selecting a level brings us to G_PLAY state.
	snprintf(buf, buf_max, "Play");
	Button play_button = init_Button(50, 25, 
			SCREEN_X, SCREEN_Y, 
			buf, buf_max,
			&event, B_LEVELSELECT);

	set_Button((SCREEN_X*9)/20, SCREEN_Y/5, &play_button);
	GFX_RenderButton(&play_button);
	Input_HandleMouseEvents(&event, &play_button);

	snprintf(buf, buf_max, "Quit");
	Button quit_button = init_Button(50, 25, 
			SCREEN_X, SCREEN_Y, 
			buf, buf_max,
			&event, B_QUIT);

	set_Button((SCREEN_X*9)/20, SCREEN_Y/3, &quit_button);
	GFX_RenderButton(&quit_button);
	Input_HandleMouseEvents(&event, &quit_button);
}

void GFX_RenderGameover(SDL_Event event, char* buf, size_t buf_max){
	int tw, th;
	SDL_Rect rect;
	SDL_Color color = {255, 255, 255, 255};

	snprintf(buf, buf_max, "Game Over!");
	GFX_RenderText((SCREEN_X*7/20), (SCREEN_Y/5.5f), 
			&tw, &th,
			buf, 
			color, &rect);

	snprintf(buf, buf_max, "You cleared %i lines before losing.", _game_data->lines_cleared);
	GFX_RenderText((SCREEN_X*5/20), (SCREEN_Y/4), 
			&tw, &th,
			buf, 
			color, &rect);

	snprintf(buf, buf_max, "Your final score was: %i", _game_data->player_score);
	GFX_RenderText((SCREEN_X*5)/20, (SCREEN_Y/3), 
			&tw, &th,
			buf, 
			color, &rect);

	snprintf(buf, buf_max, "Go back to main menu");
	Button main_menu_button = init_Button(150, 50, 
			SCREEN_X, SCREEN_Y, 
			buf, buf_max,
			&event, B_MAINMENU);
	set_Button((SCREEN_X*6)/20, SCREEN_Y/2, &main_menu_button);
	GFX_RenderButton(&main_menu_button);
	Input_HandleMouseEvents(&event, &main_menu_button);
}

uint8_t GFX_GetLevelButtonLevel(Button* button){
	return atoi(button->buf);	
}

void GFX_RenderLevelSelect(SDL_Event event, char* buf, size_t buf_max){
	int tw, th;
	SDL_Rect rect;
	SDL_Color color = {255, 255, 255, 255};
	snprintf(buf, buf_max, "Select a level.");
	GFX_RenderText((SCREEN_X/2), (SCREEN_Y/5), 
			&tw, &th,
			buf, 
			color, &rect);

	for (int level = 0; level <= 20; level++){
		int dx = 0, dy = 0;
		if (level <= 10) {
			dx = 0;
		} else {
			dx = 2;
			dy = 10;
		}
		snprintf(buf, buf_max, "%i", level);
		Button level_button = init_Button(50, 25,
				SCREEN_X, SCREEN_Y,
				buf, buf_max,
				&event, B_LEVEL);
		set_Button((SCREEN_X*(6+dx))/20, 25 * (level+1-dy), &level_button);
		GFX_RenderButton(&level_button);
		Input_HandleMouseEvents(&event, &level_button);
	}

	snprintf(buf, buf_max, "Go back to main menu");
	Button main_menu_button = init_Button(150, 50, 
			SCREEN_X, SCREEN_Y, 
			buf, buf_max,
			&event, B_MAINMENU);
	set_Button((SCREEN_X*10)/20, SCREEN_Y/2, &main_menu_button);
	GFX_RenderButton(&main_menu_button);
	Input_HandleMouseEvents(&event, &main_menu_button);
}

