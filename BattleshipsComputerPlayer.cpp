/***
 * Implmentation of BattleshipsPlayer for a ARTIFICIALLY INTELLIGENT player
 */

#include "BattleshipsComputerPlayer.h"

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3

#define COMPUTERDELAY 500

BattleshipsComputerPlayer::BattleshipsComputerPlayer()
{
  Serial.println("Computer player starting");
  //Reset all boards back to 0
  for(int i=0; i< 64; i++)
  {
    playerBoard[i]   = WATER;
    trackingBoard[i] = WATER;
  }

  reset();
}

BattleshipsComputerPlayer::~BattleshipsComputerPlayer();

/**
 * Returns a pointer to this players board
 */
uint8_t* BattleshipsComputerPlayer::getBoard()
{
  return &(playerBoard[0]);  
}

/**
 * Returns a pointer to this players tracking board
 */
uint8_t* BattleshipsComputerPlayer::getTrackingBoard()
{
  return &(trackingBoard[0]);  
}


/**
 * Returns true if the encoders are in a different position from last time.  Additionally this method will check boundary position of the cursor
 */
bool BattleshipsComputerPlayer::hasMoved()
{
  if(readyTime > millis())
  {
    //Not moved yet
    return false;  
  }

  Serial.println("Starting computer move");    
  Serial.print("Position state at start of move 0x");
  Serial.println(playerState.positionState, HEX);
  
  
  //Time to move
  if(selectedCell < 0)
  {
    Serial.println("Choosing a new cell");
    Serial.print("Position state before new target 0x");
    Serial.println(playerState.positionState, HEX);
    //No cell has been selected yet to move towards.  Select one
    selectTargetCell();
    Serial.print("Position state after target 0x");
    Serial.println(playerState.positionState, HEX);
  }

  /*
  Serial.print("Target cell ");
  Serial.println(selectedCell);
  Serial.print("Ship is aligned ");
  if((playerState.positionState & 0x80) == 0x80)
    Serial.println("vertically");
  else
    Serial.println("horizontally");
*/
  //Move towards the target cell.  Get the current cell position,
  //and calculate the vertical and horizontal distance between the current 
  //and target positions
  uint8_t currentPos = playerState.positionState & 0x3F;
      
  int hDiff = (currentPos % 8) - (selectedCell % 8);
  int vDiff = (currentPos / 8) - (selectedCell / 8);

  //Serial.print("hDiff ");
  //Serial.print(hDiff);
  //Serial.print(" vDiff ");
  //Serial.println(vDiff);

  Serial.print("Current Pos: ");Serial.print(currentPos); Serial.print(" Selected Pos: "); Serial.println(selectedCell);

  if(currentPos == selectedCell)
    return true;

  Serial.print("I think that I should move ");
  if(abs(vDiff) >= abs(hDiff))
  {
    Serial.print("vertically ");
    //Move vertically
    if(vDiff > 0)
    {
      Serial.println("up.");
      //Move up  
      currentPos -= 8;
    }
    else
    {
      Serial.println("down.");
      //Move down  
      currentPos += 8;
    }
  }
  else
  {
    Serial.print("horizontally ");
    //Move horizontally
    if(hDiff > 0)
    {
      //Move left  
      Serial.println("left.");     
      currentPos --;
    }
    else
    {
      //Move right
      Serial.println("right.");
      currentPos ++;
    }
  }

  if(selectedCell == currentPos)
    Serial.println("I've reached the target");
 
  uint8_t shipSize = playerState.selectedShip >> 4;
  
  //We need to see if any part of this proposed position overlaps with another ship (or non-water square)
  //If the ship we're moving has a length of 1, then this is actually an attack phase and not setup phase
  //so use the correct board to test
  bool valid = false;
  if(shipSize == 1)
  {
    valid = isValidPosition(currentPos, trackingBoard);
  }
  else
  {
    valid = isValidPosition(currentPos, playerBoard);
  }
  
  //Write the validity bit
  if(valid)
  {
    playerState.positionState |= 0x40;
   // Serial.println("Position is valid");
  }
  else
  {
    playerState.positionState &= 0xBF;
   // Serial.println("Position is not valid");
  }

  //Write the new position to the position state, but don't overwrite the first two bits
  playerState.positionState = (playerState.positionState & 0xC0) + (currentPos & 0x3F);
  //Serial.print("Writing new position as : ");
  //Serial.println(playerState.positionState, HEX);

  //Add a delay to make the decision in human time
  readyTime = millis() + COMPUTERDELAY;
  
  //Return true if the position has changed
  return true;
}

/**
 * The BattleshipController can request the state of this player so it can update the screens
 */
PlayerState* BattleshipsComputerPlayer::getState()
{
  return &playerState;  
}

/**
 * Moves the internal state onto the next ship for placement
 */
void BattleshipsComputerPlayer::incrementState()
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
  selectedCell = -1;
}

/**
 * Returns true if the current state matches the button pattern, but the last state did not
 */
bool BattleshipsComputerPlayer::hasPressedButton(uint8_t button)
{
  if(readyTime > millis())
  {
    //Not moved yet
    return false;  
  }

  Serial.print("Current Pos: ");Serial.print(playerState.positionState & 0x3F); Serial.print(" Selected Pos: "); Serial.println(selectedCell);

  //If our current position is the same as the target position, press the button
  if(selectedCell == (playerState.positionState & 0x3F))
  {
    Serial.println("Computer player is firing");
    //Clear the selected cell
    lastSuccessfulCell = selectedCell; //If we are successful, 
    lastPositionHit = false;           //then lastPositionHit will be reset to true;
    selectedCell = -1;
    return true;  
  }
  else
  {
    return false;  
  }
}

/**
 * Selects the cell we are going to move to, if none is selected.  This could be for placing a ship or for attacking the other player
 */
void BattleshipsComputerPlayer::selectTargetCell()
{
  Serial.print("Have we already hit a location? ");Serial.println(lastPositionHit);
  
  //If we just hit a position last time then this must be an attack round
  if(lastPositionHit)
  {
    Serial.println("Looking for an adjecent cell");
    //Select the next cell in the current search direction
    selectedCell = moveInDirection(lastSuccessfulCell, searchDirection);
    
    if(!isValidPosition(selectedCell, trackingBoard))
    {
      //Were going to enter a loop in a moment looking for an adjecent cell.  We just
      //need a moment to reset the search direction
      searchDirection = NORTH;   
      selectedCell = moveInDirection(lastSuccessfulCell, searchDirection);
    }
        
    while(!isValidPosition(selectedCell, trackingBoard))
    {
      // Try again
      searchDirection++;
      if(searchDirection > WEST)
      {
        searchDirection = NORTH;
        //We've been all around, look for another damaged cell
        findTargetFromDamaged();
      }
      else
      {
        //We've not been all around the compass yet, so try the next direction
        selectedCell = moveInDirection(lastSuccessfulCell, searchDirection);
      }
    }

    Serial.print("Last successful attack was at ");
    Serial.println(lastSuccessfulCell);
    Serial.print("Found a cell to continue an attack: ");
    Serial.println(selectedCell);
  }  
  else
  {
    //Can we find a cell that we've hit previously?
    //Choose a new location at random, this might or might not be an attack round
    selectedCell = random(0, 64);
    uint8_t shipSize = playerState.selectedShip >> 4;
    uint8_t* board = trackingBoard;
    if(shipSize > 1)
    {
      //Serial.print("Selecting location for a ship with the length ");
      //Serial.println(shipSize);
      
      //We're working out where to place a new ship during setup. 
      //Set the rotation randomly (and stick with it for this ship);
      uint8_t rotation = random(0, 64) % 2;
      //Serial.print("Rotation: ");
      //Serial.println(rotation);
        
      playerState.positionState += ((rotation << 7) & 0x80);

      board = playerBoard;
    } 
    else
    {
      Serial.println("Looking for a location to attack");  
      findTargetFromDamaged();
    }

    //Failsafe - just starting picking random locations
    while(!isValidPosition(selectedCell, board))
    {
      Serial.println("Failsafe - picking random location");
       //Reselect a new one then
       selectedCell = random(0, 64);
    }
    /*
    Serial.print("Selected cell ");
    Serial.println(selectedCell);

    Serial.println("I think that this position translates to");
    Serial.print("Row: ");
    Serial.print((selectedCell / 8) + 1);
    Serial.print(" Col: ");
    Serial.println((selectedCell % 8) + 1);
    */
    //delay(10000);
  }
}

void BattleshipsComputerPlayer::findTargetFromDamaged()
{
  Serial.println("We've search all the adjecent cells, lets look for damaged cells");
  //In C++ we can't return an array, so this method has been inlined
  
  //Get an array of all DAMAGED cells
  int c = 0;
  for(int i=0; i<63;i++)
  {
    if(trackingBoard[i] == DAMAGED)
      c++;  
  }

  Serial.print("Found "); Serial.print(c);Serial.println(" damaged but not destroyed cells ");

  if(c > 0)
  {
    int locs[c + 1]; //+1 for end indicator
  
    c = 0;
    for(int i=0; i<63;i++)
    {
      if(trackingBoard[i] == DAMAGED)
      {
        locs[c++] = i;  
      }  
    }
    //Add the end indicator
    locs[c] = -1;

    //reset c and iterate the array, looking for a good target
    bool found = false;
    c = 0;
    while(!found && locs[c] > -1)
    {
      //Check to see if this cell has attackable cells adjecent to it
      int newCell = evaluateTarget(locs[c]);
      if(newCell != locs[c])
      {
        //We've found a damaged cell we can continue from
        Serial.println("Found an old DAMAGED position");
        selectedCell = newCell;
        found = true;  
      }
      c++;  
    }

    if(!found)
    {
      //Time to pick a new cell at random
      lastPositionHit = false;
      selectedCell = random(0, 64);
    }
  }  
}

/**
 * Examines the adjecent cells to this one, and returns the location of an attackable cell adjecent. 
 * If no adjecent cell is suitable, returns the original position
 */
int BattleshipsComputerPlayer::evaluateTarget(int pos)
{
  Serial.print("Evaluating position ");
  Serial.println(pos);
  //Lets do it the stupid way...
  if(pos > 7 && trackingBoard[pos - 8] == WATER)
  {
    Serial.println("Found water to the north");
    return pos-8;
  }
  else if(pos < 57 && trackingBoard[pos + 8] == WATER)
  {
    Serial.println("Found water to the south");
    return pos+8;  
  }
  else 
  { 
    int hPos = pos % 8;
    Serial.print("Pos ");Serial.print(pos);Serial.print(" has an hPos of "); Serial.println(hPos);
    if(hPos > 0 && trackingBoard[pos - 1] == WATER)
    {
      Serial.println("Found water to the west"); 
      return pos - 1;  
    }
    else if(hPos < 7 && trackingBoard[pos + 1] == WATER)
    {
      Serial.println("Found water to the east");
      return pos + 1;
    }
  }
  
  return pos;
}

int BattleshipsComputerPlayer::moveInDirection(int oldPos, int newDir)
{
  int newPos = oldPos;
  switch(newDir)
  {
    case NORTH:
      newPos -= 8;
      break;
    case SOUTH:
      newPos += 8;
      break;
    case EAST:
      newPos += 1;
      break;
    case WEST:
      newPos -= 1;
      break;
  }

  return newPos;
}

/**
 * Reset the local state of this player and get ready for a new game
 */
void BattleshipsComputerPlayer::reset()
{
  playerState.selectedShip   = (SHIPSIZES[0] << 4) + CARRIER; //5 section long carrier
  playerState.positionState  = 0x40; // Horizontal, valid (not clashing), and index 0 is 0100 0000 or 0x40 or 64  
  playerState.buttonState    = 0x00; 
  
  for(int i=0; i<6; i++) playerState.shipPositions[i] = 0;  
  
  remainingShips = 17;

  selectedCell = -1;
  lastPositionHit = false;
  lastSuccessfulCell = -1;
  
  Serial.print("Reset cell position ");
  Serial.println(selectedCell);

  readyTime = 0;
}

bool BattleshipsComputerPlayer::isValidPosition(int initialPos, uint8_t* board)
{
  uint8_t shipSize = playerState.selectedShip >> 4;
  uint8_t rotation = (playerState.positionState & 0x80) >> 7;
  uint8_t indexNumber = initialPos; 
  boolean valid = true;

  if(initialPos > 63 || initialPos < 0) return false;

  //Check length and rotation
  if(shipSize > 1)
  {
    if(rotation == 1)
    {
      Serial.println("Validating vertical ship");
      if((initialPos / 8) + shipSize > 7)
      {
        Serial.println("This position is too close to the bottom of the screen");  
        return false;
      }
    }
    else
    {
      Serial.println("Validating horizontal ship");
      if((initialPos % 8) + shipSize > 7)
      {
        Serial.println("This position is too close to the right of the screen");  
        return false;
      }
    }
  }
  
  for(int i=0; i < shipSize; i++)
  { 
    if(indexNumber < 0 || indexNumber > 63)
      valid = false;
      
    //Abort if not valid;
    if(valid == false)
      continue;

   // Serial.print("Square ");
   // Serial.print(indexNumber);
   // Serial.print(" has an index number of ");
   // Serial.println(*(board + indexNumber));
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

void BattleshipsComputerPlayer::hit()
{
  remainingShips--; 
  Serial.print("You have ");
  Serial.print(remainingShips);
  Serial.println(" cells remaining");
}


void BattleshipsComputerPlayer::opponentHasBeenHit()
{
  lastPositionHit = true;
}

void BattleshipsComputerPlayer::opponentShipDestroyed()
{
  //Choose a new cell next time
  selectedCell = -1;  
  //Don't try to continue to hit this ship
  lastPositionHit = false;
}
    
bool BattleshipsComputerPlayer::isDestroyed()
{
  return remainingShips == 0;
}

void BattleshipsComputerPlayer::debugPositions()
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
