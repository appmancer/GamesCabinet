/*
 * GaeeBase
 * by 
 * Samuel Pickard
 * 
 * A base class each game will need to inherit from
 */

#include "Cabinet.h"
 
#ifndef GAMEBASE_H
#define GAMEBASE_H

    
#define PLAYER1 0
#define PLAYER2 1

 class GameBase
 {
  public:
    GameBase(Cabinet* cab, const char* name);
    ~GameBase() {}
    const char* id();
    virtual void gameloop() = 0;

  protected:
    Cabinet* mCabinet;
    const char* mGameName;

    void printMessage(LiquidCrystal_I2C* lcd, const __FlashStringHelper* line, const __FlashStringHelper* line2, uint8_t h1, uint8_t h2);   
 };

 #endif

