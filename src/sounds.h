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
void SFX_PlayBGM();
void SFX_StopBGM(Mix_Music* bgm);
void SFX_MuteBGM();
void SFX_ResumeBGM();

void SFX_ChangeMusicVol(uint8_t vol);


#endif // SOUNDS_H
