/*
 * Game Selector
 * by 
 * Samuel Pickard
 */
#include "GameSelector.h"
#include "PlayerControl.h"

GameSelector::GameSelector(Cabinet* cab) : GameBase (cab, "Choose your game")
{
  Serial.println("Starting Selector");
  mSelectedGame = 0;
  mDisplayGame = 1;
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
   startName();
}

//This will be set to the function pointer of the main loop
//and be called when Battleships is the active game
void GameSelector::gameloop()
{
  bool p1Ready = mCabinet->p1SFX.updateSFX();
  bool p2Ready = mCabinet->p2SFX.updateSFX();
  if(p1Ready && p2Ready)
  {
    startName();
  }

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
  
};

void GameSelector::startName()
{
  Serial.print("Starting game scroll ID: "); Serial.println(mDisplayGame);
  mCabinet->p1Display.clear();
  mCabinet->p1Display.setCursor(0, 0);
  mCabinet->p1Display.print(SELECTYOURGAME);
  mCabinet->p2Display.clear();
  mCabinet->p2Display.setCursor(0, 0);
  mCabinet->p2Display.print(SELECTYOURGAME);
  
  
  switch(mDisplayGame)
  {
    case BATTLESHIPS_INDEX:
      mCabinet->p1SFX.setTextColor(PixelEffects::Color(255, 0, 0));
      mCabinet->p2SFX.setTextColor(PixelEffects::Color(255, 0, 0));
      mCabinet->p1SFX.scrollOnce(BATTLESHIPS);
      mCabinet->p2SFX.scrollOnce(BATTLESHIPS);      
      mCabinet->p1Display.setCursor(2, 1);
      mCabinet->p1Display.print(BATTLESHIPS); 
      mCabinet->p2Display.setCursor(2, 1);
      mCabinet->p2Display.print(BATTLESHIPS);
      
    break;  
    case CONNECT4_INDEX:
      mCabinet->p1SFX.setTextColor(PixelEffects::Color(255, 0, 0));
      mCabinet->p2SFX.setTextColor(PixelEffects::Color(255, 0, 0));
      mCabinet->p1SFX.scrollOnce(CONNECT4);
      mCabinet->p2SFX.scrollOnce(CONNECT4);
      mCabinet->p1Display.setCursor(3, 1);
      mCabinet->p1Display.print(CONNECT4); 
      mCabinet->p2Display.setCursor(3, 1);
      mCabinet->p2Display.print(CONNECT4);
    break;  
    case ONX_INDEX:
      mCabinet->p1SFX.setTextColor(PixelEffects::Color(255, 0, 0));
      mCabinet->p2SFX.setTextColor(PixelEffects::Color(255, 0, 0));
      mCabinet->p1SFX.scrollOnce(ONX);
      mCabinet->p2SFX.scrollOnce(ONX);
      mCabinet->p1Display.setCursor(4, 1);
      mCabinet->p1Display.print("O and X"); 
      mCabinet->p2Display.setCursor(4, 1);
      mCabinet->p2Display.print("O and X");
    break;  
    case REVERSI_INDEX:
      mCabinet->p1SFX.setTextColor(PixelEffects::Color(255, 0, 0));
      mCabinet->p2SFX.setTextColor(PixelEffects::Color(255, 0, 0));
      mCabinet->p1SFX.scrollOnce(REVERSI);
      mCabinet->p2SFX.scrollOnce(REVERSI);
      mCabinet->p1Display.setCursor(4, 1);
      mCabinet->p1Display.print(REVERSI); 
      mCabinet->p2Display.setCursor(4, 1);
      mCabinet->p2Display.print(REVERSI);
    break;  
  }  
}


