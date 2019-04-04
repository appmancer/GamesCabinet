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
    GameBase(Cabinet* cab);
    ~GameBase() {}
    virtual void gameloop() = 0;
    virtual void startDemo() = 0;
    
  protected:
    Cabinet* mCabinet;
    bool mDemoMode;
    void printMessage(LiquidCrystal_I2C* lcd, const __FlashStringHelper* line, const __FlashStringHelper* line2, uint8_t h1, uint8_t h2);   
 };

 #endif

