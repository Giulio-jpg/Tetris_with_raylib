#include <time.h>
#include "TetrisFunctions.h"
#include "TetrisData.h"
#include "TetrisDefine.h"

int stage[] = 
{
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

Sound SFXDeleteLines;
int score;
float tetrominoDescentTime;

int main(int argc, char** argv, char** environ)
{
    const int windowWidth = 720;    
    const int windowHeight = 512;    

    const int startOffsetX = windowWidth / 2 - STAGE_WIDTH * TILE_SIZE / 2;
    const int startOffsetY = windowHeight / 2 - STAGE_HEIGHT * TILE_SIZE / 2;

    
    const int tetrominoStartCellX = STAGE_WIDTH / 2;
    const int tetrominoStartCellY = 0;

    int tetrominoCellX = tetrominoStartCellX;
    int tetrominoCellY = tetrominoStartCellY;

    time_t unixTime;
    time(&unixTime);
    SetRandomSeed(unixTime);

    int currentTetrominoType = GetRandomValue(0, 6);
    int tetrominoRotation = 0;
    int tetrominoColor = GetRandomValue(1, 8);      // first color is for the stage

    float descentTimer = 1.0f;
    tetrominoDescentTime = descentTimer;
    
    InitWindow(windowWidth, windowHeight, "Tetris");

    // AUDIO
    InitAudioDevice();

    // Music
    Music music = LoadMusicStream("TetrisMusic.mp3");
    music.looping = true;
    PlayMusicStream(music);

    // SOUND EFFECTS
    SFXDeleteLines = LoadSound("ClearLine.wav");         

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        descentTimer -= GetFrameTime();

        // INPUT
        if (IsKeyPressed(KEY_SPACE))
        {
            const int lastRotation = tetrominoRotation;

            tetrominoRotation++;

            if (tetrominoRotation > 3)
            {
                tetrominoRotation = 0;
            }

            if (CheckCollision(tetrominoCellX, tetrominoCellY, tetrominoTypes[currentTetrominoType][tetrominoRotation]))
            {
                tetrominoRotation = lastRotation;
            }
            
        }

        if (IsKeyPressed(KEY_RIGHT))
        {
            // no need to check for overflow, it is blocked by the wall
            if(!CheckCollision(tetrominoCellX + 1, tetrominoCellY, tetrominoTypes[currentTetrominoType][tetrominoRotation]))
            {
                tetrominoCellX++;
            }
            
        }
        
        if (IsKeyPressed(KEY_LEFT))
        {
            if(!CheckCollision(tetrominoCellX - 1, tetrominoCellY, tetrominoTypes[currentTetrominoType][tetrominoRotation]))
            {
                tetrominoCellX--;
            }
        }

        if (descentTimer <= 0 || IsKeyPressed(KEY_DOWN))
        {
            if (!CheckCollision(tetrominoCellX, tetrominoCellY + 1, tetrominoTypes[currentTetrominoType][tetrominoRotation]))
            {
                tetrominoCellY++;
                descentTimer = tetrominoDescentTime;
            }
            else
            {
                for (int y = 0; y < TETROMINO_SIZE; y++)
                {
                    for (int x = 0; x < TETROMINO_SIZE; x++)
                    {
                        const int offset = y * TETROMINO_SIZE + x;

                        const int* tetromino = tetrominoTypes[currentTetrominoType][tetrominoRotation];

                        if (tetromino[offset] == 1)
                        {
                            const int offset_stage = (y + tetrominoCellY) * STAGE_WIDTH + (x + tetrominoCellX);   

                            stage[offset_stage] = tetrominoColor + 1;
                        }
                    }
                }

                DeleteLines();

                tetrominoCellX = tetrominoStartCellX;
                tetrominoCellY = tetrominoStartCellY;

                currentTetrominoType = GetRandomValue(0, 6);
                tetrominoRotation = 0;
                tetrominoColor = GetRandomValue(1, 8);
            }      
            
        }


        // DRAW
        BeginDrawing();
        ClearBackground(RED);
        

        for (int y = 0; y < STAGE_HEIGHT; y++)
        {
            for (int x = 0; x < STAGE_WIDTH; x++)
            {
                const int offset = y * STAGE_WIDTH + x;

                const int stageColor = stage[offset]; 

                if (stage[offset] != 0)
                {
                    DrawRectangle(x * TILE_SIZE + startOffsetX, y * TILE_SIZE + startOffsetY, TILE_SIZE, TILE_SIZE, colorsTypes[stageColor - 1]);
                }
                
                DrawRectangleLines(x * TILE_SIZE + startOffsetX, y * TILE_SIZE + startOffsetY, TILE_SIZE, TILE_SIZE, BLACK);
            }
            
        }
        
        DrawTetromino(startOffsetX, startOffsetY, tetrominoCellX, tetrominoCellY, tetrominoTypes[currentTetrominoType][tetrominoRotation], colorsTypes[tetrominoColor]);

        DrawText(TextFormat("Score: %05i", score), SCORE_TEXT_X, SCORE_TEXT_Y, FONT_SIZE, WHITE);

        EndDrawing();
    }
    
    UnloadMusicStream(music);          

    CloseAudioDevice();     

    CloseWindow();

    return 0;
}








































