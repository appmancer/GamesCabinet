/*
 * Reversi
 * by 
 * Samuel Pickard
 */
#include "Reversi.h"
#include "ReversiPlayer.h"
#include "ReversiComputerPlayer.h"
#include "ReversiHumanPlayer.h"

Reversi::Reversi(Cabinet* cab) : GameBase (cab, "Reversi")
{
  Serial.println(F("Starting Reversi"));
  player1 = NULL;
  player2 = NULL;

  //Traditional colours
/*  player1Colour = WHITE;
  player2Colour = BLACK;
  boardColour   = GREEN;
  cursorInvalid = MAROON;*/


  //Alternative colours
  player1Colour = YELLOW;
  player2Colour = BLUE;
  boardColour   = BLACK;
  cursorInvalid = DARKSALMON;

  reset();
}

Reversi::~Reversi()
{}

void Reversi::reset()
{
  uint16_t textCol = mCabinet->p1SFX.Color(255, 0, 0);
  mCabinet->p1SFX.setTextColor(textCol);
  mCabinet->p2SFX.setTextColor(textCol);

  mCabinet->p1TopLight.setPixelColor(0, PURPLE);
  mCabinet->p1TopLight.show();
 
  mCabinet->p2TopLight.setPixelColor(0, PURPLE);
  mCabinet->p2TopLight.show();

  //Reset the setup states
  
  if(player1 != NULL)
  {
    delete player1;
    player1 = NULL;
  }
  
  if(player2 != NULL)
  {
    delete player2;
    player2 = NULL;
  }
  
  currentPlayer = PLAYER1;
  currentPhase = REVERSI_NONE;
  moveCounter = 0;
  nextFlippedCounter = 0;
    
  //Reset the board
  for(int i=0; i<BOARDSIZE; i++)
  {
    board[i] = 0;
    validMoves[i] = 0xFF;
    flippedCounters[i] = 0xFF;
  }

  //Put in the start counters;
  board[27] = PLAYER_1_COUNTER;
  board[28] = PLAYER_2_COUNTER;
  board[35] = PLAYER_2_COUNTER;
  board[36] = PLAYER_1_COUNTER;

  moveCounter = 4; //We've already made the first four moves!

 // TODO:: Del me
 // copyArray(stuckCorner, board);
 // moveCounter = 61;
  
  drawBoard();
  Serial.println(F("Reversi reset and ready"));
}

void Reversi::gameloop()
{
    switch(currentPhase)
    {
      case REVERSI_PREGAME:
         updatePregame();
         break; 
      case REVERSI_WINNER:
         updateWinner();
         break;
      case REVERSI_EFFECTS:
         updateEffects();
         break;
      case REVERSI_ATTACK:  
         updateMainGame();
         break;
      case REVERSI_CHANGE_PLAYER:
         startMove();
         break;
      case REVERSI_NONE:
        Serial.println(F("No status flag is set, starting pregame"));
        showPregame();
        break;
    }
}

//Setup the pregame show
void Reversi::showPregame()
{
  currentPhase = REVERSI_PREGAME;
  //Scroll the game name
 
  mCabinet->p1TopLight.setPixelColor(0, PURPLE);
  mCabinet->p1TopLight.show();
 
  mCabinet->p2TopLight.setPixelColor(0, PURPLE);
  mCabinet->p2TopLight.show();

  mCabinet->p1SFX.scrollOnce(mGameName);
  mCabinet->p2SFX.scrollOnce(mGameName);
}

void Reversi::updatePregame()
{
  //Update the P1 matrix
  if(mCabinet->p1SFX.updateSFX() || mCabinet->p2SFX.updateSFX() ) 
  {
    //Name has stopped scrolling, do something else!  
    mCabinet->p1SFX.larsenWipe(DARKGREEN);
    mCabinet->p2SFX.larsenWipe(DARKGREEN);
  }

  //Test to see if any of the player buttons have been pressed.
  uint8_t playerButtons = mCabinet->p1Control.buttonState();
  if(playerButtons > 0)
  {
     //Player 1 has pressed a button and wants to play
     if(player1 != NULL && player1->getState()->buttonState == 0)
     {
        //Player 1 is already in the game, so create a computer player2

        //Number of players = 0
        Serial.println(F("Player 1 is having a single player game.  Activate the robot!"));
        player2 = new ReversiComputerPlayer();
        player2->reset();
     }
     else
     {
        player1 = new ReversiHumanPlayer(&(mCabinet->p1Control));
        printMessage(&(mCabinet->p1Display), F("Waiting for"), F("other player"), 2, 2);
     }
  }

  //Keep a record of button state
  if(player1 != NULL)
  {
    player1->getState()->buttonState = playerButtons;  
  }
  
  playerButtons = mCabinet->p2Control.buttonState();
  if(playerButtons > 0)
  {
     //Player 2 has pressed a button and wants to play
     if(player2 != NULL && player2->getState()->buttonState == 0)
     {
        //Player 2 is already in the game, so create a computer player1
        Serial.println(F("Player 2 is having a single player game.  Activate the robot!"));
        player1 = new ReversiComputerPlayer();
        player1->reset();
     }
     else
     {
        player2 = new ReversiHumanPlayer(&(mCabinet->p2Control));
        printMessage(&(mCabinet->p2Display), F("Waiting for"), F("other player"), 2, 2);
 
     }
  }
  //Keep a record of button state
  if(player2 != NULL)
  {
    player2->getState()->buttonState = playerButtons;  
  }

  //When we've got two players, and all the buttons are released we can start
  if(player1 != NULL && player2 != NULL && mCabinet->p1Control.buttonState() == 0 && mCabinet->p2Control.buttonState() == 0)
  {
    //Set the screens to all sea green
    drawBoard();
    //Start with player1
    currentPlayer = player1;
    readyTime = millis() + 1000;
    currentPhase = REVERSI_CHANGE_PLAYER;
  }
}

void Reversi::drawBoard()
{
  debugBoard();
  
  for(uint8_t i = 0; i < BOARDSIZE; i++)
  {
    switch(board[i])
    {
      case PLAYER_1_COUNTER:
        mCabinet->p1SFX.setPixelColor(i, player1Colour);
        mCabinet->p2SFX.setPixelColor(i, player1Colour);
        break;
      case PLAYER_2_COUNTER:
        mCabinet->p1SFX.setPixelColor(i, player2Colour);
        mCabinet->p2SFX.setPixelColor(i, player2Colour);
        break;
      default:
        mCabinet->p1SFX.setPixelColor(i, boardColour);
        mCabinet->p2SFX.setPixelColor(i, boardColour);
        break;
    }  
  }  

  mCabinet->p1SFX.show();
  mCabinet->p2SFX.show();  
}

void Reversi::startMove()
{
  if(readyTime > millis())
  {
    //Not time yet
    return;
  }

  Serial.print(F("Board at start of move "));
  Serial.println(moveCounter);
  debugBoard();
  //debugFlipList();

  if(moveCounter == BOARDSIZE)
  {
    startWinner();
    return;
  }
  
  if(currentPlayer == player1)
  {
    currentPlayer = player2;  
    playerColour = player2Colour;
    counterColour = PLAYER_2_COUNTER;
    opponentColour = PLAYER_1_COUNTER;
    
    mCabinet->p1TopLight.setPixelColor(0, BLACK);
    mCabinet->p1TopLight.show();
    mCabinet->p2TopLight.setPixelColor(0, GREEN);
    mCabinet->p2TopLight.show();
    defendingLCD = &(mCabinet->p1Display);
    attackingLCD = &(mCabinet->p2Display); 
    winnerLight = &(mCabinet->p2TopLight);
  }
  else
  {
    currentPlayer = player1;  
    playerColour = player1Colour;
    counterColour = PLAYER_1_COUNTER;
    opponentColour = PLAYER_2_COUNTER;
    
    mCabinet->p1TopLight.setPixelColor(0, GREEN);
    mCabinet->p1TopLight.show();
    mCabinet->p2TopLight.setPixelColor(0, BLACK);
    mCabinet->p2TopLight.show();
    defendingLCD = &(mCabinet->p2Display);
    attackingLCD = &(mCabinet->p1Display);
    winnerLight = &(mCabinet->p1TopLight);
  }
  currentPlayer->getState()->buttonState = 0;

  //Reset valid moves
  for(uint8_t i = 0; i<BOARDSIZE; i++)
  {
    validMoves[i] = 0xFF;  
  }

  //Serial.println("Calculating valid moves");
  //Calculate valid moves
  uint8_t c = 0;
  for(uint8_t i = 0; i<BOARDSIZE; i++)
  {
    if(isLegal(i, opponentColour))
    {
      //Serial.print("Pos "); Serial.print(i); Serial.println(" is valid");
      validMoves[c++] = i;
    }
   /* else
    {
      Serial.print("Pos "); Serial.print(i); Serial.println(" is NOT valid");
    }*/
  }
  //Serial.print("Found ");Serial.print(c);Serial.println(" valid moves");
  //debugFlipList();

  //Special case - if there are no valid moves, then the player forgoes his turn
  if(c == 0)
  {
    Serial.println(F("No valid moves!"));
    debugBoard();
    
    printMessage(attackingLCD, F("You can't go!"), F("Miss this turn"), 2, 1);
    printMessage(defendingLCD, F("You have"), F("another go"), 4, 3);
    readyTime = millis() + 1000;
    currentPhase = REVERSI_CHANGE_PLAYER;
    return;
  }
  
  printMessage(attackingLCD, F("Your move"), F(""), 2, 3);
  printMessage(defendingLCD, F("Please wait"), F(""), 1, 4);

  drawCursor();

  if(currentPlayer->isComputer())
  {    
    //Serial.println("Computer player");
    uint8_t recMove = bestMove(counterColour);
    if(recMove == 0xFF)
    {
      Serial.println(F("No valid moves!"));
      debugBoard();
      
      printMessage(attackingLCD, F("You can't go!"), F("Miss this turn"), 2, 1);
      printMessage(defendingLCD, F("You have"), F("another go"), 4, 3);
      readyTime = millis() + 1000;
      currentPhase = REVERSI_CHANGE_PLAYER;
      return;
    }
    else
    {
      //Serial.print("Recommending move ");Serial.println(recMove);
      currentPlayer->recommendedMove(recMove); 
    }
  }

  //Allow the effects to show
  currentPhase = REVERSI_ATTACK;  
}

void Reversi::updateMainGame()
{
  if(currentPlayer->hasMoved())
  {
    cursorColour = boardColour;
    //Remove old cursor position
    if(board[selectedPosition] == PLAYER_1_COUNTER)
    {
      cursorColour = player1Colour;
    }
    else if(board[selectedPosition] == PLAYER_2_COUNTER)
    {
      cursorColour = player2Colour;
    }
    
    mCabinet->p1SFX.setPixelColor(selectedPosition, cursorColour);
    mCabinet->p2SFX.setPixelColor(selectedPosition, cursorColour);

    drawCursor();
  }

  if(currentPlayer->hasPressedButton(PlayerControl::FireButton))
  {
    if(cursorColour == playerColour)
    {
      
      mCabinet->audioController.playFileIndex(SFX_VALID);
      //Serial.println("Move has been selected");
      debugBoard();
      debugFlipList();
      //Work out which counters can be turned
      flipCounters(selectedPosition);
      
      moveCounter++;
      //We need to reset the flip counter for display
      
      //Add the new position to the board
      //Serial.println("Adding the new position to the board");
      //Serial.print("SelectedPostion:");Serial.print(selectedPosition);Serial.print(" Colour:");Serial.println(counterColour);
      board[selectedPosition] = counterColour;  
      //Reset the nextFlippableCounter as we will use it again to perform the flipping
      nextFlippedCounter = 0;
      //debugBoard();
      //debugFlipList();
      currentPhase = REVERSI_EFFECTS;
    }
    else
    {
      mCabinet->audioController.playFileIndex(SFX_INVALID);
      Serial.println("Button pressed but not valid position");
      Serial.print("Cursor: "); Serial.print(cursorColour, HEX); Serial.print(" PlayerColour:");Serial.println(playerColour, HEX);
    }
  }
}

void Reversi::drawCursor()
{
    //Draw new cursor
    selectedPosition = currentPlayer->getState()->controlState;

    cursorColour = cursorInvalid;
    //Check to see if this position in in the validMoves list
    uint8_t c = 0;
    while(c < BOARDSIZE && validMoves[c] < 0xFF)
    {
      if(validMoves[c] == selectedPosition)
      {
        //This position is in the list
        cursorColour = playerColour;
        //We can stop looking
        break;
      }
      c++;
    }
    mCabinet->p1SFX.setPixelColor(selectedPosition, cursorColour);
    mCabinet->p2SFX.setPixelColor(selectedPosition, cursorColour);
    
    mCabinet->p1SFX.show();
    mCabinet->p2SFX.show();  
}

//Positions are only valid if you can turn over another piece
//This means that the selected square is:
//  - A board square (not another counter) - very quick to test
//  - Adjecent to your opponents counter in any direction - not too slow
//  - At least of the opponents counters must then eventually have one of your counters behind it - slow
bool Reversi::isLegal(uint8_t testPos, uint8_t opponentCounter)
{
  //Serial.print("Checking square ");
  //Serial.println(testPos);
  
  //First test, check that this is an open square
  if(board[testPos] != BOARD_SQUARE)
  {
    //Serial.println("Not an open square - not legal");
    return false;  
  } 
  
  //Test adjecent squares.  Not found a quick way to do this yet
  bool foundOpponent = false;
  for(int i = 0; i<8; i++)
  {
    int8_t endPos = testPos + searchOrder[i];
    int8_t testRow = (endPos / 8) - (testPos / 8);
    /*Serial.print("Adj cell index: ");
    Serial.print(endPos);
    Serial.print(" Proposed row :");
    Serial.print(testPos / 8);
    Serial.print(" adjecent cell row: ");
    Serial.print(endPos / 8);
    Serial.print(" expected difference : ");
    Serial.print(rowOffsets[i]);
    Serial.print(" actual difference : ");
    Serial.print(testRow);*/

    if(endPos < 0 || endPos >= BOARDSIZE)
    {
      //Serial.println(" WRAPPED INVALID");  
    }
    else if(testRow == rowOffsets[i])
    {
      //Serial.println(" NOT WRAPPED");
      //We've not wrapped onto an unexpected row
      if(board[endPos] == opponentCounter)
      {
        //Serial.println("Found adjecent opponent counter, testing to see if we can flip it");
        //Serial.println(i);
        //We found an opponent, can we win it?
        foundOpponent = testLine(testPos, searchOrder[i], rowOffsets[i], counterColour) > 0;
        if(foundOpponent)
        {
          //Serial.println("Counter is flippable, add to valid moves"); 
          //We don't need to continue
          break; 
        }/*
        else
        {
          Serial.println("Sorry, you can't win this neighbour");
        }*/
      }
    }/*
    else
    {
      //Serial.println(" WRAPPED INVALID");  
    }*/
  }

  return foundOpponent;
}

void Reversi::updateEffects()
{
  if(readyTime > millis())
    return;

  //Serial.print("Flipping next counter ");Serial.println(flippedCounters[nextFlippedCounter]);

  //Flip the queued counters;
  if(nextFlippedCounter == BOARDSIZE || flippedCounters[nextFlippedCounter] == 0xFF)
  {
    //We've flipped all the counters, reset the array
    for(int i = 0; i < BOARDSIZE; i++)
    {
      flippedCounters[i] = 0xFF;  
    }  
    nextFlippedCounter = 0;

    //Serial.println("Move has completed");
    debugBoard();
    readyTime = millis() + 1000;
    currentPhase = REVERSI_CHANGE_PLAYER;
  }
  else
  {
    //Serial.print("Flipping square "); Serial.print(flippedCounters[nextFlippedCounter]);Serial.print(" Index: ");Serial.println(nextFlippedCounter);
    mCabinet->p1SFX.setPixelColor(flippedCounters[nextFlippedCounter], playerColour);
    mCabinet->p2SFX.setPixelColor(flippedCounters[nextFlippedCounter], playerColour);
 
    mCabinet->p1SFX.show();
    mCabinet->p2SFX.show();  

    //Update the board
    board[flippedCounters[nextFlippedCounter]] = counterColour;
  

    nextFlippedCounter++;
    readyTime = millis() + 300;    
  }
}

/*
 * This is where the magic happens
 * When a piece is played, we test north, south, east and west looking for the first
 * counter of the same colour, or a square of empty board.  If we find a counter before and empty space
 * then we add all the squares in between the two counters to an array, and let the effects processor
 * animate the flipping
 */
bool Reversi::flipCounters(uint32_t lastPosition)
{
  nextFlippedCounter = 0;
  int flipCount = 0;
  
  //I know that it looks like this could all be wrapped up, but when I am 
  //determining valid positions, then I need to test the position *without*
  //flipping counters, so its a two stage process here.  Count, then flip.
  for(int i = 0; i < 8; i++)
  {
  //  Serial.print("Testing pos ");Serial.print(lastPosition);Serial.print(" iteration: ");Serial.print(i);Serial.print(" direction: ");Serial.print(searchOrder[i]);Serial.print(" row offset: ");Serial.println(rowOffsets[i]);
    flipCount = testLine(lastPosition, searchOrder[i], rowOffsets[i], counterColour);
    if(flipCount > 0)
      flipSingleLine(lastPosition, searchOrder[i], flipCount);
  }
}

//This function calculates how many opponent counters are between lastPosition and another of the current players counters
uint32_t Reversi::testLine(uint32_t lastPosition, int8_t increment, int8_t rowDiff, uint8_t myCounter)
{
  /*Serial.print("Testing position: ");Serial.print(lastPosition);Serial.print(" with increment: "); Serial.print(increment);Serial.print(" and row offset ");Serial.println(rowDiff);
  Serial.print("Counter colour ; ");Serial.println(myCounter);
  if(PLAYER_1_COUNTER == myCounter)
    Serial.println("Player 1 counter");
  else
    Serial.println("Player 2 counter");*/
  
  //Work along the increment until we find a counter that matches the counter at lastPosition, or a value that matches BOARD_SQUARE
  int32_t nextPos = lastPosition + increment;
  uint32_t matchPos = 0xFF;
  uint32_t count = 0;
  while(nextPos >= 0 && nextPos < BOARDSIZE)
  {
    if(board[nextPos] == BOARD_SQUARE) break;

    //Check for left edge
    if(increment == EAST && nextPos % 8 == 0)
    {
      //Serial.println("Hit the right edge");
      break;
    }
    //Check for right edge
    if(increment == WEST && nextPos % 8 == 7)
    {
      //Serial.println("Hit the left edge");
      break;
    }

    //Check for the next pos not matching the current counter colour
    int8_t actualRowOffset = (nextPos / 8) - (lastPosition / 8);
    //Serial.print("Expected RowOffset:");Serial.print(rowDiff);Serial.print(" Actual: "); Serial.println(actualRowOffset);
    if(actualRowOffset == rowDiff)
    {
      //Serial.println("We're on the expected row");
   
      if(board[nextPos] != myCounter)
      {
        //Serial.print("Found opponent counter at ");Serial.println(nextPos);
        //We need to keep looking
        lastPosition = nextPos;
        nextPos += increment;  
        count++;
      }
      else
      {
        //We've found a matching piece, we can calculate the counter to flip now
        //Serial.print("Found my counter at ");Serial.println(nextPos);
        matchPos = nextPos;
        break;  
      }
    }
    else
    {
      //Wrapped over the edge onto an unexpected row.  If you've not found a matching counter, then you aren't going to turn this line
      //Serial.println("We've wrapped over the edge.");
      break;
    }
  }
  
  if(count > 0 && matchPos == 0xFF)
  {
    //Serial.print("Found ");Serial.print(count);Serial.println(" opponent counter BUT NO MATCHING COUNTER");
    count = 0;
  }/*
  else
  {
    Serial.print("Found ");Serial.print(count);Serial.println(" flippable counters");
  }*/
  
  return count;
}

void Reversi::flipSingleLine(uint32_t startPosition, int8_t increment, uint32_t count)
{
  //We've got some counters to flip.
  uint8_t nextCounter = startPosition + increment;
  //Serial.print("Start pos: "); Serial.print(startPosition);Serial.print(" has ");Serial.print(count);Serial.println(" flippable counters");
  for(int i = 0; i < count; i++)
  {
    //Serial.print("I'm going to flip square ");
    //Serial.println(nextCounter);
    flippedCounters[nextFlippedCounter++] = nextCounter; 
    nextCounter += increment;
  }
}

uint8_t Reversi::bestMove(uint8_t myCounter)
{
  //Calculate the position with the highest number of flipped counters;
  int i =0;
  uint8_t kingOfTheHill = 0;
  uint32_t highest = 0;
  debugFlipList();
  while(i < BOARDSIZE && validMoves[i] < 0xFF)
  {
    uint32_t runningTotal = 0;
    for(int j = 0; j<8; j++)
    {
      runningTotal += testLine(validMoves[i], searchOrder[j], rowOffsets[j], myCounter);
    }

    //Serial.print("Pos ");Serial.print(validMoves[i]);Serial.print(" scores ");Serial.println(runningTotal);
  
    if(runningTotal > highest)
    {
      highest = runningTotal;
      kingOfTheHill = validMoves[i];
      //Serial.print(kingOfTheHill); Serial.println(" is king of the hill");
    }

    //Don't forget to increment i, only a complete loser idiot would forget that
    i++;
  }

  if(highest == 0)
  {
    //No valid moves!  Return 0xFF to signify
    kingOfTheHill = 0xFF; 
  }

  return  kingOfTheHill;
}

void Reversi::startWinner()
{
  //play sound
  mCabinet->audioController.playFileIndex(SFX_WIN);
  //We need to know who won!
  uint8_t p1Count = 0;
  uint8_t p2Count = 0;
  for(int i = 0; i < BOARDSIZE; i++)
  {
    switch(board[i])
    {
      case PLAYER_1_COUNTER:
        p1Count++;
        break;
      case PLAYER_2_COUNTER:
        p2Count++;
        break;
      default:
        //Serial.println("Finished with unknown square");
        debugBoard();
        break;  
    }
  }

  Serial.print("P1: ");Serial.print(p1Count);Serial.print(" P2: ");Serial.println(p2Count);
  
  if(p1Count > p2Count)
  {
    Serial.print(F("Player 1 has won"));
    defendingLCD = &(mCabinet->p2Display);
    attackingLCD = &(mCabinet->p1Display);
    playerColour = player1Colour;
    counterColour = PLAYER_1_COUNTER;
  }
  else
  {
    Serial.print(F("Player 2 has won"));
    defendingLCD = &(mCabinet->p1Display);
    attackingLCD = &(mCabinet->p2Display);    
    playerColour = player2Colour;
    counterColour = PLAYER_2_COUNTER;
  }
  
  printMessage(attackingLCD, F("You have won"), F("Congratulations!"), 2, 0);
  printMessage(defendingLCD, F("Better luck"), F("next time"), 2, 3);

  flash = true;
  
  readyTime = millis() + 1000;
  currentPhase = REVERSI_WINNER;

}


void Reversi::updateWinner()
{
  if(player1->hasPressedButton(PlayerControl::FireButton) || player2->hasPressedButton(PlayerControl::FireButton))
  {
    reset();
  }

  if(readyTime > millis())
    return;

  flash = !flash;
  for(int i=0; i<BOARDSIZE; i++)
  {
    if(board[i] == counterColour)
    {
      if(flash)
      {
        mCabinet->p1SFX.setPixelColor(i, playerColour);
        mCabinet->p2SFX.setPixelColor(i, playerColour); 
      }
      else
      {
        mCabinet->p1SFX.setPixelColor(i, BLACK);
        mCabinet->p2SFX.setPixelColor(i, BLACK);
      }
    }
  }

  mCabinet->p1SFX.show();
  mCabinet->p2SFX.show();

  readyTime = millis() + 1000;
}

void Reversi::debugBoard()
{
  for(int i=0; i<BOARDSIZE; i++)
  {
    
    switch(board[i])
    {
      case   PLAYER_1_COUNTER:
        Serial.print("o");
        break;
      case   PLAYER_2_COUNTER:
        Serial.print("x");
        break;
      default:
        Serial.print(".");
    }  
    
    //Serial.print(board[i]);
    if(i % 8 == 7)
      Serial.print("\n");
  }
  
  Serial.println();
}

void Reversi::debugFlipList()
{
  Serial.println("Valid moves");
  for(int i=0; i<BOARDSIZE;i++)
  {
    Serial.print(validMoves[i]);Serial.print(" ");
    if(i % 8 == 7)
      Serial.println("");

    if(validMoves[i] == 0xFF)
    {
      Serial.println("");
      break;
    }
  }
  
  Serial.print("Next Flippable: ");Serial.println(nextFlippedCounter);
  for(int i=0; i<BOARDSIZE;i++)
  {
    Serial.print(flippedCounters[i]);Serial.print(" ");
    if(i % 8 == 7)
      Serial.println("");

    if(flippedCounters[i] == 0xFF)
    {
      Serial.println("");
      break;
    }
  }
}

void Reversi::copyArray(uint8_t* source, uint8_t* target)
{
  for(int i = 0; i < BOARDSIZE; i++)
  {
    *target++ = *source++;  
  }  
}
