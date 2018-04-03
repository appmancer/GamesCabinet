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
#define MAZE_INGAME    2
#define MAZE_ENDED     3

#define MATRIX_SIZE 8
#define MAZE_DEFAULT_WIDTH 31
#define MAZE_DEFAULT_HEIGHT 31

class MazeRace : public GameBase
{
  private:
    MazeRacePlayer* player1;
    MazeRacePlayer* player2;

    String mMaze;

    void reset();
    uint8_t currentPhase;

    void showPregame();
    void updatePregame();
    void updateIngame();
    void updateEnded();

    void drawMap();
    void drawMap(uint16_t playerPos, PixelEffects* sfx, uint16_t playerColour);

  public:
    MazeRace(Cabinet* cab);
    ~MazeRace();

    //This will be set to the function pointer of the main loop
    //and be called when MazeRace is the active game
    void gameloop();
};

#endif
