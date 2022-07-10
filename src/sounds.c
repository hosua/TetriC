#include "sounds.h"

bool _sfx_muted = false;

uint8_t bgm_vol = 50;

void SFX_PlayBGM(){
	/* TODO: EXPERIMENTAL CODE */
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	// Mix_Chunk* sfx_line_clear = Mix_LoadWAV("sounds/arcade-sound.wav");
	Mix_Music* sfx_bgm = Mix_LoadMUS("sfx/trepak.mp3");
	Mix_PlayMusic(sfx_bgm, -1);
	Mix_VolumeMusic(bgm_vol);
	/* END EXPERIMENTAL CODE */
}
void SFX_MuteBGM(){
	Mix_VolumeMusic(0);
}
void SFX_ResumeBGM(){
	Mix_VolumeMusic(bgm_vol);
}

void SFX_ChangeMusicVol(uint8_t vol){
	bgm_vol = vol;
	SFX_ResumeBGM();
}

