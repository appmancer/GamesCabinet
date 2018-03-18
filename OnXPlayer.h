/*
 * A player of Naughts and Crosses. Could be human... or an alien!  (or the computer)
 */

#ifndef ONXPLAYER_H
#define ONXPLAYER_H

#include <stdint.h> 

typedef struct OnXPlayerState
{
  uint8_t controlState;     // X axis position 0..7
  //NB  This button state reflects the values returned by the PlayerControl button state method
  uint8_t buttonState;      // bit 7     fire button
                            // bit 6     horizontal encoder
                            // bit 5     vertical encoder
                            // bit 0-4   unused 
};

class OnXPlayer
{
  public:  
    virtual bool hasMoved() = 0;
    virtual OnXPlayerState* getState() = 0;
    virtual bool hasPressedButton(uint8_t button) = 0;
    virtual void reset() = 0;
    virtual bool isComputer() = 0;
    virtual void recommendedMove(uint8_t recMove) = 0;
    
  protected:
    OnXPlayerState playerState;
};

#endif
