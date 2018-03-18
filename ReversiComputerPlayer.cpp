#include <Arduino.h>
#include "ReversiComputerPlayer.h"


ReversiComputerPlayer::ReversiComputerPlayer()
{
}

ReversiComputerPlayer::~ReversiComputerPlayer()
{}

bool ReversiComputerPlayer::hasMoved()
{
  if(selectedColumn == 0xFF)
    return false;
    
  if(readyTime > millis())
    return false;

  //Serial.print("Aiming for column ");
  //Serial.println(selectedColumn);
  
  if(playerState.controlState < selectedColumn)
  {
    if((playerState.controlState / 8) < (selectedColumn / 8))
    {
      //Move down one column
      playerState.controlState += 8;
    }
    else
    {
      playerState.controlState++;
    }  
  }
  else if(playerState.controlState > selectedColumn)
  {
    
    if((playerState.controlState / 8) > (selectedColumn / 8))
    {
      //Move up one column
      playerState.controlState -= 8;
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

ReversiPlayerState* ReversiComputerPlayer::getState()
{
  return &playerState; 
}

bool ReversiComputerPlayer::hasPressedButton(uint8_t button)
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

void ReversiComputerPlayer::reset()
{
  selectedColumn = 0xFF;
  playerState.controlState = 0;
  playerState.buttonState = 0;  
}


bool ReversiComputerPlayer::isComputer()
{
  return true;  
}

void ReversiComputerPlayer::recommendedMove(uint8_t recMove)
{
  //Serial.print("The recommended move is ");Serial.println(recMove);
  selectedColumn = recMove;  
  readyTime = millis() + 1000;
}

