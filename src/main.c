#include "graphics.h"
#include "tetris.h"
#include "input.h"
#include "clock.h"
#include "queue.h"
#include "sounds.h"


void PlayGame(){
    // seed the RNG
    srand(time(0));
    // Variable to determine if a piece is still falling. If one is not falling, a new piece is spawned.
    // and then this variable is set to true again.
    bool is_falling = false;

    // InitEverything(_game_data->level);
    init_GameData(_start_level);
    GFX_ClearScreen();

    /* Sound stuff */
    // Loud sound effects into memory
    SFX_PlayBGM();
    /* End sound stuff */

    /* Queue limit must be n+1 the total displayed tetronimos */
    Queue queue = init_Queue(4);
    // Fill queue with pieces
    for (int i = 0; i < queue.limit-1; i++)
        enqueue(rand_T_Type(), &queue);
    // Get first piece from queue
    Tetronimo* tetronimo = new_Tetronimo(dequeue(&queue));
    enqueue(rand_T_Type(), &queue);
    /* End queue stuff */

    // Char buffer for rendering text
    char buf[128];
    uint8_t buf_max = sizeof(buf);

    float elapsed = 0.0f;
    _game_data->fps = elapsed;
    GetLinesUntilNextLevel(_game_data->level);

    for (;;){
        uint64_t start = SDL_GetPerformanceCounter();
        SDL_Event event; 
        SDL_PollEvent(&event);
        GFX_ClearScreen();
        // Get user input
        Input_SetKeyArray(event);
        Input_MovementHandler(event, tetronimo);

        Input_VolumeController(event);

        // Render tetronimos	
        GFX_RenderBlocks(8, 0);
        // Render the UI elements
        GFX_RenderUI(20, 0, BLOCK_SIZE, buf, buf_max, _game_data->level);
        // Render controls help
        GFX_RenderHelp(20, 15, BLOCK_SIZE, buf, buf_max);
        // Render tetronimo statistics
        GFX_RenderStatsUI(3, 1, BLOCK_SIZE/1.8f, buf, buf_max);
        // Render next tetronimo queue
        GFX_RenderQueue(30, 5, BLOCK_SIZE/1.8f, buf, buf_max, 3, queue);
        // Present the renderings to the screen
        SDL_RenderPresent(_gfx->renderer); 

        Input_PauseHandler(event);
        if (_game_state == G_PAUSE) {
            _clock->last_tick = SDL_GetTicks();
            // TODO: display "PAUSED" on screen
        } else {

            if (InputTimer()){
                Input_DownwardMovementHandler(tetronimo);
            }
            // NudgeTimer() prevents the nudge sfx from being spammed
            if (NudgeTimer()){
                _sfx->should_nudge = true;
            }
            if (LevelTimer(_game_data->level)){
                is_falling = move_Tetronimo(tetronimo, M_DOWN);
                if (!is_falling){
                    if (IsPlayerDead()){
                        QuitGame(E_INGAME);
                        // _game_state = G_GAMEOVER;
                        break;
                    }

                    _game_data->lines_cleared_this_turn = CheckLines();
                    if (!_sfx->muted){
                        if (_game_data->lines_cleared_this_turn == 4)
                            SFX_Play(_sfx->tetris);
                        else if (_game_data->lines_cleared_this_turn == 0)
                            SFX_Play(_sfx->lock_piece);
                        else
                            SFX_Play(_sfx->line_clear);

                    }
                    // The number of lines to carry over to the next level
                    uint8_t carry = _game_data->lines_cleared_this_turn - _game_data->lines_until_level;
                    _game_data->lines_until_level -= _game_data->lines_cleared_this_turn;

                    free(tetronimo);

                    // dequeue a new piece from queue to spawn it, then enqueue another 
                    T_Type t_type = dequeue(&queue);
                    tetronimo = new_Tetronimo(t_type);
                    _game_data->tetronimo_counter[t_type]++;
                    enqueue(rand_T_Type(), &queue);

                    // Do all point calculations for this turn
                    CalcScore();

                    // If player levels up
                    if (_game_data->lines_until_level <= 0){
                        if (!_sfx->muted)
                            SFX_Play(_sfx->level_up);
                        _game_data->level++;
                        // Set _game_data->lines_until_level
                        GetLinesUntilNextLevel(_game_data->level);
                        // Carry over extra lines
                        if (carry)
                            _game_data->lines_until_level -= carry;
                    }
                }
            } 
        }
        // Calculate FPS
        uint64_t end = SDL_GetPerformanceCounter();
        elapsed = (end - start) / (float)SDL_GetPerformanceFrequency();
        _game_data->fps = elapsed;
    }

}


// TODO?
void MainMenu(){
    // Char buffer for rendering text
    char buf[128];
    size_t buf_max = sizeof(buf);
    SDL_Event event;
    for (;;){
        GFX_ClearScreen();
        SDL_PollEvent(&event);
        Input_SetKeyArray(event);
        SDL_RenderPresent(_gfx->renderer); 
        if (_game_state != G_MAINMENU)
            break;
    }
}

// TODO?
void Gameover(){
    char buf[128];
    uint8_t buf_max = sizeof(buf);
    PrintGameOver();
    for (;;){
        GFX_ClearScreen();
        SDL_Event event;
        SDL_PollEvent(&event);
        Input_SetKeyArray(event);
        SDL_RenderPresent(_gfx->renderer); 
        if (_game_state != G_GAMEOVER)
            break;
    }
}

// TODO?
void LevelSelect(){
    char buf[128];
    uint8_t buf_max = sizeof(buf);
    for (;;){
        GFX_ClearScreen();
        SDL_Event event;
        SDL_PollEvent(&event);
        Input_SetKeyArray(event);
        SDL_RenderPresent(_gfx->renderer); 
        if (_game_state != G_LEVELSELECT)
            break;
    }
}

int main(int argc, char **argv){
    InitEverything();
    GFX_ClearScreen();
    int level = (argc > 1) ? atoi(argv[1]) :  0;
    printf("Starting on level: %i\n", level);
    _start_level = level;
    PlayGame();
}
