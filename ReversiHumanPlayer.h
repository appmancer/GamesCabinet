
#ifndef REVERSIHUMANPLAYER_H
#define REVERSIHUMANPLAYER_H
 
#include "ReversiPlayer.h"
#include "PlayerControl.h"

class ReversiHumanPlayer : public ReversiPlayer
{
  public:
    ReversiHumanPlayer(PlayerControl* control);
    ~ReversiHumanPlayer();
    bool hasMoved();
    ReversiPlayerState* getState();
    bool hasPressedButton(uint8_t button);
    void reset();
    uint8_t* getBoard();
    bool  isComputer();
    void  recommendedMove(uint8_t recMove);
    
  protected:
    PlayerControl* playerControl;
};

#endif
