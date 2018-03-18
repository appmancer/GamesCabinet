/*
 * A player of Reversi. Could be human... or an alien!  (or the computer)
 */

#ifndef REVERSIPLAYER_H
#define REVERSIPLAYER_H

#include <stdint.h> 

typedef struct ReversiPlayerState
{
  uint8_t controlState;     // X axis position 0..7
  //NB  This button state reflects the values returned by the PlayerControl button state method
  uint8_t buttonState;      // bit 7     fire button
                            // bit 6     horizontal encoder
                            // bit 5     vertical encoder
                            // bit 0-4   unused 
};

class ReversiPlayer
{
  public:  
    virtual bool hasMoved() = 0;
    virtual ReversiPlayerState* getState() = 0;
    virtual bool hasPressedButton(uint8_t button) = 0;
    virtual void reset() = 0;
    virtual bool isComputer() = 0;
    virtual void recommendedMove(uint8_t recMove) = 0;
    
  protected:
    ReversiPlayerState playerState;
};

#endif
