/***
 * Implmentation of BattleshipsPlayer for a real player
 */

#include "BattleshipsHumanPlayer.h"

BattleshipsHumanPlayer::BattleshipsHumanPlayer(PlayerControl* control) : playerControl(control)
{
  //Reset all boards back to 0
  for(int i=0; i< 64; i++)
  {
    playerBoard[i]   = WATER;
    trackingBoard[i] = WATER;
  }

  reset();
}

BattleshipsHumanPlayer::~BattleshipsHumanPlayer();

/**
 * Returns a pointer to this players board
 */
uint8_t* BattleshipsHumanPlayer::getBoard()
{
  return &(playerBoard[0]);  
}

/**
 * Returns a pointer to this players tracking board
 */
uint8_t* BattleshipsHumanPlayer::getTrackingBoard()
{
  return &(trackingBoard[0]);  
}


/**
 * Returns true if the encoders are in a different position from last time.  Additionally this method will check boundary position of the cursor
 */
bool BattleshipsHumanPlayer::hasMoved()
{
     //Check to see if either encoder button has been pressed and the orientation has changed
  uint8_t newState = playerControl->buttonState();
  uint8_t oldState = playerState.buttonState;

  //We should store changes to button state even if the cursor hasn't moved
  if(newState != oldState)
  {
    playerState.buttonState = newState;
  }
  
  bool rotate = false;
  uint8_t rotation = (playerState.positionState & 0x80) >> 7;
  
  bool oldHPressed = (oldState & PlayerControl::HorizontalButton) == PlayerControl::HorizontalButton; 
  bool oldVPressed = (oldState & PlayerControl::VerticalButton)   == PlayerControl::VerticalButton;
  
  bool newHPressed = (newState & PlayerControl::HorizontalButton) == PlayerControl::HorizontalButton; 
  bool newVPressed = (newState & PlayerControl::VerticalButton)   == PlayerControl::VerticalButton;


  if(newHPressed != oldHPressed || newVPressed != oldVPressed)
  {  
    //Encoder is newly pressed
    rotate = true;
    //Serial.println("Encoder has changed");
    if(newHPressed || newVPressed)
    {
      //Only actually change the rotation if the button is down
      if(rotation == ROTATION_HORIZONTAL)
      {
        //Serial.println("Now vertical");
        rotation = ROTATION_VERTICAL;  
      }
      else
      {
        //Serial.println("Now horizontal");
        rotation = ROTATION_HORIZONTAL;  
      }
    }
  }
  
  int8_t hPos = playerControl->hEncoder.read() / 4; //Each encoder 'click' is 4 readable positions
  int8_t vPos = playerControl->vEncoder.read() / 4;
  
  //Check to see if the new position is valid
  //Check that we've not gone off the left or top edges
  if(hPos < 0)
  { 
    hPos = 0;
    //As well as changing the local variable to stop us falling off the edge, we have to update that value on the encoder itself
    playerControl->hEncoder.write(0);
  }
  
  if(vPos < 0)
  {
    vPos = 0;
    playerControl->vEncoder.write(0);
  }

  //Check that we're not over the right or bottom boundaries
  uint8_t shipSize = playerState.selectedShip >> 4;
  if(rotation == ROTATION_HORIZONTAL)
  {
    if(hPos + shipSize > 8)
    {
      hPos = 8 - shipSize;
      //When we write a value back to the encoder, we need to *4 as the encoder
      //actually has 4 positions for each click
      playerControl->hEncoder.write(hPos * 4);
    }
    if(vPos > 7)
    {
      vPos = 7;   
      playerControl->vEncoder.write(vPos * 4);
    }
  }
  else //ROTATION_VERTICAL
  {
    if(hPos > 7)
    {
      hPos = 7;
      playerControl->hEncoder.write(hPos * 4);
    }
     
    if(vPos + shipSize > 8)
    {
      vPos = 8 - shipSize;
      playerControl->vEncoder.write(vPos * 4);
    }
  }

  //Update the new position in the player's state
  uint8_t oldPos = playerState.positionState & 0x3F;  //Take a copy of the position
  
  //Compare our normalised position against what we had last time
  uint8_t newPos = (vPos * 8) + hPos;
  
  //Check to see if we have moved the cursor
  if(!rotate && oldPos == newPos)
  {
    //We've not moved
    return false;  
  }

  //We have moved or the buttons are different, so we need to process
  //Clean the positionState and add the rotation bit
  //Serial.print("hpos : ");
  //Serial.print(hPos);
  //Serial.print(" vPos : ");
  //Serial.println(vPos);
  
  playerState.positionState = (rotation << 7);
  //We need to see if any part of this proposed position overlaps with another ship (or non-water square)
  //If the ship we're moving has a length of 1, then this is actually an attack phase and not setup phase
  //so use the correct board to test
  bool valid = false;
  if(shipSize == 1)
  {
    Serial.println("testing valid pos for attack");
    valid = isValidPosition(newPos, trackingBoard);
  }
  else
  {
    valid = isValidPosition(newPos, playerBoard);
  }
  
  //Write the validity bit
  if(valid)
  {
    playerState.positionState |= 0x40;
    //Serial.println("Position is valid");
  }
  else
  {
    playerState.positionState &= 0xBF;
    //Serial.println("Position is not valid");
  }

  //Serial.println("Writing state");
  //Serial.print("Writing new pos as : ");
  //Serial.println(newPos & 0x3F);
  
  //Write the new position to the position state, but don't overwrite the first two bits
  playerState.positionState = (playerState.positionState & 0xC0) + (newPos & 0x3F);
  //Serial.print("Writing new position as : ");
  //Serial.println(playerState.positionState, HEX);
  
  //Store the current state as old state 
  playerState.controlState = (hPos & 0x07) << 5;
  playerState.controlState |= newHPressed << 4;
  playerState.controlState |= (vPos & 0x07) << 1;
  playerState.controlState |= newVPressed;

  //Serial.print("Writing new control as : ");
  //Serial.println(playerState.controlState, HEX);
  
  //Return true if the position has changed
  return true;
 
}

/**
 * The BattleshipController can request the state of this player so it can update the screens
 */
PlayerState* BattleshipsHumanPlayer::getState()
{
  return &playerState;  
}

/**
 * Moves the internal state onto the next ship for placement
 */
void BattleshipsHumanPlayer::incrementState()
{
  //Get the current ship type
  uint8_t currentShip = playerState.selectedShip & 0x0F;

  if(currentShip == DEATHSTAR) return;
  
  //Calculate the ship index as /2 - 2, so CARRIER is 4, gives 0
  uint8_t shipIndex = (currentShip /2) - 2;
  playerState.shipPositions[shipIndex] = playerState.positionState;
  
  //Increment shipIndex to move onto the next one
  shipIndex++;

  /*Serial.print("New ship has index of : ");
  Serial.print(SHIPORDER[shipIndex], HEX);
  Serial.print(" and a length of : ");
  Serial.println(SHIPSIZES[shipIndex]);*/

  //Now reset the state
  playerState.selectedShip = (SHIPSIZES[shipIndex] << 4) + SHIPORDER[shipIndex];
  playerState.positionState &= 0xBF; //Immediately update the position as invalid to prevent repeated placement
}

/**
 * Returns true if the current state matches the button pattern, but the last state did not
 */
bool BattleshipsHumanPlayer::hasPressedButton(uint8_t button)
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

/**
 * Reset the local state of this player and get ready for a new game
 */
void BattleshipsHumanPlayer::reset()
{
  playerState.selectedShip   = (SHIPSIZES[0] << 4) + CARRIER; //5 section long carrier
  playerState.positionState  = 0x40; // Horizontal, valid (not clashing), and index 0 is 0100 0000 or 0x40 or 64  
  playerState.buttonState    = 0x07; // All buttons pressed.  This means that we can flush any existing button presses at the start of the game

  for(int i=0; i<6; i++) playerState.shipPositions[i] = 0;  
  
  playerControl->hEncoder.write(0);
  playerControl->vEncoder.write(0);

  remainingShips = 17;
}

bool BattleshipsHumanPlayer::isValidPosition(uint8_t initialPos, uint8_t* board)
{
  uint8_t shipSize = playerState.selectedShip >> 4;
  uint8_t rotation = (playerState.positionState & 0x80) >> 7;
  uint8_t indexNumber = initialPos; 
  boolean valid = true;
  
  for(int i=0; i < shipSize; i++)
  { 
    //Abort if not valid;
    if(valid == false)
      continue;
      
    //Serial.print("Square ");
    //Serial.print(indexNumber);
    //Serial.print(" has an index number of ");
    //Serial.println(*(board + indexNumber));
    
    if(*(board + indexNumber) != WATER)
    {
      //Serial.println("Square is not water");
      valid = false;
    }
    
    //Move the index onto the next part of this ship
    if(rotation == ROTATION_VERTICAL)
    { 
      indexNumber += 8;
    }
    else  //  VERTICAL
    {
      indexNumber++;
    }
  }  

  return valid;
}

void BattleshipsHumanPlayer::hit()
{
  remainingShips--; 
  Serial.print("You have ");
  Serial.print(remainingShips);
  Serial.println(" cells remaining");
}

bool BattleshipsHumanPlayer::isDestroyed()
{
  return remainingShips == 0;
}

void BattleshipsHumanPlayer::debugPositions()
{
  Serial.println("Ship placed positions");
  for(int i =0; i <6; i++)
  {
    Serial.print("Ship ");
    Serial.print(i);
    Serial.print(" value ");
    Serial.print(playerState.shipPositions[i]);
    uint8_t pos = playerState.shipPositions[i] & 0x3F;
    Serial.print(" pixel 0x");
    if(pos < 0x10) Serial.print("0");
    Serial.println(pos, HEX);
  }  
}
