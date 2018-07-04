/*
 * OnX
 * by
 * Samuel Pickard
 */

#ifndef ONX_H
#define ONX_H

#include "Cabinet.h"
#include "GameBase.h"
#include "OnXPlayer.h"

//There are three phases:
//  before the game, waiting for players to start 
//  the attack phase where only one player has control at any time
//  end of games, where we display the winner

#define ONX_NONE           0
#define ONX_PREGAME        1
#define ONX_ATTACK         3
#define ONX_WINNER         4
#define ONX_EFFECTS        5 //A state that just allows for special effects
#define ONX_CHANGE_PLAYER  6
#define ONX_DRAW           7

#define TURN_DELAY  200 //milliseconds

#define PLAYER_1_COUNTER       2
#define PLAYER_2_COUNTER       4
#define BOARD_SQUARE           0

#define BOARDSIZE              9
#define NORTH                 -8
#define NORTHEAST             -7
#define EAST                   1
#define SOUTHEAST              9
#define SOUTH                  8
#define SOUTHWEST              7
#define WEST                  -1
#define NORTHWEST             -9

//const int8_t          searchOrder[8] = {NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST};
const int8_t    physicalPositions[9] = { 0, 3, 6, 24, 27, 30, 48, 51, 54 };

class OnX : public GameBase
{
  private:  
    void reset();

    uint32_t          selectedPosition;
    uint32_t          playerColour;
    uint32_t          counterColour;
    uint32_t          opponentColour;
    uint32_t          player1Colour;
    uint32_t          player2Colour;
    uint32_t          boardColour;
    uint32_t          cursorColour;
    uint32_t          cursorValid;
    uint32_t          cursorInvalid;
    uint32_t          currentPhase;
    uint8_t           board[BOARDSIZE]; //The 3x3 playing board
    uint8_t           moveCounter;
    unsigned long     readyTime;
    OnXPlayer*    player1;
    OnXPlayer*    player2;
    OnXPlayer*    currentPlayer;
    Adafruit_NeoPixel* winnerLight;
    LiquidCrystal_I2C* attackingLCD;
    LiquidCrystal_I2C* defendingLCD;
    uint8_t           validMoves[BOARDSIZE];
    bool              flash;
    
    void showPregame();
    void updatePregame();
    void startMove();
    void updateMainGame();
    void playPiece();
    void startWinner();
    void updateWinner();
    void updateEffects();
    void startDraw();
    void updateDraw();
    void debugBoard();
    void drawCursor();
    void drawCursor(uint32_t pos, uint32_t col);

    uint8_t bestMove(uint8_t myCounter);
    uint8_t findLines(uint8_t counter, uint8_t target);
    uint8_t countLine(uint8_t pos1, uint8_t pos2, uint8_t pos3, uint8_t counter);
    
    void drawBoard();

    const char* mGameName = "Noughts and Crosses";

  public:
    OnX(Cabinet* cab);
    ~OnX();
    
    void gameloop();
    void startDemo();
};
#endif
