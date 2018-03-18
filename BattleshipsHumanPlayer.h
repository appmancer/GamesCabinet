/**
 * The class that encapsulates player interaction
 */

 #ifndef BATTLESHIPSHUMANPLAYER_H
 #define BATTLESHIPSHUMANPLAYER_H

#include "PlayerControl.h"
#include "BattleshipsPlayer.h"

class BattleshipsHumanPlayer : public BattleshipsPlayer
{
  public:
    BattleshipsHumanPlayer(PlayerControl* control);
    ~BattleshipsHumanPlayer();

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
    
  private:
    PlayerControl* playerControl;
    bool isValidPosition(uint8_t initialPos, uint8_t* board);
};

 #endif
