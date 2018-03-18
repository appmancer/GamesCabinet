#include "OnXHumanPlayer.h"
#include "PlayerControl.h"

OnXHumanPlayer::OnXHumanPlayer(PlayerControl* control) : playerControl(control)
{
}

OnXHumanPlayer::~OnXHumanPlayer()
{
  
}

bool OnXHumanPlayer::hasMoved()
{
  //Check to see if either encoder button has been pressed and the orientation has changed
  uint8_t newState = playerControl->buttonState();
  uint8_t oldState = playerState.buttonState;

  bool oldHPressed = (oldState & PlayerControl::HorizontalButton) == PlayerControl::HorizontalButton; 
  bool newHPressed = (newState & PlayerControl::HorizontalButton) == PlayerControl::HorizontalButton; 

  int8_t hPos = playerControl->hEncoder.read() / 4; //Each encoder 'click' is 4 readable positions
  int8_t vPos = playerControl->vEncoder.read() / 4; //Each encoder 'click' is 4 readable positions
    
  //Update the new position in the player's state
  uint8_t oldPos = playerState.controlState;  //Take a copy of the position  

  //Check to see if the new position is valid
  //Check that we've not gone off the left or right edges
  if(hPos < 0)
  { 
    hPos = 0;
    playerControl->hEncoder.write(0);
  }
  else if(hPos > MAX_WIDTH)
  { 
    hPos = MAX_WIDTH;
    playerControl->hEncoder.write(MAX_WIDTH * 4);
  }
  
  if(vPos < 0)
  { 
    vPos = 0;
    playerControl->vEncoder.write(0);
  }
  else if(vPos > MAX_HEIGHT)
  { 
    vPos = MAX_HEIGHT;
    playerControl->vEncoder.write(MAX_HEIGHT * 4);
  }

  uint8_t newPos = (vPos * 3) + hPos;
  if(oldPos != newPos)
  {
    Serial.println("Changing position");Serial.print("oldPos: ");Serial.println(oldPos);Serial.print("newPos: ");Serial.println(newPos);
   
    //Store the current state as old state 
    playerState.controlState = newPos;
    //Serial.print("Writing new position as : ");Serial.println(playerState.controlState);
  }

  return !(oldPos == newPos);  
}

OnXPlayerState* OnXHumanPlayer::getState()
{
  return &playerState; 
}

bool OnXHumanPlayer::hasPressedButton(uint8_t button)
{
  uint8_t newState = playerControl->buttonState();
  uint8_t oldState = playerState.buttonState;  
  
  bool oldPressed = ((oldState & button) == button);  
  bool newPressed = ((newState & button) == button);
  
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

void OnXHumanPlayer::reset()
{
  playerState.controlState   = 0x00; // Reset selected axis
  playerState.buttonState    = 0x00; // All buttons pressed.  This means that we can flush any existing button presses at the start of the game
  playerControl->hEncoder.write(0);
  playerControl->vEncoder.write(0);
}


bool OnXHumanPlayer::isComputer()
{
  return false;  
}

void OnXHumanPlayer::recommendedMove(uint8_t recMove)
{  
}

