/*
 * A player of Connect 4. Could be human... or an alien!  (or the computer)
 */

#ifndef CONNECT4PLAYER_H
#define CONNECT4PLAYER_H

#include <stdint.h> 

typedef struct Connect4PlayerState
{
  uint8_t controlState;     // X axis position 0..7
  //NB  This button state reflects the values returned by the PlayerControl button state method
  uint8_t buttonState;      // bit 7     fire button
                            // bit 6     horizontal encoder
                            // bit 5     vertical encoder
                            // bit 0-4   unused 
};

class Connect4Player
{
  public:  
    virtual bool hasMoved() = 0;
    virtual Connect4PlayerState* getState() = 0;
    virtual bool hasPressedButton(uint8_t button) = 0;
    virtual void reset() = 0;
    virtual void setBoard(uint8_t* mainBoard, uint32_t counterColour) = 0;
    
  protected:
    Connect4PlayerState playerState;
};

#endif
