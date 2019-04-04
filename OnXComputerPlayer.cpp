 #include <Arduino.h>
#include "OnXComputerPlayer.h"


OnXComputerPlayer::OnXComputerPlayer()
{
  reset();
}

OnXComputerPlayer::~OnXComputerPlayer()
{}

bool OnXComputerPlayer::hasMoved()
{
  if(selectedColumn == 0xFF)
    return false;
    
  if(readyTime > millis())
    return false;

  //Serial.print("Aiming for column ");
  //Serial.println(selectedColumn);
  
  if(playerState.controlState < selectedColumn)
  {
    if((playerState.controlState / 3) < (selectedColumn / 3))
    {
      //Move down one column
      playerState.controlState += 3;
    }
    else
    {
      playerState.controlState++;
    }  
  }
  else if(playerState.controlState > selectedColumn)
  {
    
    if((playerState.controlState / 3) > (selectedColumn / 3))
    {
      //Move up one column
      playerState.controlState -= 3;
    }
    else
    {
      playerState.controlState--;
    }
    
  }
  else
    return false;  
    
  readyTime = millis() + 300;
  return true;
}

OnXPlayerState* OnXComputerPlayer::getState()
{
  return &playerState; 
}

bool OnXComputerPlayer::hasPressedButton(uint8_t button)
{
  //If we're at the right column, press fire
  if(selectedColumn == playerState.controlState)
  {
    selectedColumn = 0xFF; //reset  
    return true;
  }
  else
    return false;
}

void OnXComputerPlayer::reset()
{
  selectedColumn = 0xFF;
  playerState.controlState = 0;
  playerState.buttonState = 0;  
}


bool OnXComputerPlayer::isComputer()
{
  return true;  
}

void OnXComputerPlayer::recommendedMove(uint8_t recMove)
{
  //Serial.print("The recommended move is ");Serial.println(recMove);
  selectedColumn = recMove;  
  readyTime = millis() + 1000;
}

