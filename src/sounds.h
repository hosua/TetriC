#ifndef SOUNDS_H
#define SOUNDS_H

#define SOUND_LINE_CLEAR "sounds/arcade-sound.wav"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_audio.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

extern bool _sfx_muted;
void SFX_PlayBGM(const char* bgm_path, int8_t bgm_vol);
void SFX_StopBGM(Mix_Music* bgm);
void SFX_MuteBGM();
void SFX_ResumeBGM();

void SFX_MusicVol(uint8_t vol);

// If _should_nudge is true, nudge sound will play and then it will be unset for _nudge_delay milleseconds.
extern bool _should_nudge;

#define NUM_SOUNDS 4

extern Mix_Chunk* _sfx_lockpiece; 
extern Mix_Chunk* _sfx_tetris; 
extern Mix_Chunk* _sfx_menu; 
extern Mix_Chunk* _sfx_lineclear;
extern Mix_Chunk* _sfx_levelup;
extern Mix_Chunk* _sfx_nudge;

// Load all sfx into memory
void SFX_LoadSoundBank();

void SFX_PlaySFX(Mix_Chunk* sfx);
// Free sfx pointers from memory
void SFX_FreeSFX();
void SFX_MusicVolUp();
void SFX_MusicVolDown();

#endif // SOUNDS_H
