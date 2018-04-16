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

typedef struct Cabinet
{
  //Player 1 encoders
  PlayerControl p1Control = PlayerControl(2, 34, 3, 30, 29, 31, 43);
  PlayerControl p2Control = PlayerControl(19, 22, 18, 24, 23, 25, 42);
	//Volume control
	//Encoder volume		    = Encoder(52, 53);
	//const uint8_t volumeButton	= 5;
	
	//The playing areas.  We also have some controller classes which will manage special effects on the 
  //LED matrices for us, like scrolling text
  PixelEffects p1SFX = PixelEffects(8, 8, 51);
  PixelEffects p2SFX = PixelEffects(8, 8, 50);

  //Secondary displays
  LiquidCrystal_I2C p1Display = LiquidCrystal_I2C(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
  LiquidCrystal_I2C p2Display = LiquidCrystal_I2C(0x3E, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
  
	//The two lights at the top of the cabinet
	Adafruit_NeoPixel p1TopLight = Adafruit_NeoPixel(1, 49, NEO_RGB + NEO_KHZ800);
	Adafruit_NeoPixel p2TopLight = Adafruit_NeoPixel(1, 48, NEO_RGB + NEO_KHZ800);

  BY8X0116P audioController = BY8X0116P(Serial3);

};


#endif
