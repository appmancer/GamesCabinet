/*
 * Connect 4
 * by
 * Samuel Pickard
 */

#ifndef CONNECT4_H
#define CONNECT4_H

#include "Cabinet.h"
#include "GameBase.h"
#include "Connect4Player.h"

//There are three phases:
//  before the game, waiting for players to start 
//  the attack phase where only one player has control at any time
//  end of games, where we display the winner

#define CONNECT4_NONE           0
#define CONNECT4_PREGAME        1
#define CONNECT4_ATTACK         3
#define CONNECT4_WINNER         4
#define CONNECT4_EFFECTS        5 //A state that just allows for special effects
#define CONNECT4_CHANGE_PLAYER  6

#define DROP_DELAY  200 //milliseconds

#define PLAYER_1_COUNTER        2
#define PLAYER_2_COUNTER        4

#define BOARDSIZE              64

class Connect4 : public GameBase
{
  private:
    void reset();

    uint32_t        selectedColumn;
    uint32_t        playerColour;
    uint32_t        counterColour;
    uint32_t        dropRow;
    uint32_t        currentPhase;
    int8_t          winningPieces[7];
    bool            flashOn;
    uint8_t         board[64]; //The 6x8 playing board, but mapped onto the matrix
    unsigned long   readyTime;
    Connect4Player* player1;
    Connect4Player* player2;
    Connect4Player* currentPlayer;
    Adafruit_NeoPixel* winnerLight;
    LiquidCrystal_I2C* attackingLCD;
    LiquidCrystal_I2C* defendingLCD;
    
    const uint32_t COLOURS[2] = {
        YELLOW,         /* player 1 */
        RED             /* player 2 */
    };

    void showPregame();
    void updatePregame();
    void startMove();
    void updateMainGame();
    void dropPiece();
    void startWinner();
    void updateWinner();
    void updateEffects();
    void debugBoard();
    void drawCursor();

    bool testLine(uint8_t testBoard[], uint32_t lastPosition, uint32_t increment);
    
  public:
    Connect4(Cabinet* cab);
    ~Connect4();
    
    void gameloop();
    bool playerWon(uint8_t testBoard[], uint32_t lastPosition);
};
#endif
