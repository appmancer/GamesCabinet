#include <LiquidCrystal.h>

/*
 * Pixel Effects
 * by
 * Samuel Pickard
 * 
 * This class manages high level effects on a NeoPixel matrix
 */

#ifndef PIXELEFFECTS_H
#define PIXELEFFECTS_H

#include <Adafruit_NeoMatrix.h>

#define PIXELEFFECT_NONE        0
#define PIXELEFFECT_SCROLL      1
#define PIXELEFFECT_LARSENWIPE  2
#define PIXELEFFECT_DISSOLVE    3
#define PIXELEFFECT_DROP        4
#define PIXELEFFECT_WIPE        5
#define PIXELEFFECT_FADEOUT     6
#define PIXELEFFECT_FADEIN      7

#define PIXELEFFECT_DIR_LR      1
#define PIXELEFFECT_DIR_RL      -1

 class PixelEffects : public Adafruit_NeoMatrix
 {
    public:
      PixelEffects(uint8_t width, uint8_t height, uint8_t pin);
      ~PixelEffects();

      void scrollOnce(const char* message);
      void larsenWipe(uint32_t colour);
      void dissolve(); // dissolves to black
      void dissolve(uint32_t final[64]); // dissolves to provided array of colours
      void drop();// drop the matrix to black
      void drop(uint32_t final[64]); // drops to provided array of colours
      void wipe();// wipe the matrix to black
      void wipe(uint32_t final[64]); // wipes to provided array of colours
      void fade();// fade the matrix to black
      void fade(uint32_t final[64]); // fade in to provided array of colours
      void updateBoard(uint32_t final[64]); // just show this matrix

      //Override the 16bit NeoMatrix fillScreen
      void fillScreen(uint32_t color);
      //And the 16bit text color
      void setTextColor(uint32_t color);
   
      //Update returns true when the operation (e.g. scroll text) has completed its cycle
      bool updateSFX();
      
    private:
      unsigned long readyTime;
      uint8_t currentEffect;
      int16_t currentOffset;
      uint8_t charIndex;
      int8_t wipeDirection;
      uint32_t wipeColour;
      uint32_t dimColour;
      uint32_t dimmerColour;
      uint32_t dimmestColour;
      uint8_t savedBrightness;
      char* scrollMessage;
      bool updateLarsen();
      bool updateScroll();
      bool updateDissolve();
      bool updateDrop();
      bool updateWipe();
      bool updateFadeOut();
      bool updateFadeIn();

      //If we have an effect that finishes with a certain matrix configuration
      //we will store that matrix here
      uint32_t endPosition[64];

      //The dissolve effect needs an order to change pixel.  This order is stored in this array
      uint8_t dissolveOrder[64];
      
      uint8_t red(uint32_t color);
      uint8_t green(uint32_t color);
      uint8_t blue(uint32_t color);
      uint32_t dim(uint32_t color);
      uint32_t dim(uint32_t color, double multiplier);
 };

 #endif

