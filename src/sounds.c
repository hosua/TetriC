#include "sounds.h"

bool _sfx_muted = false;

uint8_t bgm_vol = 50;

void SFX_PlayBGM(const char* bgm_path, int8_t bgm_vol){
	// Open audio device
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_Music* sfx_bgm = Mix_LoadMUS(bgm_path);
	Mix_PlayMusic(sfx_bgm, -1);
	Mix_VolumeMusic(bgm_vol);
}
void SFX_MuteBGM(){
	Mix_VolumeMusic(0);
}
void SFX_ResumeBGM(){
	Mix_VolumeMusic(bgm_vol);
}

void SFX_MusicVol(uint8_t vol){
	bgm_vol = vol;
	SFX_ResumeBGM();
}

void SFX_MusicVolUp(){
	bgm_vol++;
	if (bgm_vol >= 100)
		bgm_vol = 100;
}
void SFX_MusicVolDown(){
	bgm_vol--;
	if (bgm_vol <= 2)
		bgm_vol = 3;
}


// If _should_nudge is true, nudge sound will play and then it will be unset for _nudge_delay milleseconds.
bool _should_nudge = true;

// sound effects
Mix_Chunk* _sfx_lockpiece = NULL;
Mix_Chunk* _sfx_lineclear = NULL;
Mix_Chunk* _sfx_levelup = NULL;
Mix_Chunk* _sfx_tetris = NULL;
Mix_Chunk* _sfx_menu = NULL;
Mix_Chunk* _sfx_nudge = NULL;

void SFX_LoadSoundBank(){
	// Open audio device
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ){
		fprintf(stderr, "Error: Failed to open audio device\n");
		exit(1);
	}
	// Load all sound effects into memory
	_sfx_lineclear = Mix_LoadWAV("sfx/line-clear.wav");
	_sfx_levelup = Mix_LoadWAV("sfx/level-up.wav");
	_sfx_lockpiece = Mix_LoadWAV("sfx/lock-piece.wav");
	_sfx_tetris =  Mix_LoadWAV("sfx/tetris-sound.wav");
	_sfx_menu = Mix_LoadWAV("sfx/menu-sound.wav");
	_sfx_nudge = Mix_LoadWAV("sfx/nudge.wav");

 	// If there was a problem loading the sound effects, show error
	if (!_sfx_lockpiece || !_sfx_tetris || !_sfx_menu ){
		fprintf(stderr, "Error: There was an error loading an sfx file: %s\n", Mix_GetError());
		exit(1);
	}
}

void SFX_FreeSFX(){
	Mix_FreeChunk(_sfx_lockpiece);
	Mix_FreeChunk(_sfx_tetris);
	Mix_FreeChunk(_sfx_menu);
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

