/**
 * The class that encapsulates player interaction
 */

 #ifndef BATTLESHIPSCOMPUTERPLAYER_H
 #define BATTLESHIPSCOMPUTERPLAYER_H

#include "PlayerControl.h"
#include "BattleshipsPlayer.h"

class BattleshipsComputerPlayer : public BattleshipsPlayer
{
  public:
    BattleshipsComputerPlayer();
    ~BattleshipsComputerPlayer();

    bool hasMoved();
    PlayerState* getState();
    void incrementState();
    bool hasPressedButton(uint8_t button);
    void reset();
    uint8_t* getBoard();
    uint8_t* getTrackingBoard();
    
    void hit();
    bool isDestroyed();

    void debugPositions();

    void opponentHasBeenHit();
    void opponentShipDestroyed();
    
  private:
    bool isValidPosition(int initialPos, uint8_t* board);
    int moveInDirection(int oldPos, int newDir);
   
    int evaluateTarget(int pos);
    void findTargetFromDamaged();
    void selectTargetCell();

    int16_t selectedCell;
    int lastSuccessfulCell;
    bool lastPositionHit;
    int16_t searchDirection;

    unsigned long readyTime;
};

 #endif
