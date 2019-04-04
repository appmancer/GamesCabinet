#include "MazeRace.h"
#include "MazeRaceHumanPlayer.h"
//#include "MazeRaceComputerPlayerLeftWall.h"
#include "MazeRaceComputerPlayerRandomMouse.h"

MazeRace::MazeRace(Cabinet* cab) : GameBase(cab)
{
  Serial.println(F("Starting Maze Race"));
  player1 = NULL;
  player2 = NULL;
  mMaze = NULL;
  reset();	
}

MazeRace::~MazeRace()
{
  if(player1 != NULL) delete player1;
  if(player2 != NULL) delete player2;
  
}

void MazeRace::reset()
{
  uint16_t textCol = mCabinet->p1SFX.Color(255, 255, 0);
  mCabinet->p1SFX.setTextColor(textCol);
  mCabinet->p2SFX.setTextColor(textCol);

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

  Serial.println(F("Generating maze"));
  MazeGenerator mazes;
  if(mMaze == NULL)
  {
    mMaze = (char*)malloc((MAZE_DEFAULT_WIDTH * MAZE_DEFAULT_HEIGHT) + 1);
  }
 
  mazes.generate(mMaze, MAZE_DEFAULT_WIDTH, MAZE_DEFAULT_HEIGHT);
  //Find the first empty space - this is the entrace
  int e=0;
  while(mMaze[e]=='#'){e++;}
  mEntrance = e + MAZE_DEFAULT_WIDTH;
  //Close the door
  mMaze[e] = '#';
  e = MAZE_DEFAULT_WIDTH * MAZE_DEFAULT_HEIGHT - 1;
  while(mMaze[e]=='#'){e--;}
  mExit = e;

  currentPhase = MAZE_NONE;
  readyTime = millis();
  mPlayer1Finished = false;
  mPlayer2Finished = false;
  Serial.println(F("MazeRace reset and ready"));
}

void MazeRace::gameloop()
{
  switch(currentPhase)
  {
	case MAZE_NONE:
	  showPregame();
	  break;
	case MAZE_PREGAME:
	  updatePregame();
	  break;
  case MAZE_COUNTDOWN:
    updateCountdown();
    break;
	case MAZE_INGAME:
	  updateIngame();
	  break;
	case MAZE_ENDED:
	  updateEnded();
	  break;
  }
}

void MazeRace::startDemo()
{
  mDemoMode = true;  
  reset();  
  player1 = new MazeRaceComputerPlayerRandomMouse(MAZE_DEFAULT_WIDTH, MAZE_DEFAULT_HEIGHT);
  player2 = new MazeRaceComputerPlayerRandomMouse(MAZE_DEFAULT_WIDTH, MAZE_DEFAULT_HEIGHT);  

  //Set the initial positions
  player1->setPosition(mEntrance);
  player2->setPosition(mEntrance);
  mCountdown = 3;
  printMessage(&(mCabinet->p1Display), F("Get Ready!"), F(""), 3, 2);
  printMessage(&(mCabinet->p2Display), F("Get Ready!"), F(""), 3, 2);
  currentPhase = MAZE_COUNTDOWN;
}

void MazeRace::showPregame()
{
  currentPhase = MAZE_PREGAME;
  //Scroll the game name

  mCabinet->p1SFX.scrollOnce(mGameName);
  mCabinet->p2SFX.scrollOnce(mGameName);
}

void MazeRace::updatePregame()
{
  if(mCabinet->p1SFX.updateSFX() || mCabinet->p2SFX.updateSFX() ) 
  {
    //Name has stopped scrolling, do something else!
    mCabinet->p1SFX.scrollOnce(mGameName);
    mCabinet->p2SFX.scrollOnce(mGameName);
  }

  uint8_t playerButtons = mCabinet->p1Control.buttonState();

  //Serial.print("button state: ");Serial.println(playerButtons);
  if(playerButtons > 0)
  {
     //Serial.println("Testing player 1");
     //Serial.print("Player1: "); Serial.println((int)player1, HEX);
     //Player 1 has pressed a button and wants to play
     if(player1 != NULL && player2 == NULL && player1->getState()->buttonState == 0)
     {
        //Player 1 is already in the game, so create a computer player2
        //Number of players = 0
        Serial.println("Player 1 is having a single player game.  Activate the robot!");
        player2 = new MazeRaceComputerPlayerRandomMouse(MAZE_DEFAULT_WIDTH, MAZE_DEFAULT_HEIGHT);
     }
     else if(player1 == NULL)
     {
        //Serial.println("Creating human player");
        player1 = new MazeRaceHumanPlayer(&(mCabinet->p1Control), MAZE_DEFAULT_WIDTH, MAZE_DEFAULT_HEIGHT);
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
  if(playerButtons > 0)
  {
     //Serial.println("Testing player 2");
     //Player 2 has pressed a button and wants to play
     if(player2 != NULL && player1 == NULL && player2->getState()->buttonState == 0)
     {
        //Player 2 is already in the game, so create a computer player1
        //Serial.println("Player 2 is having a single player game.  Activate the robot!");
        player1 = new MazeRaceComputerPlayerRandomMouse(MAZE_DEFAULT_WIDTH, MAZE_DEFAULT_HEIGHT);
     }
     else if(player2 == NULL)
     {
        //Serial.println("Creating human player for player 2");
        player2 = new MazeRaceHumanPlayer(&(mCabinet->p2Control), MAZE_DEFAULT_WIDTH, MAZE_DEFAULT_HEIGHT);
        printMessage(&(mCabinet->p2Display), F("Waiting for"), F("other player"), 2, 2);
     }
  }
  //Keep a record of button state
  if(player2 != NULL)
  {
    //Serial.println("Setting p1 buttons");
    player2->getState()->buttonState = playerButtons;  
  }


  //When we've got two players, and all the buttons are released we can start
  if(player1 != NULL && player2 != NULL && mCabinet->p1Control.buttonState() == 0 && mCabinet->p2Control.buttonState() == 0)
  {
    //Set the initial positions
    player1->setPosition(mEntrance);
    player2->setPosition(mEntrance);
    mCountdown = 3;
    printMessage(&(mCabinet->p1Display), F("Get Ready!"), F(""), 3, 2);
    printMessage(&(mCabinet->p2Display), F("Get Ready!"), F(""), 3, 2);
  	currentPhase = MAZE_COUNTDOWN;
  }
}

void MazeRace::updateCountdown()
{
  if(readyTime > millis()) return;

  if(mCountdown == 0)
  {
    printMessage(&(mCabinet->p1Display), F("Find your"), F("way out"), 3, 4);
    printMessage(&(mCabinet->p2Display), F("Find your"), F("way out"), 3, 4);
    mStartTime = millis();
    //Draw the initial map
    drawMap();
    currentPhase = MAZE_INGAME;
  }
  else
  {
    char buff[2];
    sprintf(buff, "%d\0", mCountdown);
    mCabinet->p1SFX.setCursor(2, 0);
    mCabinet->p1SFX.fillScreen(BLACK);
    mCabinet->p1SFX.print(buff);
    mCabinet->p1SFX.show();
    mCabinet->p2SFX.setCursor(2, 0);
    mCabinet->p2SFX.fillScreen(BLACK);
    mCabinet->p2SFX.print(buff);
    mCabinet->p2SFX.show();
    
    readyTime = millis() + 1000;
    mCountdown--;
  }
}

void MazeRace::updateIngame()
{
  if(mPlayer1Finished && mPlayer2Finished)
  {
      mCabinet->audioController.playFileIndex(SFX_WIN);  
      readyTime = millis() + 5000;
      currentPhase = MAZE_ENDED;
  }
  //Serial.println("Getting positions from players");
  if(!mPlayer1Finished) player1->getMove(mMaze);
  if(!mPlayer2Finished) player2->getMove(mMaze);

  drawMap();  
}

void MazeRace::updateEnded()
{
  if(mDemoMode && (readyTime < millis()))
  {
    //Reset the chip
    digitalWrite(RESET_PIN, HIGH);
  }

  //Wait for a button press
  uint8_t player1Buttons = mCabinet->p1Control.buttonState();
  uint8_t player2Buttons = mCabinet->p2Control.buttonState();

  if(player1Buttons > 0 || player2Buttons > 0)
  {
    //Serial.println("Starting a new game");
    reset();  
  }   
}

void MazeRace::drawMap()
{
  //Update the clock
  char  buff[16];
  uint32_t endTime = millis() - mStartTime;
  sprintf(buff, "%d seconds", endTime / 1000);
    
  if(!mPlayer1Finished)
  {
    uint16_t p1Pos = player1->getPosition();
    drawMap(p1Pos, &(mCabinet->p1SFX), GREEN);
    if(p1Pos == mExit)
    {
      mPlayer1Finished = true;
      mCabinet->p1Display.clear();
      mCabinet->p1Display.setCursor(4, 0);
      mCabinet->p1Display.print(F("FINISHED"));
      mCabinet->p1Display.setCursor(3, 1);
      mCabinet->p1Display.print(buff);
    }
  }
  
  if(!mPlayer2Finished)
  {
    uint16_t p2Pos = player2->getPosition();
    drawMap(p2Pos, &(mCabinet->p2SFX), GREEN);
    
    if(p2Pos == mExit)
    {
      mPlayer2Finished = true;
      mCabinet->p2Display.clear();
      mCabinet->p2Display.setCursor(4, 0);
      mCabinet->p2Display.print(F("FINISHED"));
      mCabinet->p2Display.setCursor(3, 1);
      mCabinet->p2Display.print(buff);
    }
  }
}

//Draw a map onto the matrix.  We try to keep the player position as central as possible, but we will not scroll beyond the limits of the map
void MazeRace::drawMap(uint16_t playerPos, PixelEffects* sfx, uint32_t playerColour)
{
	//We need to calculate the top left origin of the map
	uint16_t origin = 0;
	uint8_t playerColumn = playerPos % MAZE_DEFAULT_WIDTH;
	uint8_t playerRow = playerPos / MAZE_DEFAULT_HEIGHT;
	int8_t originColumn = 0;
	int8_t originRow = 0;
	//Serial.print("I think that the player "); Serial.print(playerPos);Serial.print(" is in ROW:");Serial.print(playerRow);Serial.print(" and COL:");Serial.println(playerColumn);
	
	originRow = playerRow - 4;
	originColumn = playerColumn - 4;
	if(originRow < 0) originRow = 0;
	if(originRow > (MAZE_DEFAULT_HEIGHT - MATRIX_SIZE)) originRow = MAZE_DEFAULT_HEIGHT - MATRIX_SIZE;
	if(originColumn < 0) originColumn = 0;
	if(originColumn > (MAZE_DEFAULT_WIDTH - MATRIX_SIZE)) originColumn = MAZE_DEFAULT_WIDTH - MATRIX_SIZE;
	origin = (originRow * MAZE_DEFAULT_WIDTH) + originColumn;
	//Serial.print("I think that the origin "); Serial.print(origin);Serial.print(" is in ROW:");Serial.print(originRow);Serial.print(" and COL:");Serial.println(originColumn);

  //OK, lets draw a matrix
  uint16_t cell = origin;
  for(int i=0; i < 64; i++)
  {
      uint32_t cellColor = BLACK;
      if(mMaze[cell] == '#') cellColor = TOMATO;
      else if(cell == playerPos) cellColor = playerColour;
      //Serial.print("setting matrix cell ");Serial.print(i);Serial.print(" to color 0x");Serial.print(cellColor,HEX);Serial.print(" because maze cell at position ");Serial.print(cell);Serial.print(" is a '");Serial.print(mMaze[cell]);Serial.println("'");
      
      cell++;
      if(i > 0 && (i % MATRIX_SIZE) == MATRIX_SIZE - 1) cell += (MAZE_DEFAULT_WIDTH - MATRIX_SIZE);
     
      sfx->setPixelColor(i, cellColor);
  }
  sfx->show();
}


