#include "MazeRace.h"
#include "MazeRaceHumanPlayer.h"
#include "MazeRaceComputerPlayerLeftWall.h"

MazeRace::MazeRace(Cabinet* cab) : GameBase(cab, "Maze Race")
{
  Serial.println("Starting Maze Race");
  player1 = NULL;
  player2 = NULL;
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

  mCabinet->p1TopLight.setPixelColor(0, PURPLE);
  mCabinet->p1TopLight.show();
 
  mCabinet->p2TopLight.setPixelColor(0, PURPLE);
  mCabinet->p2TopLight.show();

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

  Serial.println("Generating maze");
  MazeGenerator mazes;
  mMaze.reserve(MAZE_DEFAULT_WIDTH * MAZE_DEFAULT_HEIGHT);
  mazes.generate(&mMaze, MAZE_DEFAULT_WIDTH, MAZE_DEFAULT_HEIGHT);

  currentPhase = MAZE_NONE;
  Serial.println("MazeRace reset and ready");
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
	case MAZE_INGAME:
	  updateIngame();
	  break;
	case MAZE_ENDED:
	  updateEnded();
	  break;
    }
}

void MazeRace::showPregame()
{
  currentPhase = MAZE_PREGAME;
  //Scroll the game name
 
  mCabinet->p1TopLight.setPixelColor(0, PURPLE);
  mCabinet->p1TopLight.show();
 
  mCabinet->p2TopLight.setPixelColor(0, PURPLE);
  mCabinet->p2TopLight.show();

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
    Serial.println("Testing player 1");
    //Serial.print("Player1: "); Serial.println((int)player1, HEX);
     //Player 1 has pressed a button and wants to play
     if(player1 != NULL)
     {
        //Player 1 is already in the game, so create a computer player2

        //Number of players = 0
        Serial.println("Player 1 is having a single player game.  Activate the robot!");
        player2 = new MazeRaceComputerPlayerLeftWall();
     }
     else
     {
        Serial.println("Creating human player");
        player1 = new MazeRaceHumanPlayer(&(mCabinet->p1Control));
        printMessage(&(mCabinet->p1Display), F("Waiting for"), F("other player"), 2, 2);
     }
  }

  //Serial.println("Getting p2 buttons");
  playerButtons = mCabinet->p2Control.buttonState();
  if(playerButtons > 0)
  {
      Serial.println("Testing player 2");
     //Player 2 has pressed a button and wants to play
     if(player2 != NULL)
     {
        //Player 2 is already in the game, so create a computer player1
        Serial.println("Player 2 is having a single player game.  Activate the robot!");
        player1 = new MazeRaceComputerPlayerLeftWall();
     }
     else
     {
        Serial.println("Creating human player for player 2");
        player2 = new MazeRaceHumanPlayer(&(mCabinet->p2Control));
        printMessage(&(mCabinet->p2Display), F("Waiting for"), F("other player"), 2, 2);
     }
  }

  //When we've got two players, and all the buttons are released we can start
  if(player1 != NULL && player2 != NULL && mCabinet->p1Control.buttonState() == 0 && mCabinet->p2Control.buttonState() == 0)
  {
	//Draw the initial map
	drawMap();
	currentPhase = MAZE_INGAME;
  }
}

void MazeRace::updateIngame()
{}

void MazeRace::updateEnded()
{}

void MazeRace::drawMap()
{
	drawMap(player1->getPosition(), &(mCabinet->p1SFX), GREEN);
	drawMap(player2->getPosition(), &(mCabinet->p2SFX), PURPLE);
}

//Draw a map onto the matrix.  We try to keep the player position as central as possible, but we will not scroll beyond the limits of the map
void MazeRace::drawMap(uint16_t playerPos, PixelEffects* sfx, uint16_t playerColour)
{
	//We need to calculate the top left origin of the map
	uint16_t origin = 0;
	uint8_t playerColumn = playerPos % MAZE_DEFAULT_WIDTH;
	uint8_t playerRow = playerPos / MAZE_DEFAULT_HEIGHT;
	int8_t originColumn = 0;
	int8_t originRow = 0;
	Serial.print("I think that the player "); Serial.print(playerPos);Serial.print(" is in ROW:");Serial.print(playerRow);Serial.print(" and COL:");Serial.println(playerColumn);
	
	originRow = playerRow - 4;
	originColumn = playerColumn - 4;
	if(originRow < 0) originRow = 0;
	if(originRow > (MAZE_DEFAULT_HEIGHT - MATRIX_SIZE)) originRow = MAZE_DEFAULT_HEIGHT - MATRIX_SIZE;
	if(originColumn < 0) originColumn = 0;
	if(originColumn > (MAZE_DEFAULT_WIDTH - MATRIX_SIZE)) originColumn = MAZE_DEFAULT_WIDTH - MATRIX_SIZE;
	origin = (originRow * MAZE_DEFAULT_WIDTH) + originColumn;
	Serial.print("I think that the origin "); Serial.print(origin);Serial.print(" is in ROW:");Serial.print(originRow);Serial.print(" and COL:");Serial.println(originColumn);
	
	//TODO: Remove this debug
	//Add the player position to the maze for debug
	mMaze.setCharAt(playerPos, '*');
	uint16_t startPos = origin;
	for(int i = 0; i < MATRIX_SIZE; i++)
	{
	    Serial.println(mMaze.substring(startPos, MATRIX_SIZE));
	    startPos += MAZE_DEFAULT_WIDTH;
	}
	mMaze.setCharAt(playerPos, ' ');
}


