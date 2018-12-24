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

  //Serial.print("Aiming for column ");Serial.println(selectedColumn);

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
    Serial.println(F("Computer player is pressing fire"));
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
  Serial.println(F("Board has been initialised"));
}

uint8_t Connect4ComputerPlayer::choosePosition()
{ 
  uint8_t longestRow = 0;
  Serial.println(F("Connect 4 computer playing is looking for a valid move"));

  if(board == NULL)
  {
    Serial.println(F("Board is null you nutter!"));
    return 0xFF;  
  }

  uint8_t validPositions[8];
  //Reset valid positions
  for(uint8_t c =0; c < 8; c++)validPositions[c] = 0xFF;
  uint8_t validPosCounter = 0;
  
  //Iterate through all 8 columns
  for(uint8_t col =0; col < 8; col++)
  {
    //We have to work out the final place for this column
    uint8_t nextPos = col + 8; //we have 6 rows of gamespace, so start on row above the top valid position
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
    
    if(nextPos == col + 8)
    {
      //this column is full
      //Serial.print("Column ");Serial.print(col);Serial.println(" is full.");
    }
    else
    { 
      //Then see if this new position will win
      //Serial.print("Column ");Serial.print(col);Serial.print(" settles at position ");Serial.println(nextPos);
      validPositions[validPosCounter++] = nextPos;
    }
  }

  //From the list of valid positions, choose the one with the longest line.
  uint8_t lineLengths[validPosCounter];
  uint8_t kingOfTheHill = 0;
  uint8_t kingCounter = 0;
  for(uint8_t c=0; c<validPosCounter; c++)
  {
    uint8_t thisLine = longestLine(validPositions[c], counterColour); 
    if(thisLine > kingOfTheHill)
    {
      kingOfTheHill = thisLine;
      kingCounter = 1;
    }
    else if(thisLine == kingOfTheHill)
    { 
      kingCounter++;
    } 
    lineLengths[c] = thisLine;
  }

  //Serial.print("There are "); Serial.print(kingCounter); Serial.print(" lines of length ");Serial.println(kingOfTheHill);
  uint8_t moves[kingCounter];
  uint8_t validMovesCounter = 0;
  for(int i=0; i<validPosCounter; i++)
  {
    if(lineLengths[i] == kingOfTheHill)
    {
      //If the move at validPostions[i] is one of the longest moves, then copy the position to moves
      moves[validMovesCounter++] = validPositions[i];  
    }
  }

  uint8_t blockOppo = 0xFF;
  if(kingOfTheHill < 4)
  {
    //Would this be a winning move for my opponent?  
    for(uint8_t c=0; c<validPosCounter; c++)
    {
      uint8_t oppoColour = counterColour == 2 ? 4 : 2;
      uint8_t thisLine = longestLine(validPositions[c], oppoColour); 
      if(thisLine > 3)
      {
        //Block the opposition
        blockOppo = validPositions[c] % 8;
      }
    }
  }

  //If one of these moves is the winning move...
  if(kingOfTheHill > 3)
  {
    //If there is only one longest move...
    if(validMovesCounter == 1)
    {
      selectedColumn = moves[0] % 8;
    }
    else
    {
      selectedColumn = moves[random(validMovesCounter)]  % 8;
    }
  }
  else
  {
     if(blockOppo < 0xFF)
     {
        //Stop the opponent
        selectedColumn = blockOppo; 
     }
     else
     {
        //Choose next best move from list
        selectedColumn = moves[random(validMovesCounter)]  % 8; 
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

uint8_t Connect4ComputerPlayer::longestLine(uint8_t pos, uint8_t myCounter)
{
  uint8_t maxLen = 0;
  uint8_t thisLen = 0;
  //Serial.print("Testing position ");Serial.println(pos);
  thisLen = testLine(pos, 9, 1, myCounter); //Diagonal, top-left to bottom-right \
  if(thisLen > maxLen) maxLen = thisLen;

  thisLen = testLine(pos, 8, 0, myCounter); //Straight up and down               |
  if(thisLen > maxLen) maxLen = thisLen;

  thisLen = testLine(pos, 7, 1, myCounter); //Diagonal, top-right to bottom-left /
  if(thisLen > maxLen) maxLen = thisLen;

  thisLen = testLine(pos, 1, 1, myCounter); //Horizontal                         —
  if(thisLen > maxLen) maxLen = thisLen;                              

  return maxLen;
}

uint8_t Connect4ComputerPlayer::testLine(uint8_t startPos, int8_t offset, int8_t expectedColumnChange, uint8_t myCounter)
{
  uint8_t counter = 1;
  int8_t newPos = startPos;
  uint8_t lineLen = 0;
  boolean valid  = true;
  uint8_t winningPieceIndex = 0;
  //First test the negative line, i.e. subtract the offset and expect a negative column change
  while(valid)
  {
    int8_t oldCol = newPos % 8;
    newPos -= offset;
    int8_t newCol = newPos % 8;

    //Debug position
    //Serial.print("Testing for counter at ");Serial.print(newPos);Serial.print(" Exp:");Serial.print(counterColour);Serial.print(" Act:");Serial.print(board[newPos]);
    //Serial.print(" Exp col:"); Serial.print(expectedColumnChange); Serial.print(" Act col:"); Serial.print(abs(oldCol-newCol));
    
    if(newPos < 0) valid = false; 
    if(valid && board[newPos] != myCounter) valid = false;
    if(valid && (abs(oldCol - newCol) != expectedColumnChange)) valid = false;
    if(valid) counter++;

    //if(valid) Serial.println(" VALID");
    //else      Serial.println(" INVALID");
  }
  newPos = startPos;
  valid = true;
  //then test the positive line, adding the offset and expecting a positive column change
  while(valid)
  {
    int8_t oldCol = newPos % 8;
    newPos += offset;
    int8_t newCol = newPos % 8;

    //Debug position
    //Serial.print("Testing for counter at ");Serial.print(newPos);Serial.print(" Exp:");Serial.print(counterColour);Serial.print(" Act:");Serial.print(board[newPos]);
    //Serial.print(" Exp col:"); Serial.print(expectedColumnChange); Serial.print(" Act col:"); Serial.print(abs(oldCol-newCol));
    
    if(newPos > 64) valid = false; 
    if(valid && board[newPos] != myCounter) valid = false;
    if(valid && (abs(oldCol - newCol) != expectedColumnChange)) valid = false;
    if(valid) counter++;

    //if(valid) Serial.println(" VALID");
    //else      Serial.println(" INVALID");
  } 

  return counter;
}
