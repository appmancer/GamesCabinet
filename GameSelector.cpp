/*
 * Game Selector
 * by 
 * Samuel Pickard
 */
#include "GameSelector.h"
#include "PlayerControl.h"

GameSelector::GameSelector(Cabinet* cab) : GameBase (cab, "Choose your game")
{
  Serial.println(F("Starting Selector"));
  mSelectedGame = 0;
  mDisplayGame = 1;
  mIsDemo = false;
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
   startName();
}

void GameSelector::startDemo()
{
}

//This will be set to the function pointer of the main loop
//and be called when Battleships is the active game
void GameSelector::gameloop()
{
  bool p1Ready = mCabinet->p1SFX.updateSFX();
  bool p2Ready = mCabinet->p2SFX.updateSFX();
  //if(p1Ready && p2Ready)
  //{
  //  startName();
  //}
  
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
    mSelectedGame = mDisplayGame;  
  }
  else
  {
    uint32_t t = (mDemoTime - millis()) / 1000;
    //Serial.print("demo in ");Serial.print(t);Serial.println(" seconds");
  }
};

void GameSelector::startName()
{

  //Reset the demo timer
  mDemoTime = millis() + DEMO_TIMEOUT;
  
  //Serial.print("Starting game scroll ID: "); Serial.println(mDisplayGame);
  mCabinet->p1Display.clear();
  mCabinet->p1Display.setCursor(0, 0);
  mCabinet->p1Display.print(SELECTYOURGAME);
  mCabinet->p2Display.clear();
  mCabinet->p2Display.setCursor(0, 0);
  mCabinet->p2Display.print(SELECTYOURGAME);
  
  
  switch(mDisplayGame)
  {
    case BATTLESHIPS_INDEX:
      mCabinet->p1SFX.dissolve(battleshipsBoard);
      mCabinet->p2SFX.dissolve(battleshipsBoard);
      mCabinet->p1Display.setCursor(2, 1);
      mCabinet->p1Display.print(BATTLESHIPS); 
      mCabinet->p2Display.setCursor(2, 1);
      mCabinet->p2Display.print(BATTLESHIPS);
      
    break;  
    case CONNECT4_INDEX:
      mCabinet->p1SFX.drop(connect4Board);
      mCabinet->p2SFX.drop(connect4Board);
      mCabinet->p1Display.setCursor(3, 1);
      mCabinet->p1Display.print(CONNECT4); 
      mCabinet->p2Display.setCursor(3, 1);
      mCabinet->p2Display.print(CONNECT4);
    break;  
    case ONX_INDEX:
      mCabinet->p1SFX.wipe(onxBoard);
      mCabinet->p2SFX.wipe(onxBoard);
      mCabinet->p1Display.setCursor(4, 1);
      mCabinet->p1Display.print("O and X"); 
      mCabinet->p2Display.setCursor(4, 1);
      mCabinet->p2Display.print("O and X");
    break;  
    case REVERSI_INDEX:
      mCabinet->p1SFX.fade(reversiBoard);
      mCabinet->p2SFX.fade(reversiBoard);
      mCabinet->p1Display.setCursor(4, 1);
      mCabinet->p1Display.print(REVERSI); 
      mCabinet->p2Display.setCursor(4, 1);
      mCabinet->p2Display.print(REVERSI);
    break;  
    case MAZERACE_INDEX:
      mCabinet->p1SFX.dissolve(mazeBoard);
      mCabinet->p2SFX.dissolve(mazeBoard);
      mCabinet->p1Display.setCursor(4, 1);
      mCabinet->p1Display.print(MAZERACE); 
      mCabinet->p2Display.setCursor(4, 1);
      mCabinet->p2Display.print(MAZERACE);
    break;  
  }  
}

bool GameSelector::isDemo()
{
  return mIsDemo;
}


