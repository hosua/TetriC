#include "sounds.h"

SFX* _sfx = NULL;

void SFX_PlayBGM(){
	// Open audio device
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_PlayMusic(_sfx->music, -1);
	Mix_VolumeMusic(_sfx->vol);
}

void SFX_MuteBGM(){
	Mix_VolumeMusic(0);
}

void SFX_ResumeBGM(){
	Mix_VolumeMusic(_sfx->vol);
}

void SFX_MusicSetVol(uint8_t vol){
	_sfx->vol = vol;
	SFX_ResumeBGM();
}

void SFX_MusicVolUp(){
	_sfx->vol++;
	if (_sfx->vol >= 100)
		_sfx->vol = 100;
}
void SFX_MusicVolDown(){
	_sfx->vol--;
	if (_sfx->vol <= 0)
		_sfx->vol = 0;
}

void init_SFX(uint8_t vol){
	_sfx = (SFX*)malloc(sizeof(SFX));

	// Open audio device
    if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1){
		fprintf(stderr, "Error: Failed to open audio device\n");
		exit(EXIT_FAILURE);
	}
	// Load music into memory
	_sfx->music = Mix_LoadMUS("sfx/trepak.mp3");
	// Load all sound effects into memory
	_sfx->line_clear = Mix_LoadWAV("sfx/line-clear.wav");
	_sfx->level_up = Mix_LoadWAV("sfx/level-up.wav");
	_sfx->lock_piece = Mix_LoadWAV("sfx/lock-piece.wav");
	_sfx->tetris =  Mix_LoadWAV("sfx/tetris-sound.wav");
	_sfx->menu = Mix_LoadWAV("sfx/menu-sound.wav");
	_sfx->nudge = Mix_LoadWAV("sfx/nudge.wav");

	_sfx->vol = vol;

	if (!_sfx->music || !_sfx->line_clear || !_sfx->level_up || !_sfx->lock_piece 
			|| !_sfx->tetris || !_sfx->menu || !_sfx->nudge){
		fprintf(stderr, "Error: There was an error loading an sfx file: %s\n", Mix_GetError());
		exit(EXIT_FAILURE);
	}
}

void SFX_Play(Mix_Chunk* sfx){
	
	int channel = Mix_PlayChannel(-1, sfx, 0);
	if (!sfx) { 
		fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError()); 
	}
	if (channel == -1){
		fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
	}
}

