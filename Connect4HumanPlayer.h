
#ifndef CONNECT4HUMANPLAYER_H
#define CONNECT4HUMANPLAYER_H
 
#include "Connect4Player.h"
#include "PlayerControl.h"

class Connect4HumanPlayer : public Connect4Player
{
  public:
    Connect4HumanPlayer(PlayerControl* control);
    ~Connect4HumanPlayer();
    bool hasMoved();
    Connect4PlayerState* getState();
    bool hasPressedButton(uint8_t button);
    void reset();
    uint8_t* getBoard();
    void setBoard(uint8_t* mainBoard, uint32_t counterColour) {};
    
  protected:
    PlayerControl* playerControl;
};

#endif
