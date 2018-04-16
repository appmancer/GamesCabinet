/*
 * Battleships
 * by 
 * Samuel Pickard
 */
 
#ifndef BATTLESHIPS_H
#define BATTLESHIPS_H

#include "Cabinet.h"
#include "GameBase.h"
#include "BattleshipsPlayer.h"
#include "BattleshipsHumanPlayer.h"


//There are three phases:
//  before the game, waiting for players to start 
//  set up where the players place their pieces and are both active
//  the attack phase where only one player has control at any time
//  end of games, where we display the winner

#define BATTLESHIPS_NONE      0
#define BATTLESHIPS_PREGAME   1
#define BATTLESHIPS_SETUP     2
#define BATTLESHIPS_ATTACK    3
#define BATTLESHIPS_WINNER    4
#define BATTLESHIPS_EFFECTS   5 //A state that just allows for special effects
#define BATTLESHIPS_CHANGE_PLAYER 6
#define BATTLESHIPS_DESTROYED_SHIP 7

class Battleships : public GameBase
{
  private:
    /* Colour reference
     * The index in the array is also the byte value of that data
     */
    const uint32_t COLOURS[14] = {
        BLACK,            /* nothing */
        NAVY,             /* sea */
        BLACK,            /* unused */
        DARKRED,          /* destroyed */
        DARKGOLDENROD,    /* carrier */
        WHITE,            /* damaged */
        PURPLE,           /* battleship */
        DARKOLIVEGREEN,   /* land */
        SALMON,           /* destroyer */
        DARKGREEN,        /* valid setup position */
        SEAGREEN,         /* submarine */
        RED,              /* invalid setup position */
        ORANGERED,        /* cruiser */
        BLACK             /* missed shot */
      };

    //Keep the highlighted position for targeting
    uint8_t currentAim = 0;
    uint8_t currentPhase;
    uint8_t nextPhase; //Keep track of which phase to go to when the effects finish
    unsigned long readyTime;

    //Keep score
    uint8_t p1SquaresRemaining = 17;
    uint8_t p2SquaresRemaining = 17;

    //Details of the ship under attack
    uint8_t attackedShipLength;
    uint8_t attackedShipAnimationCounter;
    uint8_t attackedShipNextCellPos;
    uint8_t attackedShipInc;
    
    uint8_t currentPlayer = PLAYER1;

    BattleshipsPlayer* player1;
    BattleshipsPlayer* player2;

    BattleshipsPlayer* attackingPlayer;
    BattleshipsPlayer* defendingPlayer;
    PixelEffects* attackingMatrix;
    PixelEffects* defendingMatrix;
    LiquidCrystal_I2C* attackingLCD;
    LiquidCrystal_I2C* defendingLCD;
    
  public:
    Battleships(Cabinet* cab);
    ~Battleships();
    
    //Restart the game
    void reset();
    
    //This will be set to the function pointer of the main loop
    //and be called when Battleships is the active game
    void gameloop();

    void startDemo();

    void updateEffects();
    void updatePregame();
    void updateSetup();
    void updateMainGame();
    void showPregame();

    //Swapped active players, update the boards and get ready for new player
    void startMove();
    //Copies board positions to a matrix
    void drawBoard(uint8_t board[64], PixelEffects* matrix);

    //Methods supporting the setup of the game board
    void updateSetupForPlayer(BattleshipsPlayer* player, PixelEffects* matrix);
    void addToBoard(uint8_t* board, PlayerState* state, PixelEffects* matrix);
    void drawCursor(PlayerState* state, PixelEffects* matrix);
    bool isValidPosition(uint8_t* board, PlayerState* state, uint8_t initialPos);
    void debugBoard(uint8_t* board);
      
    //Returns true if the cursor has been moved
    bool updateSetupPosition(PlayerControl* control, PlayerState* state, uint8_t* board);
    void incrementState(PlayerState* state);

    //Test for movement in attack phase
    void moveForAttacker();
    bool updateAttackCursor(PlayerControl* control, PlayerState* state);
    
    //Make an attack
    void fire();
    bool shipDestroyed(uint8_t pos);
    void updateDestroyedShip();

    //We have a winner    
    void updateWinner();

};

#endif

