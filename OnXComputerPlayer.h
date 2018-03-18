
 #ifndef ONXCOMPUTERPLAYER_H
 #define ONXCOMPUTERPLAYER_H
 
 #include "OnXPlayer.h"

class OnXComputerPlayer : public OnXPlayer
{
  public:  
    OnXComputerPlayer();
    ~OnXComputerPlayer();
    bool hasMoved();
    OnXPlayerState* getState();
    bool hasPressedButton(uint8_t button);
    void reset();
    bool  isComputer();
    void  recommendedMove(uint8_t recMove);
    
  protected:
    uint8_t selectedColumn;

    unsigned long readyTime;
};

#endif
