/*
 * Battleships
 * by 
 * Samuel Pickard
 */
#include "Battleships.h"
#include "BattleshipsComputerPlayer.h"
#define LONGDELAY 300
#define SHORTDELAY 50

Battleships::Battleships(Cabinet* cab) : GameBase (cab)
{
  Serial.println(F("Starting Battleships"));
  player1 = NULL;
  player2 = NULL;
  reset();
}

Battleships::~Battleships()
{}

//Battleships functions
void Battleships::reset()
{

  p1SquaresRemaining = 17;
  p2SquaresRemaining = 17;
  uint16_t textCol = mCabinet->p1SFX.Color(255, 0, 0);
  mCabinet->p1SFX.setTextColor(textCol);
  mCabinet->p2SFX.setTextColor(textCol);

  //Reset the setup states
  /*
  Serial.println("Setting selected ship");
  Serial.print("Setting length : ");
  Serial.println(SHIPSIZES[0] << 4);
  Serial.print("Setting type : ");
  Serial.println(CARRIER); 
  Serial.print("Final byte : ");
  Serial.println((SHIPSIZES[0] << 4) + CARRIER, HEX); */

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
  currentPhase = BATTLESHIPS_NONE;
  Serial.println("Battleships reset and ready");
};

//This will be set to the function pointer of the main loop
//and be called when Battleships is the active game
void Battleships::gameloop()
{
    switch(currentPhase)
    {
      case BATTLESHIPS_PREGAME:
         updatePregame();
         break;
      case BATTLESHIPS_SETUP:
         updateSetup();
         break;
      case BATTLESHIPS_ATTACK:
         updateMainGame();
         break;
      case BATTLESHIPS_WINNER:
         updateWinner();
         break;        
      case BATTLESHIPS_CHANGE_PLAYER:
        startMove();
        break;
      case BATTLESHIPS_EFFECTS:
        updateEffects();
        break;
      case BATTLESHIPS_DESTROYED_SHIP:
        updateDestroyedShip();
        break;
      case BATTLESHIPS_NONE:
        //Serial.println("No status flag is set, starting pregame");
        showPregame();
        break;
    }
};

void Battleships::startDemo()
{
  mDemoMode = true;
  player1 = new BattleshipsComputerPlayer();
  player2 = new BattleshipsComputerPlayer();

  //Set the screens to all sea green
  mCabinet->p1SFX.fillScreen(NAVY);
  mCabinet->p1SFX.show();
  mCabinet->p2SFX.fillScreen(NAVY);
  mCabinet->p2SFX.show();

  //Draw the initial ship positions
  drawCursor(player1->getState(), &(mCabinet->p1SFX));
  drawCursor(player2->getState(), &(mCabinet->p2SFX));

  printMessage(&(mCabinet->p1Display), F("Choose where to"), F("place your ships"), 0, 0);
  printMessage(&(mCabinet->p2Display), F("Choose where to"), F("place your ships"), 0, 0);
  
  currentPhase = BATTLESHIPS_SETUP;
}

/***#
 * Allow a special effects routine to complete
 */
void Battleships::updateEffects()
{
  bool p1Ready = mCabinet->p1SFX.updateSFX();
  bool p2Ready = mCabinet->p2SFX.updateSFX();
  //Serial.print("p1Ready"); if(p1Ready)Serial.print(": true "); else Serial.print(": false ");Serial.print(" p2Ready");if(p2Ready)Serial.println(": true "); else Serial.println(": false ");
  if(p1Ready && p2Ready)
  {
    //Effects have finished
    currentPhase = nextPhase;  

    //Draw in a cursor at the start of the attack round
    if(currentPhase == BATTLESHIPS_ATTACK)
    {
      uint8_t posState = attackingPlayer->getState()->positionState;
      uint8_t newPos = posState & 0x3F;
      uint8_t cursorValid = attackingPlayer->getTrackingBoard()[newPos] == WATER ? VALIDPOS : INVALIDPOS;
      attackingMatrix->setPixelColor(newPos, COLOURS[cursorValid]);
      defendingMatrix->setPixelColor(newPos, COLOURS[cursorValid]);
      attackingMatrix->show();
      defendingMatrix->show();  
    }
  }
}

//Setup the pregame show
void Battleships::showPregame()
{
  currentPhase = BATTLESHIPS_PREGAME;
  //Scroll the game name
  static const char* gameName = "Battleships";
  mCabinet->p1SFX.scrollOnce(gameName);
  mCabinet->p2SFX.scrollOnce(gameName);
}

void Battleships::updatePregame()
{
  //Update the P1 matrix
  //Serial.println("updating pregame");
  if(mCabinet->p1SFX.updateSFX() || mCabinet->p2SFX.updateSFX() ) 
  {
    //Name has stopped scrolling, do something else!  
    mCabinet->p1SFX.larsenWipe(DARKGREEN);
    mCabinet->p2SFX.larsenWipe(DARKGREEN);
  }

  //Serial.println("testing button");
  //Test to see if any of the player buttons have been pressed.
  uint8_t playerButtons = mCabinet->p1Control.buttonState();

  //Serial.print("p1 button state: ");Serial.println(playerButtons);
  if(playerButtons > 0)
  {
    //Serial.println("Testing player 1");
    //Serial.print("Player1: "); Serial.println((int)player1, HEX);
     //Player 1 has pressed a button and wants to play
     if(player1 != NULL && player2 == NULL && player1->getState()->buttonState == 0)
     {
        //Player 1 is already in the game, so create a computer player2

        //Number of players = 0
        //Serial.println("Player 1 is having a single player game.  Activate the robot!");
        player2 = new BattleshipsComputerPlayer();
     }
     else if(player1 == NULL)
     {
        //Serial.println("Creating human player");
        player1 = new BattleshipsHumanPlayer(&(mCabinet->p1Control));
        printMessage(&(mCabinet->p1Display), F("Waiting for"), F("other player"), 2, 2);
     }
  }

  //Keep a record of button state
  if(player1 != NULL)
  {
    //Serial.println("Setting p1 buttons");
    player1->getState()->buttonState = playerButtons;  
  }

  //Serial.println("Getting p2 buttons");
  playerButtons = mCabinet->p2Control.buttonState();
  //Serial.print("p2 button state: ");Serial.println(playerButtons);
  if(playerButtons > 0)
  {
      //Serial.println("Testing player 2");
      //Serial.print("Player2: "); Serial.println((int)player2, HEX);
     //Player 2 has pressed a button and wants to play
     if(player2 != NULL && player1 == NULL && player2->getState()->buttonState == 0)
     {
        //Player 2 is already in the game, so create a computer player1
        //Serial.println("Player 2 is having a single player game.  Activate the robot!");
        player1 = new BattleshipsComputerPlayer();
        currentPlayer = PLAYER2;
     }
     else if(player2 == NULL)
     {
        //Serial.println("Creating human player for player 2");
        player2 = new BattleshipsHumanPlayer(&(mCabinet->p2Control));
        printMessage(&(mCabinet->p2Display), F("Waiting for"), F("other player"), 2, 2);
 
     }
  }
  //Keep a record of button state
  if(player2 != NULL)
  {
    //Serial.println("Setting p2 buttons");
    player2->getState()->buttonState = playerButtons;  
  }

  //When we've got two players, and all the buttons are released we can start
  if(player1 != NULL && player2 != NULL && mCabinet->p1Control.buttonState() == 0 && mCabinet->p2Control.buttonState() == 0)
  {
    //Serial.println("Player has pressed a button, starting setup");
    //A player has pressed something  START SETUP

    //Set the screens to all sea green
    mCabinet->p1SFX.fillScreen(NAVY);
    mCabinet->p1SFX.show();
    mCabinet->p2SFX.fillScreen(NAVY);
    mCabinet->p2SFX.show();

    //Draw the initial ship positions
    drawCursor(player1->getState(), &(mCabinet->p1SFX));
    drawCursor(player2->getState(), &(mCabinet->p2SFX));

    printMessage(&(mCabinet->p1Display), F("Choose where to"), F("place your ships"), 0, 0);
    printMessage(&(mCabinet->p2Display), F("Choose where to"), F("place your ships"), 0, 0);
    
    currentPhase = BATTLESHIPS_SETUP;
/*
    Serial.println("Default config:");
    Serial.print("Selected Ship : ");
    Serial.println(player1State.selectedShip);
    Serial.print("Ship length : ");
    Serial.println(player1State.selectedShip >> 4);
    Serial.print("Ship tyoe : ");
    Serial.println(player1State.selectedShip & 0x0F);*/
    
    //Serial.println("Setup reset complete, waiting for player to move");
  }
}

void Battleships::updateSetup()
{ 
  //Check to see if both players are ready
  if((player1->getState()->selectedShip & 0x0F) == DEATHSTAR
  && (player2->getState()->selectedShip & 0x0F) == DEATHSTAR)
  {
    //We're already to go!
      currentPhase = BATTLESHIPS_CHANGE_PLAYER;
      //Serial.println("Players are ready to begin!  ATTTACCKKK");
      //debugBoard(player1->getBoard());
      readyTime = 0;
      return;
  }

  //Check for updates to player
  updateSetupForPlayer(player1, &(mCabinet->p1SFX));
  updateSetupForPlayer(player2, &(mCabinet->p2SFX));
}

/***
 * Check the state of the controls, and update the screen if there has been any change
 */
void Battleships::updateSetupForPlayer(BattleshipsPlayer* player, PixelEffects* matrix)
{
  //Remember the old position in case the player has moved
  uint8_t oldPosState = player->getState()->positionState;
  uint8_t oldShip = player->getState()->selectedShip;
 
  //If fire button is pressed, but wasn't pressed previously
  if(player->hasPressedButton(PlayerControl::FireButton))
  {
    //Serial.println("Fire button has been pressed, add a ship to the board");
    //Add the ship to playing board
    if((player->getState()->positionState & 0x40) == 0x40)
    {
      //valid position
       addToBoard(player->getBoard(), player->getState(), matrix);

      //player->getState()->buttonState |= PlayerControl::FireButton;
    
      //Move onto the next ship
      player->incrementState();
      //player->debugPositions();
    }
  }

  //Read the encoders an update the cursor position, provided we're not on the Deathstar
  if(oldShip != 0x1E && player->hasMoved())
  {
    //Serial.println("Redrawing ship position");
    uint8_t oldPos = oldPosState & 0x3F;
    
    //Remove the old peice from the board, by resetting the matrix with the colour values from the playing board
    uint8_t inc = (oldPosState & 0x80) == 0x80 ? 8 : 1; //if the old ship was aligned vertically, we are going to count in 8s
    uint8_t shipLength = (oldShip & 0xF0) >> 4;       //take the length of the previously selected ship

    for(int i=0; i<shipLength; i++)
    {
        //We need to know the value of the piece we're representing 
        uint8_t piece = *(player->getBoard() + oldPos);
        
        /*Serial.print("Drawing ship index ");
        Serial.print(oldShip & 0x0F);
        Serial.print(" with a length of ");
        Serial.print(shipLength);
        Serial.print(" in colour ");
        Serial.println(COLOURS[piece], HEX);*/
                
        //Copy the colour value to the matrix at the cursor position
        matrix->setPixelColor(oldPos, COLOURS[piece]);
        oldPos += inc;
    }

     //Draw the new cursor
     drawCursor(player->getState(), matrix);
  }
}

/***
 * Draws the current ship to be placed on the board, but without adding it to the board data
 */
void Battleships::drawCursor(PlayerState* state, PixelEffects* matrix)
{
    //Now draw the new proposed ship
    //Serial.println("Drawing ship");
    
    uint8_t colourIndex = (state->positionState & 0x40) == 0x40 ? VALIDPOS : INVALIDPOS;
    //Serial.print("Using colour index : ");
    //Serial.println(colourIndex);
    uint8_t inc = (state->positionState & 0x80) == 0x80 ? 8 : 1;    //if the new ship was aligned vertically, we are going to count in 8s
    uint8_t oldPos = state->positionState & 0x3F;
    //Serial.print("at pixel ");
    //Serial.println(oldPos);
    uint8_t shipLength = (state->selectedShip & 0xF0) >> 4;       //take the length of the current ship
    for(int i=0; i<shipLength; i++)
    {
        //Serial.print("Drawing pixel ");
        //Serial.println(oldPos);
        //Copy the colour value to the matrix at the cursor position
        matrix->setPixelColor(oldPos, COLOURS[colourIndex]);
        oldPos += inc;
    }

    matrix->show();
}

/**
 * A piece has been placed.  Add this peice to the playing board
 */
void Battleships::addToBoard(uint8_t* board, PlayerState* state, PixelEffects* matrix)
{
  //The bottom nibble is the ship index
  uint8_t shipIndex = state->selectedShip & 0x0F;

  //Now draw the committed ship
  //Serial.println("Committed ship");
  //Serial.print("Using colour index : ");
  //Serial.println(shipIndex);
  
  //Get the correct rotation
  uint8_t inc = (state->positionState & 0x80) == 0x80 ? 8 : 1;    //if the new ship was aligned vertically, we are going to count in 8s
  uint8_t oldPos = state->positionState & 0x3F;
  //Serial.print("at pixel ");
  //Serial.println(oldPos);
  uint8_t shipLength = (state->selectedShip & 0xF0) >> 4;       //take the length of the current ship
  //Serial.print("Ship Length : ");
  //Serial.println(shipLength);
  for(int i=0; i<shipLength; i++)
  {
      //Serial.print("Drawing pixel ");
      //Serial.println(oldPos);
      //Copy the colour value to the matrix at the cursor position
      matrix->setPixelColor(oldPos, COLOURS[shipIndex]);
      *(board + oldPos) = shipIndex;
      oldPos += inc;
  }
  
  matrix->show();
}

//Update screens for a new player attack round
void Battleships::startMove()
{  
  if(readyTime > millis())
  {
    //Not time yet
    return;
  }
  
  if(currentPlayer == PLAYER1)
  {
    attackingPlayer = player1;
    defendingPlayer = player2;
    attackingMatrix = &(mCabinet->p1SFX);
    defendingMatrix = &(mCabinet->p2SFX);
    attackingLCD = &(mCabinet->p1Display);
    defendingLCD = &(mCabinet->p2Display);
  }
  else
  {
    attackingPlayer = player2;
    defendingPlayer = player1;
    attackingMatrix = &(mCabinet->p2SFX);
    defendingMatrix = &(mCabinet->p1SFX);
    attackingLCD = &(mCabinet->p2Display);
    defendingLCD = &(mCabinet->p1Display);
  }

  attackingPlayer->getState()->buttonState = 0;
  defendingPlayer->getState()->buttonState = 0;
  attackingMatrix->larsenWipe(DARKGREEN);
  defendingMatrix->larsenWipe(DARKGREEN);

  while(!attackingMatrix->updateSFX())
  {
    defendingMatrix->updateSFX();
  }

  //We have to do one more update to complete the defending matrix
  defendingMatrix->updateSFX();
  
  printMessage(attackingLCD, F("Choose where"), F("to attack"), 2, 3);
  printMessage(defendingLCD, F("You are under"), F("attack"), 1, 4);

  drawBoard(attackingPlayer->getTrackingBoard(), attackingMatrix);
  drawBoard(defendingPlayer->getBoard(), defendingMatrix);

  //Delay going to the next phase until the special effects have finished
  nextPhase = BATTLESHIPS_ATTACK;
  //Allow the effects to show
  currentPhase = BATTLESHIPS_EFFECTS;
}

void Battleships::debugBoard(uint8_t* board)
{
  Serial.println("Board position");
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<8; j++)
    {
      if(*board < 0x0F)
      {
        Serial.print("0");
      }
      
      Serial.print(*board++, HEX);  
      Serial.print(" ");  
    }
    Serial.println(""); 
  }  
}

/**
 * Draws the playing board on the matrix with a special effect! (oooohhh)
 */
void Battleships::drawBoard(uint8_t board[64], PixelEffects* matrix)
{
  //Convert the playing board to the colours
  uint32_t matBoard[64];
  for(int i=0; i<64; i++)
  {
    matBoard[i] = COLOURS[board[i]]; 
 /*   Serial.print("Updating pixel ");
    Serial.print(i);
    Serial.print(" with colour index ");
    Serial.println(board[i]); */
  }

  //matrix->drop(matBoard);
  matrix->updateBoard(matBoard);
}

void Battleships::updateMainGame()
{
  //Read in any change in position to the attacking player
  bool moved = false;
  uint8_t oldPos = 0;
  uint8_t newPos = 0;

   
  moveForAttacker();
  if(attackingPlayer->hasPressedButton(PlayerControl::FireButton))
  {
    fire();
  }
}

/**
 * Process the next move from the current player
 */
void Battleships::moveForAttacker()
{
  uint8_t oldPos = attackingPlayer->getState()->positionState & 0x3F;
  uint8_t newPos = 0;
   //Check to see if the controls have been moved
  if(attackingPlayer->hasMoved())
  {
    //Update attacker matrix
    uint8_t cursorValid = (attackingPlayer->getState()->positionState & 0x40) == 0x40 ? VALIDPOS : INVALIDPOS;
    //Player has moved, we need to redraw the cursors
    newPos = attackingPlayer->getState()->positionState & 0x3F;
    //Remove the old cursor
    uint8_t colourIndex = *(attackingPlayer->getTrackingBoard() + oldPos);
    attackingMatrix->setPixelColor(oldPos, COLOURS[colourIndex]);
    //Draw the new one
    attackingMatrix->setPixelColor(newPos, COLOURS[cursorValid]);
    
    attackingMatrix->show();

    //Update defender matrix
    //Remove the old cursor
    colourIndex = *(defendingPlayer->getBoard() + oldPos);
    defendingMatrix->setPixelColor(oldPos, COLOURS[colourIndex]);
    //Draw the new one
    defendingMatrix->setPixelColor(newPos, COLOURS[cursorValid]);
    defendingMatrix->show();
  } 
}

void Battleships::fire()
{
  //Serial.println("FIRE!");

  uint8_t cursorValid = (attackingPlayer->getState()->positionState & 0x40) == 0x40 ? VALIDPOS : INVALIDPOS;
  if(cursorValid == INVALIDPOS)
  {
    //Not a valid position to hit, return and wait for a better attempt
    //Serial.println("Attempting to fire on invalid position");
    return;  
  }

  uint8_t pos = attackingPlayer->getState()->positionState & 0x3F;

  uint8_t attackedPos = *(defendingPlayer->getBoard() + pos);
  //Serial.print("Attacked position 0x");Serial.println(attackedPos, HEX);
  if((attackedPos & WATER) != WATER)
  {
    //We've not hit water!
    //Serial.println("Hit");
    //debugBoard(defendingPlayer->getBoard());
    //Read the current value of the hit square
    uint8_t hitShip = *(defendingPlayer->getBoard() + pos);

    //Tell the attacker that they've been successful
    attackingPlayer->opponentHasBeenHit();
      
    //Update the defender's board
    *(defendingPlayer->getBoard() + pos) = DAMAGED;
    defendingMatrix->setPixelColor(pos, COLOURS[DAMAGED]);
    defendingMatrix->show();
    printMessage(defendingLCD, F("You have"), F("been hit"), 4, 4);
    
    //Update the attacker's tracking board
    *(attackingPlayer->getTrackingBoard() + pos) = DAMAGED;
    attackingMatrix->setPixelColor(pos, COLOURS[DAMAGED]);  
    attackingMatrix->show();
    printMessage(attackingLCD, F("You have hit"), F("your opponent"), 2, 2);

    //play sound
    mCabinet->audioController.playFileIndex(SFX_HIT);
    
    //Check to see if there any any other square for this ship
    bool found = false;
    uint8_t c = 0;
    uint8_t* board = defendingPlayer->getBoard();
    while(!found && c < 64) found = (*(board + c++) == hitShip);

    if(!found)
    {
      //This ship has been completely destroyed  
      uint8_t attackedShipIndex = hitShip / 2 - 2;
      pos = defendingPlayer->getState()->shipPositions[attackedShipIndex];
      //Serial.print("Ship number ");Serial.print(attackedShipIndex);Serial.println(" has been destroyed");
      
      printMessage(attackingLCD, F("You have sunk"), F("their ship"), 2, 3);
      printMessage(defendingLCD, F("Your ship"), F("has been sunk"), 3, 2);
        
      attackedShipInc = 1;
      if((pos & 0x80) == 0x80)
      {
        attackedShipInc = 8;
      }
      attackedShipNextCellPos = pos & 0x3F;
      attackedShipAnimationCounter = 0;
      attackedShipLength = attackedShipNextCellPos + ((SHIPSIZES[attackedShipIndex] - 1) *  attackedShipInc);
      /*Serial.println("Ready to animate destruction");
      Serial.print("Pos:");
      Serial.print(attackedShipNextCellPos);
      Serial.print(" Inc:");
      Serial.print(attackedShipInc);
      Serial.print(" Len:");
      Serial.println(attackedShipLength);*/

      //Tell the attacker that they've destroyed a ship
      attackingPlayer->opponentShipDestroyed();

      readyTime = millis() + SHORTDELAY;
      currentPhase = BATTLESHIPS_DESTROYED_SHIP;

      //Play sound
      mCabinet->audioController.playFileIndex(SFX_SINKING);
      return;
    }
  }
  else //shot missed
  {
    //Serial.println("Missed!");
    //Update the attacker's tracking board
    *(attackingPlayer->getTrackingBoard() + pos) = MISSED;
    attackingMatrix->setPixelColor(pos, COLOURS[MISSED]);
    attackingMatrix->show();
    attackingLCD->clear();
    attackingLCD->setCursor(0,0);
    attackingLCD->print(F("You have missed"));
    attackingLCD->setCursor(1,1);
    attackingLCD->print(F("your opponent"));
    
    *(defendingPlayer->getBoard() + pos) = MISSED;
    defendingMatrix->setPixelColor(pos, COLOURS[MISSED]);
    defendingMatrix->show();
    defendingLCD->clear();
    defendingLCD->setCursor(4,0);
    defendingLCD->print(F("Missed!"));
    
    //Play sound
    mCabinet->audioController.playFileIndex(SFX_SPLASH);
  }

  //Change player
  currentPlayer = !currentPlayer;
  currentPhase = BATTLESHIPS_CHANGE_PLAYER;
  readyTime = millis() + LONGDELAY;
}

void Battleships::updateDestroyedShip()
{
  if(readyTime > millis())
  {
    //Not time yet
    return;
  }

  //Continue
  //Get the current destroyed ship animation status
  /*Serial.println("Destroying cell");
  Serial.print("Pos:");
  Serial.print(attackedShipNextCellPos);
  Serial.print(" Inc:");
  Serial.print(attackedShipInc);
  Serial.print(" Len:");
  Serial.println(attackedShipLength);*/

  attackingMatrix->setPixelColor(attackedShipNextCellPos, COLOURS[DESTROYED]);
  attackingMatrix->show();
  defendingMatrix->setPixelColor(attackedShipNextCellPos, COLOURS[DESTROYED]);
  defendingMatrix->show();
  *(defendingPlayer->getBoard() + attackedShipNextCellPos) = DESTROYED;
  *(attackingPlayer->getTrackingBoard() + attackedShipNextCellPos) = DESTROYED;

  //TODO: play sound
  //Decrease player's remaining squares
  defendingPlayer->hit();
   
  if(attackedShipNextCellPos == attackedShipLength)
  {
    //Serial.println("Finished the destruction animation");  
    BattleshipsPlayer* defendingPlayer = player1;
    if(currentPlayer == PLAYER1) defendingPlayer = player2;
    
    if(defendingPlayer->isDestroyed()) 
    {
      //Serial.println("Defender is destroyed, end the game");  
      printMessage(attackingLCD, F("You are"), F("the winner!"), 5, 2);
      printMessage(defendingLCD, F("All of your"), F("ships have sunk!"), 2, 0);
      
      attackingMatrix->larsenWipe(DARKGREEN);
      defendingMatrix->larsenWipe(DARKGREEN);  
      
      currentPhase = BATTLESHIPS_WINNER;
      readyTime = millis() + 5000;
      //Play sound
      mCabinet->audioController.playFileIndex(SFX_WIN);
      return;
    }
    else
    {
      //Finished destroying this ship, but more remain!
      //Change player
      currentPlayer = !currentPlayer;
      currentPhase = BATTLESHIPS_CHANGE_PLAYER;
      readyTime = millis() + LONGDELAY;
      return;
    }
  }

  //If we're here in the flow, then we've not finished destroying this ship
  readyTime = millis() + SHORTDELAY;
  attackedShipNextCellPos += attackedShipInc;
}

void Battleships::updateWinner()
{
  //Wait for a button press
  uint8_t player1Buttons = mCabinet->p1Control.buttonState();
  uint8_t player2Buttons = mCabinet->p2Control.buttonState();

  if(mDemoMode && readyTime > millis())
  {
    //Reset the chip
    digitalWrite(RESET_PIN, HIGH);
  }

  if(player1Buttons > 0 || player2Buttons > 0)
  {
    //Serial.println("Starting a new game");
    
    printMessage(attackingLCD, F("Press any button"), F("to start"), 0, 4);
    printMessage(defendingLCD, F("Press any button"), F("to start"), 0, 4);
    
    reset();  
  }
}


