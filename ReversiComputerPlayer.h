
 #ifndef REVERSICOMPUTERPLAYER_H
 #define REVERSICOMPUTERPLAYER_H
 
 #include "ReversiPlayer.h"

class ReversiComputerPlayer : public ReversiPlayer
{
  public:  
    ReversiComputerPlayer();
    ~ReversiComputerPlayer();
    bool hasMoved();
    ReversiPlayerState* getState();
    bool hasPressedButton(uint8_t button);
    void reset();
    bool  isComputer();
    void  recommendedMove(uint8_t recMove);
    
  protected:
    uint8_t selectedColumn;

    unsigned long readyTime;
};

#endif
