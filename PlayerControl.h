/*
 * Player Control
 * by
 * Samuel Pickard
 * 
 * Player control interfaces the two encoders, and three buttons that each player has
 */

#ifndef PLAYERCONTROL_H
#define PLAYERCONTROL_H
#include <Encoder.h>

class PlayerControl
{
  public:    
    PlayerControl(uint8_t horizontalPinA, uint8_t horizontalPinB, uint8_t verticalPinA, uint8_t verticalPinB, uint8_t horizontalFire, uint8_t verticalFire, uint8_t firePin);
    ~PlayerControl();

    Encoder hEncoder;
    Encoder vEncoder;
    uint8_t buttonState();

    const static uint8_t FireButton = 1;
    const static uint8_t HorizontalButton = 2;
    const static uint8_t VerticalButton = 4;
    
  protected:
    uint8_t hEncoderButton;
    uint8_t vEncoderButton;
    uint8_t fireButton;
};

#endif
