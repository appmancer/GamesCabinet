#ifndef CABINET_H
#define CABINET_H

#include <Arduino.h>
#include "PlayerControl.h"
#include <Adafruit_NeoMatrix.h>
//This is the non-package library from https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads/
#include <LiquidCrystal_I2C.h>
#include <gamma.h>
#include "WS2812_Definitions.h"
#include "PixelEffects.h"
#include "BY8X01-16P.h"

#define BATTLESHIPS_INDEX   1
#define CONNECT4_INDEX      2
#define ONX_INDEX           3
#define REVERSI_INDEX       4
#define MAZERACE_INDEX      5
#define MAX_INDEX           5

//Sounds
#define SFX_DRAW      1
#define SFX_DROP      2
#define SFX_HIT       3
#define SFX_INVALID   4
#define SFX_LAUNCH    5
#define SFX_SINKING   6
#define SFX_SPLASH    7
#define SFX_START     8
#define SFX_TICK      9
#define SFX_VALID    10
#define SFX_WIN      11

#define RESET_PIN     7


const char battleshipsName[]  PROGMEM  = {"Battleships"};
const char connect4Name[]     PROGMEM  = {"Connect 4"};
const char reversiName[]      PROGMEM  = {"Reversi"};
const char onxName[]          PROGMEM  = {"Noughts and Crosses"};
const char mazeRaceName[]     PROGMEM  = {"Maze Race"};
const char onxShortName[]     PROGMEM  = {"0s and Xs"};

const char* const names_table[] PROGMEM = {battleshipsName, connect4Name, onxName, reversiName, mazeRaceName, onxShortName};


typedef struct Cabinet
{
  //Player 1 encoders - Arduino Mega interrupt pins 2, 3, 18, 19, 20, 21.  20 & 21 are IIC pins
  // PlayerControl(uint8_t horizontalPinA, uint8_t horizontalPinB, uint8_t verticalPinA, uint8_t verticalPinB, uint8_t horizontalFire, uint8_t verticalFire, uint8_t firePin);

  /* Legacy version
  PlayerControl p1Control = PlayerControl(2, 34, 3, 30, 29, 31, 43);
  PlayerControl p2Control = PlayerControl(19, 22, 18, 24, 23, 25, 42);
  */
  /* Mega Sound and Power Board version */ 
  // OK, it turns out that you can't use the analogue pins on the back! Avoid 30-37 inclusive
  // Some (34, 36) don't work with quadrature encoders and neopixels as digital pins (some do annoyingly, like 30)
  
  PlayerControl p1Control = PlayerControl(2, 24, 3, 30, 28, 22, 46);
  PlayerControl p2Control = PlayerControl(19, 48, 18, 42, 44, 40, 38);
	
	//The playing areas.  We also have some controller classes which will manage special effects on the 
  //LED matrices for us, like scrolling text
  
  // Legacy version
  // PixelEffects p1SFX = PixelEffects(8, 8, 51);
  // PixelEffects p2SFX = PixelEffects(8, 8, 50);

  /*  Mega Sound and Power Board version, p1SFX moves to pin 52 */
  PixelEffects p1SFX = PixelEffects(8, 8, 52); 
  PixelEffects p2SFX = PixelEffects(8, 8, 26); 
   
  //Secondary displays
  LiquidCrystal_I2C p1Display = LiquidCrystal_I2C(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
  LiquidCrystal_I2C p2Display = LiquidCrystal_I2C(0x3E, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
  
  BY8X0116P audioController = BY8X0116P(Serial3);
};


#endif
