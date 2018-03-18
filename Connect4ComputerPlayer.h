
 #ifndef CONNECT4COMPUTERPLAYER_H
 #define CONNECT4COMPUTERPLAYER_H
 
 #include "Connect4Player.h"

class Connect4ComputerPlayer : public Connect4Player
{
  public:  
    Connect4ComputerPlayer();
    ~Connect4ComputerPlayer();
    bool hasMoved();
    Connect4PlayerState* getState();
    bool hasPressedButton(uint8_t button);
    void reset();
    void setBoard(uint8_t* mainBoard, uint32_t counterColour);
    
  protected:
    uint8_t choosePosition();
    uint8_t lineLength(uint8_t nextCounter);
    uint8_t testLine(uint32_t lastPosition, uint32_t increment);
    uint8_t* board;
    uint8_t selectedColumn;
    uint32_t counterColour;

    unsigned long readyTime;
};

#endif
