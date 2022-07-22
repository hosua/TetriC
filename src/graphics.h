#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include "tetris.h"
#include "queue.h"

#define FONT_PATH "fonts/pixel-letters.ttf"
#define FONT_SIZE 30
#define UNIT 15

typedef struct GFX {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Rect *rect;
	SDL_Texture *texture;
	TTF_Font* font;
} GFX;

extern GFX* _gfx;

void init_Font();
void init_GFX();

SDL_Rect GFX_GetRect(uint16_t x, uint16_t y, uint8_t block_size);
// Gets 10 rects to draw the play_field
SDL_Rect* GFX_GetPlayfieldLine(uint16_t dx, uint16_t dy, uint16_t y, size_t* num_blocks);

void PrintPlayField();

// Renders the each block in the play field 
void GFX_RenderBlocks(uint16_t dx, uint16_t dy);
// Sets the SDL Renderer to the proper color before rendering a tetronimo
void GFX_SetRenderColorByType(T_Type t_type);

SDL_Rect* GFX_GetTetronimoByOrigin(uint16_t x, uint16_t y, uint8_t block_size, Tetronimo* tetronimo);

void GFX_ClearScreen();

void GFX_RenderText(int x, int y, int *tw, int *th, char *text, 
		SDL_Color color, SDL_Rect* rect);

void GFX_RenderUI(uint16_t dx, uint16_t dy, uint8_t block_size, 
		char* buf, uint8_t buf_max, uint8_t curr_level);

void GFX_RenderHelp(uint16_t dx, uint16_t dy, uint8_t block_size, 
		char* buf, uint8_t buf_max);

void GFX_RenderQueue(uint8_t dx, uint8_t dy, uint8_t block_size, 
		char* buf, size_t buf_max, uint8_t num_to_display, Queue queue);

void GFX_RenderStatsUI(uint8_t dx, uint8_t dy, uint8_t block_size, 
		char* buf, size_t buf_max);

Tetronimo* GFX_DrawTetronimo(T_Type t_type, uint16_t x, uint16_t y);

/* Button action */
typedef enum B_Type {
	B_PLAY, 
	B_MAINMENU, 
	B_LEVELSELECT, // The button to get to the level select screen
	B_LEVEL, // The numbered buttons on the level select screen
	B_SETTINGS, // TODO implement settings
	B_QUIT,
} B_Type;

// Button mouse events
typedef enum BMOUSE_Event{
	BMOUSE_OUT, // Mouse moved outside of button
	BMOUSE_OVER, // Mouse moved over button
	BMOUSE_DOWN, // Mouse was over button and clicked down
	BMOUSE_UP, // Mouse was over button and let go of mousedown input
} BMOUSE_Event;

const char* BMOUSE_Event_to_str(BMOUSE_Event bm_event);


typedef struct Button {
	// renderer color
	SDL_Color color; 
    // The attributes of the button
    SDL_Rect box;
    // The part of the button sprite sheet that will be shown
    SDL_Rect* clip;
    // int x, y, w, h;
	// Origin is top left of button
	SDL_Point origin;
	B_Type type; // Button type
	BMOUSE_Event mouse_event;
	char* buf;
	size_t buf_max;
} Button;

Button init_Button(int w, int h, // width and height
					int ox, int oy, // Origin x and y
					char* buf, size_t buf_max,
					SDL_Event* event, B_Type action); // The button's action type

void set_Button(int x, int y, Button* button); // Set button to (x,y) coordinates
											   
uint8_t GFX_GetLevelButtonLevel(Button* button);
void GFX_SetButtonColor(Button* button);

void GFX_RenderButton(Button* button);

/* Menu stuff below */

void GFX_RenderMainMenu(SDL_Event event, char* buf, size_t buf_max);

void GFX_RenderLevelSelect(SDL_Event event, char* buf, size_t buf_max);

void GFX_RenderGameover(SDL_Event event, char* buf, size_t buf_max);

#endif // GRAPHICS_H
