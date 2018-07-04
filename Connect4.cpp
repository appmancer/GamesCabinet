/*
 * Connect4
 * by 
 * Samuel Pickard
 */
#include "Connect4.h"
#include "Connect4Player.h"
#include "Connect4ComputerPlayer.h"
#include "Connect4HumanPlayer.h"

Connect4::Connect4(Cabinet* cab) : GameBase (cab)
{
  Serial.println(F("Starting Connect4"));
  player1 = NULL;
  player2 = NULL;
  reset();
}

Connect4::~Connect4()
{}

void Connect4::reset()
{
  uint16_t textCol = mCabinet->p1SFX.Color(255, 0, 0);
  mCabinet->p1SFX.setTextColor(textCol);
  mCabinet->p2SFX.setTextColor(textCol);

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
  currentPhase = CONNECT4_NONE;
  selectedColumn = 0;

  //Reset the board
  for(int i=0; i<BOARDSIZE; i++)
  {
    board[i] = 0;
  }

  for(int i=0; i<7; i++)
  {
    winningPieces[i] = -1;  
  }
  debugBoard();
  Serial.println(F("Connect4 reset and ready"));
}

void Connect4::gameloop()
{
    switch(currentPhase)
    {
      case CONNECT4_PREGAME:
         updatePregame();
         break; 
      case CONNECT4_WINNER:
         updateWinner();
         break;
      case CONNECT4_EFFECTS:
         updateEffects();
         break;
      case CONNECT4_ATTACK:  
         updateMainGame();
         break;
      case CONNECT4_CHANGE_PLAYER:
         startMove();
         break;
      case CONNECT4_NONE:
        //Serial.println("No status flag is set, starting pregame");
        showPregame();
        break;
    }
}

void Connect4::startDemo()
{
  mDemoMode = true;
  player1 = new Connect4ComputerPlayer();
  player1->setBoard(&board[0], PLAYER_1_COUNTER);
  player2 = new Connect4ComputerPlayer();
  player2->setBoard(&board[0], PLAYER_2_COUNTER);

  //Set the screens to all sea green
  mCabinet->p1SFX.fillScreen(BLACK);
  mCabinet->p1SFX.show();
  mCabinet->p2SFX.fillScreen(BLACK);
  mCabinet->p2SFX.show();
 
  //Start with player1
  currentPlayer = player2;
  readyTime = millis() + 1000;
  currentPhase = CONNECT4_CHANGE_PLAYER;
}

//Setup the pregame show
void Connect4::showPregame()
{
  currentPhase = CONNECT4_PREGAME;
  //Scroll the game name

  static const char* gameName = "Connect 4";
  mCabinet->p1SFX.scrollOnce(gameName);
  mCabinet->p2SFX.scrollOnce(gameName);
}

void Connect4::updatePregame()
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
        player2 = new Connect4ComputerPlayer();
        player2->setBoard(&board[0], PLAYER_2_COUNTER);
     }
     else
     {
        player1 = new Connect4HumanPlayer(&(mCabinet->p1Control));
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
        player1 = new Connect4ComputerPlayer();
        player1->setBoard(&board[0], PLAYER_1_COUNTER);
     }
     else
     {
        player2 = new Connect4HumanPlayer(&(mCabinet->p2Control));
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
    mCabinet->p1SFX.fillScreen(BLACK);
    mCabinet->p1SFX.show();
    mCabinet->p2SFX.fillScreen(BLACK);
    mCabinet->p2SFX.show();
   
    //Start with player1
    currentPlayer = player2;
    readyTime = millis() + 1000;
    currentPhase = CONNECT4_CHANGE_PLAYER;
  }
}

void Connect4::startMove()
{
  if(readyTime > millis())
  {
    //Not time yet
    return;
  }

  if(currentPlayer == player1)
  {
    currentPlayer = player2;  
    playerColour = RED;
    counterColour = PLAYER_2_COUNTER;
    defendingLCD = &(mCabinet->p1Display);
    attackingLCD = &(mCabinet->p2Display);
  }
  else
  {
    currentPlayer = player1;  
    playerColour = YELLOW;
    counterColour = PLAYER_1_COUNTER;
    defendingLCD = &(mCabinet->p2Display);
    attackingLCD = &(mCabinet->p1Display);
  }
  
  currentPlayer->getState()->buttonState = 0;
  printMessage(attackingLCD, F("Your move"), F(""), 2, 3);
  printMessage(defendingLCD, F("Please wait"), F(""), 1, 4);

  drawCursor();

  //Allow the effects to show
  currentPhase = CONNECT4_ATTACK;  
}

void Connect4::updateMainGame()
{
  
  if(currentPlayer->hasMoved())
  {
    //Remove old cursor position
    mCabinet->p1SFX.setPixelColor(selectedColumn, BLACK);
    mCabinet->p2SFX.setPixelColor(selectedColumn, BLACK);

    drawCursor();
  }

  if(currentPlayer->hasPressedButton(PlayerControl::FireButton))
  {
    debugBoard();
    if(board[selectedColumn + 16] > 0)
    {
      //Serial.println("Column is full, exiting");  
    }
    else
    { 
      currentPhase = CONNECT4_EFFECTS;
      dropRow = selectedColumn;
    }
  }
}

void Connect4::drawCursor()
{
    //Draw new cursor
    selectedColumn = currentPlayer->getState()->controlState;
    mCabinet->p1SFX.setPixelColor(selectedColumn, playerColour);
    mCabinet->p2SFX.setPixelColor(selectedColumn, playerColour);
    
    mCabinet->p1SFX.show();
    mCabinet->p2SFX.show();  
}

void Connect4::updateEffects()
{
  if(readyTime > millis())
    return;

  uint32_t rowLength = 8;
  //Test the next position
  //Serial.print("Testing new position ");Serial.println(dropRow + rowLength);
  if(dropRow + rowLength >= BOARDSIZE)
  {
    //Serial.println("Reached the bottom of an empty column");  
  }
  else
  {
    //Serial.print("Next position colour ");
    //Serial.println(mCabinet->p1SFX.getPixelColor(dropRow + rowLength));
  }
  
  if(dropRow + rowLength >= BOARDSIZE || mCabinet->p1SFX.getPixelColor(dropRow + rowLength) > 0)
  {
    //Serial.println("Final position has been reached");
    //Serial.print("Setting square ");Serial.print(dropRow);Serial.print(" to ");Serial.println(counterColour);
    board[dropRow] = counterColour;
    debugBoard();
  
    //See if a player has won.  
    if(playerWon(board, dropRow))
    {
      startWinner();
    }
    else
    {
      //play sound
      mCabinet->audioController.playFileIndex(SFX_TICK);
      //Serial.println("Changing player");
      readyTime = millis() + 1000;
      currentPhase = CONNECT4_CHANGE_PLAYER;
    }
  }
  else
  {
    //Serial.println("Piece is still falling");  
    //Set the old position to black
    mCabinet->p1SFX.setPixelColor(dropRow, BLACK);
    mCabinet->p2SFX.setPixelColor(dropRow, BLACK);
    dropRow += rowLength;
    mCabinet->p1SFX.setPixelColor(dropRow, playerColour);
    mCabinet->p2SFX.setPixelColor(dropRow, playerColour);
    
    mCabinet->p1SFX.show();
    mCabinet->p2SFX.show();
    
    readyTime = millis() + DROP_DELAY;
  }
}

/*
 * This is where the magic happens
 * A victory is achieved when (NB origin is top left)
 * 1.  We test pieces horzontally, and find 3 consecutive matching pieces (+ the one we've started with).  Pos +/- 1
 * 2.  We test pieces vertically, and find 3 consecutive matching pieces. Pos +/- 8
 * 3.  We test pieces diagonally bottom left to top right Pos +/- 7
 * 4.  We test pieces diagonally top left to bottom right Pos +/- 9
 * 
 * Victory is achieved when we have 4, 5, 6, or 7 pieces in a line
 */
bool Connect4::playerWon(uint8_t testBoard[], uint32_t lastPosition)
{
  bool hasWon = false;

  //First test, horizontal
  int i = 0;

  if(testLine(lastPosition, 1, 1) >= 4)
  {
    hasWon = true;
    Serial.println(F("Horizontal Line is winner"));  
  }
  else if(testLine(lastPosition, 8, 0) >= 4)
  {
    hasWon = true;
    Serial.println(F("Vertical Line is winner"));  
  }
  else if(testLine(lastPosition, 7, 1) >= 4)
  {
    hasWon = true;
    Serial.println(F("Diagnonal top right/bottom left is winner"));  
  }
  else if(testLine(lastPosition, 9, 1) >= 4)
  {
    hasWon = true;
    Serial.println(F("Diagnonal top left/bottom right is winner"));  
  }
  
  readyTime = millis() + 1000;
  return hasWon;
}

void Connect4::startWinner()
{
  //play sound
  mCabinet->audioController.playFileIndex(SFX_WIN);
  currentPhase = CONNECT4_WINNER;

  printMessage(attackingLCD, F("You have won"), F("Congratulations!"), 2, 0);
  printMessage(defendingLCD, F("Better luck"), F("next time"), 2, 3);
}


void Connect4::updateWinner()
{
  if(readyTime > millis())
    return;  

  if(mDemoMode)
  {
    //Reset the chip
    Serial.println(F("Connect 4 resetting"));
    digitalWrite(RESET_PIN, HIGH);  
  }

  flashOn = !flashOn;
  uint32_t flashColour = flashOn ? playerColour : BLACK;
  for(int i=0; i<7; i++)
  {
    if(winningPieces[i] >= 0)
    {
      //Serial.print("Winning square : ");Serial.println(winningPieces[i]);
      mCabinet->p1SFX.setPixelColor(winningPieces[i], flashColour);
      mCabinet->p2SFX.setPixelColor(winningPieces[i], flashColour);    
      winnerLight->setPixelColor(0, flashColour);  
      mCabinet->p1SFX.show();
      mCabinet->p2SFX.show();
      winnerLight->show();
    }  
  }

  readyTime = millis() + 1000;
  if(player1->hasPressedButton(PlayerControl::FireButton) || player2->hasPressedButton(PlayerControl::FireButton))
  {
    reset();
  }
  else
  {
    readyTime = millis() + DROP_DELAY;
  }
}

uint8_t Connect4::testLine(uint8_t startPos, int8_t offset, int8_t expectedColumnChange)
{
  //This is an array containing the positions of the pieces in the winning combination.
  //This could be up to 7 pieces, if the piece just dropped in is a new piece
  //Often this will only be 4 pieces, so a value of 255 (&FF) fills in unneeded spaces
  for(int c = 0; c<7; c++)
  {
    winningPieces[c] = 0xFF;  
  }

  int8_t newPos = startPos;
  uint8_t lineLen = 0;
  boolean valid  = true;
  uint8_t winningPieceIndex = 0;
  //First test the negative line, i.e. subtract the offset and expect a negative column change
  while(valid)
  {
    int8_t oldCol = newPos % 8;
    newPos -= newPos;
    int8_t newCol = newPos % 8;
    if(newPos < 0) valid = false; 
    if(valid && board[newPos] != counterColour) valid = false;
    if(valid && (abs((oldCol - newCol) != expectedColumnChange))) valid = false;

    if(valid) winningPieces[winningPieceIndex++] = newPos;
  }

  //Add the newly dropped piece
  winningPieces[winningPieceIndex++] = startPos;  
  newPos = startPos;
  valid = true;
  //then test the positive line, adding the offset and expecting a positive column change
  while(valid)
  {
    int8_t oldCol = newPos % 8;
    newPos += offset;
    int8_t newCol = newPos % 8;
    if(newPos > BOARDSIZE) valid = false; 
    if(valid && board[newPos] != counterColour) valid = false;
    if(valid && (abs(oldCol - newCol) != expectedColumnChange)) valid = false;

    if(valid) winningPieces[winningPieceIndex++] = newPos;
  } 

  return winningPieceIndex;
}

void Connect4::debugBoard()
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
 
