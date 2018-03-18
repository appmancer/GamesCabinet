#include <Arduino.h>
#include "Connect4ComputerPlayer.h"


Connect4ComputerPlayer::Connect4ComputerPlayer()
{
  board = 0;  
  selectedColumn = 0xFF;
}

Connect4ComputerPlayer::~Connect4ComputerPlayer()
{}

bool Connect4ComputerPlayer::hasMoved()
{
  if(selectedColumn == 0xFF)
    choosePosition();
    
  if(readyTime < millis())
    return false;

  Serial.print("Aiming for column ");
  Serial.println(selectedColumn);

  if(playerState.controlState < selectedColumn)
  {
    playerState.controlState++;
    readyTime = millis() + 300;
    return true;  
  }
  else if(playerState.controlState > selectedColumn)
  {
    playerState.controlState--;
    readyTime = millis() + 300;
    return true;
  }
  else
    return false;  
}

Connect4PlayerState* Connect4ComputerPlayer::getState()
{
  return &playerState; 
}

bool Connect4ComputerPlayer::hasPressedButton(uint8_t button)
{
  //If we're at the right column, press fire
  if(selectedColumn == playerState.controlState)
  {
    Serial.println("Computer player is pressing fire");
    selectedColumn = 0xFF; //reset  
    return true;
  }
  else
    return false;
}

void Connect4ComputerPlayer::reset()
{
  selectedColumn = 0xFF;
  playerState.controlState = 0;
  playerState.buttonState = 0;  
}

void Connect4ComputerPlayer::setBoard(uint8_t* mainBoard, uint32_t counter)
{
  board = mainBoard;  
  counterColour = counter;
  reset();
  Serial.println("Board has been initialised");
}

uint8_t Connect4ComputerPlayer::choosePosition()
{ 
  uint8_t longestRow = 0;
  Serial.println("Connect 4 computer playing is looking for a valid move");

  if(board == NULL)
  {
    Serial.println("Board is null you nutter!");
    return 0xFF;  
  }
  
  //Iterate through all 8 columns
  for(uint8_t col =0; col < 8; col++)
  {
    //We have to work out the final place for this column
    uint8_t nextPos = col + 16; //we have 6 rows of gamespace, so start on the top valid position
    bool found = false;
    while(!found) //Look to see if the next position is black
    {
      if(board[nextPos + 8] > 0)
      {
        found = true;
      }
      else
      {
        nextPos += 8;
      }

      //Break out when we reach the end of the board
      if(nextPos+8 > 63)
        break;
    }
    
    if(nextPos == col + 16)
    {
      //this column is full
      Serial.print("Column ");Serial.print(col);Serial.println(" is full.");
    }
    else
    { 
      //Then see if this new position will win
      Serial.print("Column ");Serial.print(col);Serial.print(" settles at position ");Serial.println(nextPos);
  
      //Check the board at this position
      uint8_t thisLen = lineLength(nextPos);
      if(thisLen > longestRow)
      {
        //We have a new longest row
        Serial.print("New longest line found when using column ");Serial.println(col);
        selectedColumn = col;
        longestRow = thisLen;  
      }
    }
  }

  //If we've still not chosen (no line was better than 0), choose a random line
  if(selectedColumn == 0xFF)
  {
    selectedColumn = random(0, 8);
  }

  //We're ready to move, to enter a readyTime
  readyTime = millis() + 100;
  return selectedColumn;
}

// Returns the length of the longest line if you add a counter at the new position
uint8_t Connect4ComputerPlayer::lineLength(uint8_t nextCounter)
{
  //First test, horizontal
  int i = 0;
  int longest = 0;
  
  Serial.println("");Serial.print("Checking position ");Serial.println(nextCounter);

  longest = testLine(nextCounter, 1);
  Serial.print("\tHorizontal line length : ");
  Serial.println(longest);
  
  i = testLine(nextCounter, 8);
  Serial.print("\tVertical line length : ");
  Serial.println(i);
  if(i > longest) longest = i;
  
  i = testLine(nextCounter, 7);
  Serial.print("\ttop-right/bottom-left line length : ");
  Serial.println(i);
  if(i > longest) longest = i;
  
  i = testLine(nextCounter, 9);
  Serial.print("\ttop-left/bottom-right line length : ");
  Serial.println(i);
  if(i > longest) longest = i;

  return longest;
}

uint8_t Connect4ComputerPlayer::testLine(uint32_t lastPosition, uint32_t increment)
{
  uint32_t i = 0; //i is the number of counters in a row.
  uint32_t nextPos = lastPosition;
  
    
  //Serial.print("Testing for a line using increment ");
  //Serial.println(increment);

  //Serial.println("Checking to the left");
  //check to see if we've hit the left edge
  nextPos -= increment;
  while(nextPos > 15) //ignore the top two rows
  {
    int col = nextPos % 8;
    //Serial.print("Current colour ");Serial.println(counterColour);
    //Serial.print("Board value ");Serial.println(board[nextPos]);
    if(board[nextPos] == counterColour)
    {
      //found a matching piece
      //Serial.print("Found a match at position ");
      //Serial.println(nextPos);
      nextPos -= increment;
      i++;
    } 
    else
    {
      //Serial.println("Not a match, stopping search to left");
      break;
    }
    //Check for the left hand edge
    if(nextPos % 8 > col) //Now we've incremented, see if our column has increased (ie. we've wrapped on to the right)
    {
      //We've wrapped on the right hand side, quit
      //Serial.println("Left edge, continue search on right");
      break;  
    }
  }

  //Serial.println("Checking positions to the right");
  //Now check to see if there are any more pieces to the right
  nextPos = lastPosition + increment;
  while(nextPos < 64)
  {
    int col = nextPos % 8;
    //Serial.print("Checking position ");
    //Serial.println(nextPos);
    //Serial.print("Current colour ");
    //Serial.println(counterColour);
    if(board[nextPos] == counterColour)
    {
      //found a matching piece
      //Serial.print("Found a match at position ");
      //Serial.println(nextPos);
      nextPos += increment;
      i++;
    } 
    else
    {
      //Serial.println("Not a match, stopping search to right");
      break;
    } 

    //Check for the left hand edge, unless were testing vertically
    if(nextPos % 8 < col) //Now we've incremented, see if our column has increased (ie. we've wrapped on to the left)
    {
      //We've wrapped on the right hand side, quit
      //Serial.println("Right edge, aborting");
      break;  
    }
  }

  return i;
}
