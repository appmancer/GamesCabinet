#include "GameBase.h"

GameBase::GameBase(Cabinet* cab, const char* name) : mGameName(name)
{
  mCabinet = cab;  
}

const char* GameBase::id()
{
  return mGameName;
}


void GameBase::printMessage(LiquidCrystal_I2C* lcd, const __FlashStringHelper* line, const __FlashStringHelper* line2, uint8_t h1, uint8_t h2)
{
    lcd->clear();
    lcd->setCursor(h1, 0);
    lcd->print(line);
    lcd->setCursor(h2, 1);
    lcd->print(line2);     
}

