/*
 * Noughts and Crosses
 * by 
 * Samuel Pickard
 * 
 * The algorithm for choosing a move is deliberately weakened to allow the human player to win sometimes ;-)
 * 
 */
#include "OnX.h"
#include "OnXPlayer.h"
#include "OnXComputerPlayer.h"
#include "OnXHumanPlayer.h"

OnX::OnX(Cabinet* cab) : GameBase (cab, "Noughts and Crosses")
{
  Serial.println(F("Starting OnX"));
  player1 = NULL;
  player2 = NULL;

  //Alternative colours
  player1Colour = YELLOW;
  player2Colour = BLUE;
  boardColour   = BLACK;
  cursorInvalid = DARKSALMON;

  reset();
}

OnX::~OnX()
{}

void OnX::reset()
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
  currentPhase = ONX_NONE;

  moveCounter = 0;

    
  //Reset the board
  for(int i=0; i<BOARDSIZE; i++)
  {
    board[i] = 0;
  }
  
  drawBoard();
 
  Serial.println(F("OnX reset and ready"));
}

void OnX::gameloop()
{
    switch(currentPhase)
    {
      case ONX_PREGAME:
         updatePregame();
         break; 
      case ONX_WINNER:
         updateWinner();
         break; 
      case ONX_DRAW:
         updateDraw();
         break;
      case ONX_EFFECTS:
         updateEffects();
         break;
      case ONX_ATTACK:  
         updateMainGame();
         break;
      case ONX_CHANGE_PLAYER:
         startMove();
         break;
      case ONX_NONE:
        Serial.println(F("No status flag is set, starting pregame"));
        showPregame();
        break;
    }
}

//Setup the pregame show
void OnX::showPregame()
{
  currentPhase = ONX_PREGAME;
  //Scroll the game name
 
  mCabinet->p1TopLight.setPixelColor(0, PURPLE);
  mCabinet->p1TopLight.show();
 
  mCabinet->p2TopLight.setPixelColor(0, PURPLE);
  mCabinet->p2TopLight.show();

  mCabinet->p1SFX.scrollOnce(mGameName);
  mCabinet->p2SFX.scrollOnce(mGameName);
}

void OnX::updatePregame()
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
        player2 = new OnXComputerPlayer();
        player2->reset();
     }
     else
     {
        player1 = new OnXHumanPlayer(&(mCabinet->p1Control));
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
        player1 = new OnXComputerPlayer();
        player1->reset();
     }
     else
     {
        player2 = new OnXHumanPlayer(&(mCabinet->p2Control));
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
    //Start with player1
    currentPlayer = player1;
    readyTime = millis() + 1000;

    mCabinet->p1SFX.fillScreen(BLACK);
    mCabinet->p1SFX.show();
    mCabinet->p2SFX.fillScreen(BLACK);
    mCabinet->p2SFX.show();
    
    currentPhase = ONX_CHANGE_PLAYER;
  }
}

void OnX::drawBoard()
{
  debugBoard();

}

void OnX::startMove()
{
  if(readyTime > millis())
  {
    //Not time yet
    return;
  }

  Serial.print(F("Board at start of move ")); Serial.println(moveCounter);
  debugBoard();

  if(moveCounter == BOARDSIZE)
  {
    //All spaces have been taken, but no winner. Its a draw
    Serial.println("Its a draw");
    startDraw();
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
  
  printMessage(attackingLCD, F("Your move"), F(""), 2, 3);
  printMessage(defendingLCD, F("Please wait"), F(""), 1, 4);

  drawCursor();

  if(currentPlayer->isComputer())
  {    
    //Serial.println("Computer player");
    uint8_t recMove = bestMove(counterColour);
   
    //Serial.print("Recommending move ");Serial.println(recMove);
    currentPlayer->recommendedMove(recMove); 
  }

  //Allow the effects to show
  currentPhase = ONX_ATTACK;  
}

void OnX::updateMainGame()
{
  if(currentPlayer->hasMoved())
  {
    Serial.print("Player has moved to square ");Serial.println(currentPlayer->getState()->controlState);
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

    drawCursor(selectedPosition, cursorColour);

    //Draw the new cursor
    drawCursor();
  }

  if(currentPlayer->hasPressedButton(PlayerControl::FireButton))
  {
    if(cursorColour == playerColour)
    {
      
      mCabinet->audioController.playFileIndex(SFX_VALID);
      //Serial.println("Move has been selected");
      debugBoard();
      
      //Add the new position to the board
      //Serial.println("Adding the new position to the board");
      //Serial.print("SelectedPostion:");Serial.print(selectedPosition);Serial.print(" Colour:");Serial.println(counterColour);
      board[selectedPosition] = counterColour;  


      //Check to see if we have a winner
      if(findLines(counterColour, 3) > 0)
      {
        Serial.println("Winner!");
        
        startWinner();
      }
      else
      {
        //Reset the nextFlippableCounter as we will use it again to perform the flipping
        currentPhase = ONX_EFFECTS;
      }

      moveCounter++;
      
    }
    else
    {
      mCabinet->audioController.playFileIndex(SFX_INVALID);
      Serial.println("Button pressed but not valid position");
      Serial.print("Cursor: "); Serial.print(cursorColour, HEX); Serial.print(" PlayerColour:");Serial.println(playerColour, HEX);
    }
  }
}

void OnX::drawCursor()
{
    //Draw new cursor
    selectedPosition = currentPlayer->getState()->controlState;

    cursorColour = cursorInvalid;
    //Check to see if this position is valid
    if(board[selectedPosition] == boardColour)
      cursorColour = playerColour;  

    drawCursor(selectedPosition, cursorColour);
}

void OnX::drawCursor(uint32_t pos, uint32_t col)
{   
    //Read the reference base led positon from the array
    uint8_t baseLED = physicalPositions[pos];
    
    //Serial.print("Drawing cursor at position "); Serial.println(pos);
    mCabinet->p1SFX.setPixelColor(baseLED, col);
    mCabinet->p1SFX.setPixelColor(baseLED + 1, col);
    mCabinet->p1SFX.setPixelColor(baseLED + 8, col);
    mCabinet->p1SFX.setPixelColor(baseLED + 9, col);
    
    mCabinet->p2SFX.setPixelColor(baseLED, col);
    mCabinet->p2SFX.setPixelColor(baseLED + 1, col);
    mCabinet->p2SFX.setPixelColor(baseLED + 8, col);
    mCabinet->p2SFX.setPixelColor(baseLED + 9, col);
    
    mCabinet->p1SFX.show();
    mCabinet->p2SFX.show();  
}

void OnX::updateEffects()
{
  if(readyTime > millis())
    return;

    currentPhase = ONX_CHANGE_PLAYER;
}

/* Priority 1 - can I win next move ?                                                                     Score 250
 * Priority 2 - can my opponent win next move?                                                            Score 230
 * Priority 3 - can I make a move that will give me 2 lines which have 2 my counters and a free space?    Score 200
 * Priority 4 - can I make a move that will give me 1 line with 2 of my counters and a free space?        Score 100
 * Priority 5 - can I make a move that is legal                                                           Score 10
 * 
 */

uint8_t OnX::bestMove(uint8_t counter)
{
  uint8_t moves[BOARDSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  for(uint8_t i = 0; i < BOARDSIZE; i++)
  {
    if(board[i] == 0)
    {
      //OK, its an option
      board[i] = counter;

      if(findLines(counter, 3) > 0)       moves[i] = 250;
      else if(findLines(counter, 2) == 2) moves[i] = 200;
      else if(findLines(counter, 2) == 1) moves[i] = 100;
      else                                moves[i] =  10;

      //Switch the counter and see if it is a winning move for the opponent
      if(moves[i] < 230)
      {
        uint8_t oppo = PLAYER_1_COUNTER;
        if(oppo == counter) oppo = PLAYER_2_COUNTER;
        board[i] = oppo;
        if(findLines(oppo, 3) > 0) moves[i] = 230;
      }
      //Return the space to blank
      board[i] = 0;
    }  
  }


  uint8_t bestMove = 0;
  uint8_t topScore = 0;

  for(int i = 0; i < BOARDSIZE; i++)
  { 
    
    //Lets print the score for debug
    if(moves[i] < 10) Serial.print("  ");
    else if(moves[i] < 100) Serial.print(" ");
    Serial.print(moves[i]); 
    if((i+1) % 3 == 0)
    {
      Serial.println("");
    }
    else
    {
      Serial.print("|");
    }
    //Sometimes move to this square in a draw, just to mix it up.  But only maybe 25% of the time     
    if(moves[i] > topScore || (moves[i] == topScore && random(100) < 25))
    {
      topScore = moves[i];
      bestMove = i;
    }
  }

  Serial.print("Best move: ");Serial.print(bestMove);Serial.print(" Score: ");Serial.println(topScore);

  return bestMove;
}

//Returns the number of lines in the board of the specfied counter and length
uint8_t OnX::findLines(uint8_t counter, uint8_t target)
{
    uint8_t matches = 0;
    
    /**
     * xxx
     * ...
     * ...
     */
    if(countLine(0, 1, 2, counter) == target) matches++;
   
    /**
     * ...
     * xxx
     * ...
     */
    if(countLine(3, 4, 5, counter) == target) matches++;
   
    /**
     * ...
     * ...
     * xxx
     */
    if(countLine(6, 7, 8, counter) == target) matches++;

    /**
     * x..
     * x..
     * x..
     */
    if(countLine(0, 3, 6, counter) == target) matches++;
  
    /**
     * .x.
     * .x.
     * .x.
     */
    if(countLine(1, 4, 7, counter) == target) matches++;
   
    /**
     * ..x
     * ..x
     * ..x
     */
    if(countLine(2, 5, 8, counter) == target) matches++;
   
    /**
     * x..
     * .x.
     * ..x
     */
    if(countLine(0, 4, 8, counter) == target) matches++;

    /**
     * ..x
     * .x.
     * x..
     */
    if(countLine(2, 4, 6, counter) == target) matches++;

    return matches;
}

//Returns the total number of matching counters on this line
uint8_t OnX::countLine(uint8_t pos1, uint8_t pos2, uint8_t pos3, uint8_t counter)
{
  //This is where the algorithm is deliberately weak.  The algorithm doesn't
  //take into account any opposition counter, meaning it rates xx- and xxo as
  //being as strong as each other.  To make the game unbeatable, add a penalty
  //for an opposition counter (immediately return 0)
  
  int c = 0;
  if(board[pos1] == counter) c++;
  if(board[pos2] == counter) c++;
  if(board[pos3] == counter) c++;
  return c;
}

void OnX::startDraw()
{
  //play sound
  
  mCabinet->audioController.playFileIndex(SFX_DRAW);
  
  printMessage(attackingLCD, F("It's a draw"), F(""), 2, 0);
  printMessage(defendingLCD, F("It's a draw"), F(""), 2, 0);


  currentPhase = ONX_DRAW;
}

void OnX::updateDraw()
{
  if(mCabinet->p1Control.buttonState() > 0 || mCabinet->p2Control.buttonState() > 0)
  {
    //Restart
    reset();  
  }   
}

void OnX::startWinner()
{
  //play sound
  mCabinet->audioController.playFileIndex(SFX_WIN);
  
  printMessage(attackingLCD, F("You have won"), F("Congratulations!"), 2, 0);
  printMessage(defendingLCD, F("Better luck"), F("next time"), 2, 3);

  flash = true;
  
  readyTime = millis() + 1000;
  currentPhase = ONX_WINNER;

}


void OnX::updateWinner()
{
  if(player1->hasPressedButton(PlayerControl::FireButton) || player2->hasPressedButton(PlayerControl::FireButton))
  {
    reset();
  }

  if(readyTime > millis())
    return;

  flash = !flash;
  uint32_t flashColour = playerColour;
  if(!flash) flashColour = boardColour;
  for(int i=0; i<BOARDSIZE; i++)
  {
    if(board[i] == counterColour)
    {
      drawCursor(i, flashColour);
    }
  }

  mCabinet->p1SFX.show();
  mCabinet->p2SFX.show();

  readyTime = millis() + 1000;
}

void OnX::debugBoard()
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
    if((i+1) % 3 == 0)
      Serial.print("\n");
  }
  
  Serial.println();
}

