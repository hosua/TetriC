#ifndef SOUNDS_H
#define SOUNDS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_audio.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void SFX_PlayBGM();
void SFX_StopBGM(Mix_Music* bgm);
void SFX_MuteBGM();
void SFX_ResumeBGM();

void SFX_MusicSetVol(uint8_t vol);

typedef struct SFX {
	Mix_Music* music;

	Mix_Chunk* lock_piece;
	Mix_Chunk* line_clear;
	Mix_Chunk* level_up;
	Mix_Chunk* tetris;
	Mix_Chunk* menu;
	Mix_Chunk* nudge;

	// If _should_nudge is true, nudge sound will play and then it will be unset for _nudge_delay milleseconds.
	bool should_nudge;
	bool muted;
	int8_t vol;
} SFX;

extern SFX* _sfx;

// Load all sfx into memory
void init_SFX(uint8_t bgl_vol);

void SFX_PlaySFX(Mix_Chunk* sfx);

void SFX_MusicVolUp();
void SFX_MusicVolDown();

#endif // SOUNDS_H
