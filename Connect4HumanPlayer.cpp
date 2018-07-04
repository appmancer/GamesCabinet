#include "Connect4HumanPlayer.h"
#include "PlayerControl.h"

Connect4HumanPlayer::Connect4HumanPlayer(PlayerControl* control) : playerControl(control)
{
}

Connect4HumanPlayer::~Connect4HumanPlayer()
{}


bool Connect4HumanPlayer::hasMoved()
{
  //Check to see if either encoder button has been pressed and the orientation has changed
  uint8_t newState = playerControl->buttonState();
  uint8_t oldState = playerState.buttonState;

  bool oldHPressed = (oldState & PlayerControl::HorizontalButton) == PlayerControl::HorizontalButton; 
  bool newHPressed = (newState & PlayerControl::HorizontalButton) == PlayerControl::HorizontalButton; 

  //For Connect4, we're only interested in the horizontal encoder.
  int8_t hPos = playerControl->hEncoder.read() / 4; //Each encoder 'click' is 4 readable positions
    
  //Update the new position in the player's state
  uint8_t oldPos = playerState.controlState;  //Take a copy of the position  

  if(oldPos != hPos)
  {
    //Check to see if the new position is valid
    //Check that we've not gone off the left or right edges
    if(hPos < 0)
    { 
      hPos = 0;
      //As well as changing the local variable to stop us falling off the edge, we have to update that value on the encoder itself
      playerControl->hEncoder.write(0);
    }
    else if(hPos > 7)
    { 
      hPos = 7;
      playerControl->hEncoder.write(hPos * 4);
    }
    /*
    Serial.println("Reading position");
    Serial.print("oldPos: ");
    Serial.println(oldPos);
    Serial.print("hPos: ");
    Serial.println(hPos);
    */
    //Store the current state as old state 
    playerState.controlState = hPos;
    Serial.print("Writing new position as : ");
    Serial.println(playerState.controlState, HEX);
  }
  return !(oldPos == hPos);  
}

Connect4PlayerState* Connect4HumanPlayer::getState()
{
  return &playerState; 
}

bool Connect4HumanPlayer::hasPressedButton(uint8_t button)
{
  uint8_t newState = playerControl->buttonState();
  uint8_t oldState = playerState.buttonState;  
  
  bool oldPressed = ((oldState & button) == button);  
  bool newPressed = ((newState & button) == button);

  //Debug buttons
  //if(0)
  //{
    //Serial.print(newPressed & button);    Serial.print(" | ");
    //Serial.print(newPressed);
    //Serial.print(" | ");
    //Serial.print(oldPressed);
    //Serial.print(" | ");
    //Serial.println(newPressed && !oldPressed);
  //}
  
  //Save the new state, but only for the tested button
  if(newPressed)
  {
    playerState.buttonState |= button;
  }
  else
  { 
    playerState.buttonState &= ~button;
  }

  //Return true if the button is down on the newly read state, but not down on the old state
  
  return (newPressed && !oldPressed);
}

void Connect4HumanPlayer::reset()
{
  playerState.controlState   = 0x00; //Reset selected axis
  playerState.buttonState    = 0x00 ; // All buttons pressed.  This means that we can flush any existing button presses at the start of the game
  playerControl->hEncoder.write(0);
  playerControl->vEncoder.write(0);
}


