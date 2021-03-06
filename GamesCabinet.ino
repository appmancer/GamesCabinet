
#include <Arduino.h>

#include "Cabinet.h"
#include "GameBase.h"
#include "Battleships.h"
#include "Connect4.h"
#include "OnX.h"
#include "Reversi.h"
#include "MazeRace.h"
#include "GameSelector.h"

GameBase* currentGame;
Cabinet* cab;
Battleships* battleships;
Connect4* connect4;
Reversi*  reversi;
OnX*      onx;
MazeRace* mazeRace;
GameSelector* gameSelector;


/**
 * The games to be supported are:
 * Battleship
 * Noughts and Crosses
 * Connect 4
 * Reversi
 * Maze Race
 * Co-op Maze
 */
 
const uint16_t colors[] = {
  PixelEffects::Color(255, 0, 0), PixelEffects::Color(0, 255, 0), PixelEffects::Color(0, 0, 255), PixelEffects::Color(0, 0, 0) };

void setup() {
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  
  Serial.begin(115200);
  Serial.println(F("Starting Sammy's Game Cabinet"));

  cab = new Cabinet();
  
  cab->p1SFX.begin();
  cab->p1SFX.setTextWrap(false);
  cab->p1SFX.setBrightness(50);
  cab->p1SFX.setTextColor(colors[0]);
  cab->p1SFX.fillScreen(colors[3]);
  cab->p1SFX.show();

  cab->p2SFX.begin();
  cab->p2SFX.setTextWrap(false);
  cab->p2SFX.setBrightness(50);
  cab->p2SFX.setTextColor(colors[0]);
  cab->p2SFX.fillScreen(colors[3]);
  cab->p2SFX.show();

  cab->p1Display.begin(16, 2);
  cab->p2Display.begin(16, 2);

  randomSeed(analogRead(0));

  //Serial.println(("Playing the sound effects");
  //Init the audio player
  Serial3.begin(9600);
  cab->audioController.init();
  //cab->audioController.printModuleInfo();
  cab->audioController.playFileIndex(SFX_START);

  currentGame = NULL;
  gameSelector = new GameSelector(cab);
  gameSelector->start();
  
  
  Serial.println("Starting main game loop"); 
}

void loop() 
{  
  
  //Check games selector button to see if we need to change the game
  //Serial.println("Calling current game loop");
  if(currentGame == NULL)
  {
    //Check to see if a game has been chosen
    uint8_t game = gameSelector->selectedGame();
    bool isDemo = gameSelector->isDemo();
    
    if(game > 0)
    {
      Serial.print(F("Game "));Serial.print(game);Serial.println(F(" has been chosen"));
      switch(game)
      {
        case   BATTLESHIPS_INDEX:
          currentGame = (GameBase*)new Battleships(cab);
          break;
        case   REVERSI_INDEX:
          currentGame = (GameBase*)new Reversi(cab);
          break;
        case  ONX_INDEX:
          currentGame = (GameBase*)new OnX(cab);
          break;
        case   CONNECT4_INDEX:
          currentGame = (GameBase*)new Connect4(cab);
          break;
        case   MAZERACE_INDEX:
          currentGame = (GameBase*)new MazeRace(cab);
          break;
      }
      if(isDemo)
        currentGame->startDemo();
    }
    else
    {
      //Continue as normal
      gameSelector->gameloop();  
    }
  }
  else
  {
    //Serial.print("Passing control to game ");Serial.println(currentGame->id());
    currentGame->gameloop();
  }
}

