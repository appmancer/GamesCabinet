
#ifndef REVERSIHUMANPLAYER_H
#define REVERSIHUMANPLAYER_H
 
#include "OnXPlayer.h"
#include "PlayerControl.h"

#define MAX_WIDTH 2
#define MAX_HEIGHT 2

class OnXHumanPlayer : public OnXPlayer
{
  public:
    OnXHumanPlayer(PlayerControl* control);
    ~OnXHumanPlayer();
    bool hasMoved();
    OnXPlayerState* getState();
    bool hasPressedButton(uint8_t button);
    void reset();
    uint8_t* getBoard();
    bool  isComputer();
    void  recommendedMove(uint8_t recMove);
    
  protected:
    PlayerControl* playerControl;
};

#endif
