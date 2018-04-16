/*
 * Reversi
 * by
 * Samuel Pickard
 */

#ifndef REVERSI_H
#define REVERSI_H

#include "Cabinet.h"
#include "GameBase.h"
#include "ReversiPlayer.h"

//There are three phases:
//  before the game, waiting for players to start 
//  the attack phase where only one player has control at any time
//  end of games, where we display the winner

#define REVERSI_NONE           0
#define REVERSI_PREGAME        1
#define REVERSI_ATTACK         3
#define REVERSI_WINNER         4
#define REVERSI_EFFECTS        5 //A state that just allows for special effects
#define REVERSI_CHANGE_PLAYER  6

#define TURN_DELAY  200 //milliseconds

#define PLAYER_1_COUNTER       2
#define PLAYER_2_COUNTER       4
#define BOARD_SQUARE           0

#define BOARDSIZE              64
#define NORTH                 -8
#define NORTHEAST             -7
#define EAST                   1
#define SOUTHEAST              9
#define SOUTH                  8
#define SOUTHWEST              7
#define WEST                  -1
#define NORTHWEST             -9

const int8_t          searchOrder[8] = {NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST};
const int8_t          rowOffsets[8]  = {-1,   -1,         0,    1,         1,     1,         0,   -1};

  //Alternate starting positions for testing
const int8_t  closeToEnd[64] = {4, 2, 2, 4, 4, 2, 2, 4,
                                2, 2, 2, 2, 2, 2, 2, 2,
                                2, 2, 2, 2, 2, 2, 2, 2,
                                4, 2, 2, 0, 2, 2, 2, 4,
                                4, 2, 2, 2, 0, 2, 2, 4,
                                2, 2, 2, 2, 2, 2, 2, 2,
                                2, 2, 2, 2, 2, 2, 2, 2,
                                4, 2, 2, 4, 4, 2, 2, 4};

const int8_t  blackCantMove[64] = {2, 2, 2, 2, 2, 2, 2, 2,
                                   4, 4, 2, 4, 2, 4, 2, 4,
                                   4, 4, 2, 4, 2, 4, 2, 4,
                                   0, 0, 2, 4, 2, 4, 2, 0,
                                   0, 0, 4, 2, 4, 2, 0, 0,
                                   0, 0, 4, 2, 4, 2, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0};

const int8_t  stuckCorner[64] =   {2, 0, 0, 0, 0, 0, 0, 4,
                                   0, 2, 2, 0, 0, 0, 4, 0,
                                   0, 0, 2, 2, 0, 4, 0, 0,
                                   4, 2, 2, 4, 4, 2, 0, 0,
                                   2, 2, 2, 4, 2, 2, 4, 4,
                                   0, 4, 4, 4, 2, 2, 0, 0,
                                   0, 0, 0, 4, 0, 2, 4, 0,
                                   0, 0, 0, 0, 0, 0, 0, 4};

class Reversi : public GameBase
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
    uint8_t           board[BOARDSIZE]; //The 8x8 playing board
    uint8_t           moveCounter;
    unsigned long     readyTime;
    ReversiPlayer*    player1;
    ReversiPlayer*    player2;
    ReversiPlayer*    currentPlayer;
    Adafruit_NeoPixel* winnerLight;
    LiquidCrystal_I2C* attackingLCD;
    LiquidCrystal_I2C* defendingLCD;
    uint8_t           flippedCounters[BOARDSIZE];
    uint8_t           nextFlippedCounter;
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
    void debugBoard();
    void debugFlipList();
    void drawCursor();
    bool flipCounters(uint32_t lastPosition);
    uint32_t testLine(uint32_t lastPosition, int8_t increment, int8_t rowDiff, uint8_t myCounter);
    void flipSingleLine(uint32_t startPosition, int8_t increment, uint32_t count);
    bool isLegal(uint8_t testPos, uint8_t opponentCounter);
    uint8_t bestMove(uint8_t myCounter);
    
    void drawBoard();

    void copyArray(uint8_t* source, uint8_t* target);

  public:
    Reversi(Cabinet* cab);
    ~Reversi();
    
    void gameloop();
    void startDemo();
};
#endif
