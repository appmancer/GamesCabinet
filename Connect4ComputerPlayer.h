
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
    uint8_t possibleMoves[8];
    
  protected:
    uint8_t choosePosition();
    uint8_t longestLine(uint8_t pos, uint8_t myColour);
    uint8_t testLine(uint8_t startPos, int8_t offset, int8_t expectedColumnChange, uint8_t myColour);
    uint8_t* board;
    uint8_t selectedColumn;
    uint32_t counterColour;

    unsigned long readyTime;
};

#endif
