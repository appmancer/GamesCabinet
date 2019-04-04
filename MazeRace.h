/*
 * Maze Race
 * Competitive maze game
 * by Samuel Pickard
 */

#ifndef MAZERACE_H
#define MAZERACE_H

#include "Cabinet.h"
#include "GameBase.h"
#include "MazeRacePlayer.h"
#include "MazeGenerator.h"

#define MAZE_NONE      0
#define MAZE_PREGAME   1
#define MAZE_COUNTDOWN 2
#define MAZE_INGAME    3
#define MAZE_ENDED     4

#define MATRIX_SIZE 8
#define MAZE_DEFAULT_WIDTH 15
#define MAZE_DEFAULT_HEIGHT 15

class MazeRace : public GameBase
{
  private:
    MazeRacePlayer* player1;
    MazeRacePlayer* player2;

    char* mMaze;
    const char* mGameName = "Maze Race";
    uint16_t mEntrance;
    uint16_t mExit;
    bool mPlayer1Finished;
    bool mPlayer2Finished;

    void reset();
    uint8_t currentPhase;
    uint32_t readyTime;
    uint32_t mStartTime;
    uint8_t mCountdown;
    
    void showPregame();
    void updatePregame();
    void updateCountdown();
    void updateIngame();
    void updateEnded();

    void drawMap();
    void drawMap(uint16_t playerPos, PixelEffects* sfx, uint32_t playerColour);

  public:
    MazeRace(Cabinet* cab);
    ~MazeRace();

    //This will be set to the function pointer of the main loop
    //and be called when MazeRace is the active game
    void gameloop();

    void startDemo();
};

#endif
