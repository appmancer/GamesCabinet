/*
 * Game Selector
 * by 
 * Samuel Pickard
 */
#include "GameSelector.h"
#include "PlayerControl.h"

GameSelector::GameSelector(Cabinet* cab) : GameBase (cab)
{
  Serial.println(F("Starting Selector"));
  mSelectedGame = 0;
  mDisplayGame = 1;
  mIsDemo = false;
  currentPhase = SELECTOR_NONE; 
}

GameSelector::~GameSelector()
{}

void GameSelector::reset()
{
  mCabinet->p1Control.hEncoder.write(0);
  mCabinet->p1Control.vEncoder.write(0);
  mCabinet->p2Control.hEncoder.write(0);
  mCabinet->p2Control.vEncoder.write(0);
}

//GameSelector functions
uint8_t GameSelector::selectedGame()
{
  return mSelectedGame;
};

void GameSelector::start()
{
   mDemoTime = millis() + DEMO_TIMEOUT;
   currentPhase = SELECTOR_CHOOSE;
   startName();
}

void GameSelector::startDemo()
{
}

void GameSelector::gameloop()
{
  switch(currentPhase)
  {
    case SELECTOR_NONE:
      start();
      break;
    case SELECTOR_CHOOSE:
      updateChooseGame();
      break;
    case SELECTOR_DEMO:
      updateScroll();
      break; 
  }
}

void GameSelector::updateChooseGame()
{
  bool p1Ready = mCabinet->p1SFX.updateSFX();
  bool p2Ready = mCabinet->p2SFX.updateSFX();
  
  //Check to see if the fire button is pressed
  if(((mCabinet->p1Control.buttonState() & PlayerControl::FireButton) == PlayerControl::FireButton)
  || ((mCabinet->p2Control.buttonState() & PlayerControl::FireButton) == PlayerControl::FireButton))
  {
    //FIRE!
    Serial.println("Fire button has been pressed");
    Serial.println(mCabinet->p1Control.buttonState(), HEX);
    Serial.println(mCabinet->p2Control.buttonState(), HEX);
    mSelectedGame = mDisplayGame;  
  }
  
  //Check to see if any values are != 0
  if(mCabinet->p1Control.hEncoder.read() <= -4 ||
     mCabinet->p1Control.vEncoder.read() <= -4 ||
     mCabinet->p2Control.hEncoder.read() <= -4 ||
     mCabinet->p2Control.vEncoder.read() <= -4)
  {
    //Serial.println("Encoder left");
    //Move left  
    if(mDisplayGame == 1)
    {
      mDisplayGame = MAX_INDEX;
    }
    else
    {
      mDisplayGame--;
    } 
    startName();
    reset();
  }
  else if(mCabinet->p1Control.hEncoder.read() >= 4 ||
          mCabinet->p1Control.vEncoder.read() >= 4 ||
          mCabinet->p2Control.hEncoder.read() >= 4 ||
          mCabinet->p2Control.vEncoder.read() >= 4)
  {
    //Serial.println("Encoder right");
    //Move right
    if(mDisplayGame == MAX_INDEX)
    {
      mDisplayGame = 1;
    }
    else
    {
      mDisplayGame++;
    } 
    startName();
    reset();
  }

  //if there is no action, and we've reached the timeout
  if(millis() > mDemoTime)
  {
    //Start demo mode
    Serial.println(F("Starting demo mode"));
    mIsDemo = true;
    //Choose a game at random  
    randomSeed(analogRead(0));
    mDisplayGame = random(1, 6);
    strcpy_P(gameName, (char*)pgm_read_word(&(names_table[mDisplayGame - 1]))); // Necessary casts and dereferencing, just copy.
    mCabinet->p1SFX.scrollOnce(gameName);
    mCabinet->p2SFX.scrollOnce(gameName);  
    mCabinet->p1Display.clear();
    mCabinet->p1Display.setCursor(3, 0);
    mCabinet->p1Display.print(F("Demo mode"));
    mCabinet->p2Display.clear();
    mCabinet->p2Display.setCursor(3, 0);
    mCabinet->p2Display.print(F("Demo mode"));
    currentPhase = SELECTOR_DEMO;
  }
  else
  {
    uint32_t t = (mDemoTime - millis()) / 1000;
    //Serial.print("demo in ");Serial.print(t);Serial.println(" seconds");
  }
};

void GameSelector::updateScroll()
{  
  bool p1Ready = mCabinet->p1SFX.updateSFX();
  bool p2Ready = mCabinet->p2SFX.updateSFX();
  if(p1Ready && p2Ready)
  {
    if(currentPhase == SELECTOR_DEMO)
    {
      mSelectedGame = mDisplayGame;  
    }
  }
}

void GameSelector::startName()
{    
  //Reset the demo timer
  mDemoTime = millis() + DEMO_TIMEOUT;
  
  //Serial.print("Starting game scroll ID: "); Serial.println(mDisplayGame);
  mCabinet->p1Display.clear();
  mCabinet->p1Display.setCursor(0, 0);
  mCabinet->p1Display.print(F("Choose your game"));
  mCabinet->p2Display.clear();
  mCabinet->p2Display.setCursor(0, 0);
  mCabinet->p2Display.print(F("Choose your game"));

  uint32_t matrix[64];
    
  switch(mDisplayGame)
  {
    case BATTLESHIPS_INDEX:
      for(int i=0; i<64; i++) matrix[i] = pgm_read_dword_far(battleshipsBoard + i);
      mCabinet->p1SFX.dissolve(matrix);
      mCabinet->p2SFX.dissolve(matrix);
      mCabinet->p1Display.setCursor(2, 1);
      mCabinet->p2Display.setCursor(2, 1);
      
    break;  
    case CONNECT4_INDEX:
      for(int i=0; i<64; i++) matrix[i] = pgm_read_dword_far(connect4Board + i);
      mCabinet->p1SFX.drop(matrix);
      mCabinet->p2SFX.drop(matrix);
      mCabinet->p1Display.setCursor(3, 1); 
      mCabinet->p2Display.setCursor(3, 1);
    break;  
    case ONX_INDEX:
      for(int i=0; i<64; i++) matrix[i] = pgm_read_dword_far(onxBoard + i);
      mCabinet->p1SFX.wipe(matrix);
      mCabinet->p2SFX.wipe(matrix);
      mCabinet->p1Display.setCursor(4, 1);
      mCabinet->p2Display.setCursor(0, 1);
    break;  
    case REVERSI_INDEX:
      for(int i=0; i<64; i++) matrix[i] = pgm_read_dword_far(reversiBoard + i);
      mCabinet->p1SFX.fade(matrix);
      mCabinet->p2SFX.fade(matrix);
      mCabinet->p1Display.setCursor(4, 1);
      mCabinet->p2Display.setCursor(4, 1);
    break;  
    case MAZERACE_INDEX:
      for(int i=0; i<64; i++) matrix[i] = pgm_read_dword_far(mazeBoard + i);
      mCabinet->p1SFX.dissolve(matrix);
      mCabinet->p2SFX.dissolve(matrix);
      mCabinet->p1Display.setCursor(4, 1); 
      mCabinet->p2Display.setCursor(4, 1);
    break;  
  }
  if(mDisplayGame == ONX_INDEX)
  {
    strcpy_P(gameName, (char*)pgm_read_word(&(names_table[MAX_INDEX]))); // The short version of 0s and Xs is at the end.    
  }
  else
  {
    strcpy_P(gameName, (char*)pgm_read_word(&(names_table[mDisplayGame - 1]))); // Necessary casts and dereferencing, just copy.
  }
  mCabinet->p1Display.print(gameName); 
  mCabinet->p2Display.print(gameName); 
}

bool GameSelector::isDemo()
{
  return mIsDemo;
}


