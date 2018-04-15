#include <Arduino.h>
#include "PlayerControl.h"


 
PlayerControl::
    PlayerControl(uint8_t horizontalPinA, uint8_t horizontalPinB, uint8_t verticalPinA, uint8_t verticalPinB, uint8_t horizontalFire, uint8_t verticalFire, uint8_t firePin)
     : hEncoderButton(horizontalFire), vEncoderButton(verticalFire), fireButton(firePin), hEncoder(horizontalPinA, horizontalPinB), vEncoder(verticalPinA, verticalPinB)
{
  pinMode(horizontalFire, INPUT_PULLUP);
  pinMode(verticalFire, INPUT_PULLUP);
  pinMode(firePin, INPUT);

  Serial.println("Player Control created");
}

PlayerControl::~PlayerControl()
{}

uint8_t PlayerControl::buttonState()
{
  uint8_t state = 0;
  //The firebutton is pulled up, so when it is pressed, it is LOW
  if(digitalRead(fireButton) == HIGH)
  {
    state+= FireButton;  
  }

  //The encoders are pulled up, so when it is pressed, it is LOW
  if(digitalRead(hEncoderButton) == LOW)
  {
    state+= HorizontalButton;  
  }
  
  if(digitalRead(vEncoderButton) == LOW)
  {
    state+= VerticalButton;  
  }

  return state;
}
