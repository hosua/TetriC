#include "sounds.h"

SFX* _sfx = NULL;

void SFX_PlayBGM(const char* bgm_path, int8_t bgm_vol){
	// Open audio device
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_Music* sfx_bgm = Mix_LoadMUS(bgm_path);
	Mix_PlayMusic(sfx_bgm, -1);
	Mix_VolumeMusic(_sfx->bgm_vol);
}
void SFX_MuteBGM(){
	Mix_VolumeMusic(0);
}
void SFX_ResumeBGM(){
	Mix_VolumeMusic(_sfx->bgm_vol);
}

void SFX_MusicVol(uint8_t vol){
	_sfx->bgm_vol = vol;
	SFX_ResumeBGM();
}

void SFX_MusicVolUp(){
	_sfx->bgm_vol++;
	if (_sfx->bgm_vol >= 100)
		_sfx->bgm_vol = 100;
}
void SFX_MusicVolDown(){
	_sfx->bgm_vol--;
	if (_sfx->bgm_vol <= 2)
		_sfx->bgm_vol = 3;
}


// If _should_nudge is true, nudge sound will play and then it will be unset for _nudge_delay milleseconds.
bool _should_nudge = true;

void init_SFX(){
	_sfx = (SFX*)malloc(sizeof(SFX));
}

void SFX_LoadSoundBank(){
	init_SFX();
	// Open audio device
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ){
		fprintf(stderr, "Error: Failed to open audio device\n");
		exit(1);
	}
	// Load all sound effects into memory
	_sfx->line_clear = Mix_LoadWAV("sfx/line-clear.wav");
	_sfx->level_up = Mix_LoadWAV("sfx/level-up.wav");
	_sfx->lock_piece = Mix_LoadWAV("sfx/lock-piece.wav");
	_sfx->tetris =  Mix_LoadWAV("sfx/tetris-sound.wav");
	_sfx->menu = Mix_LoadWAV("sfx/menu-sound.wav");
	_sfx->nudge = Mix_LoadWAV("sfx/nudge.wav");

	if (!_sfx->lock_piece || !_sfx->tetris || !_sfx->menu ){
		fprintf(stderr, "Error: There was an error loading an sfx file: %s\n", Mix_GetError());
		exit(1);
	}
}

void SFX_FreeSFX(){
	Mix_FreeChunk(_sfx->lock_piece);
	Mix_FreeChunk(_sfx->tetris);
	Mix_FreeChunk(_sfx->menu);
	// printf("Freed all sounds from memory.\n");
}

void SFX_PlaySFX(Mix_Chunk* sfx){
	
	int channel = Mix_PlayChannel(-1, sfx, 0);
	if (!sfx) { 
		fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError()); 
	}
	if (channel == -1){
		fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
	}
}

