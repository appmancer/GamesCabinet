#include "PixelEffects.h"
#include <Adafruit_NeoMatrix.h>
#include "WS2812_Definitions.h"

#define SFX_VERYSHORTDELAY   20
#define SFX_SHORTDELAY   80
#define SFX_MEDIUMDELAY 200
#define SFX_LONGDELAY   500

/**
 * Colors are a bit of a mess in these libraries, because the Adafruit_GFX library uses 16bit colours, but the NeoPixel library is 24bit.  
 * NeoMatrix routines are 16bit, but NeoPixel routines are 24bit.
 * 
 * We're going to use 24bit, there is no point working in 16bit unless we need to (scroll text).  Colours are 24bit RGB values stored in uint32_t (the MSB is not used)
 */

PixelEffects::PixelEffects(uint8_t width, uint8_t height, uint8_t pin): Adafruit_NeoMatrix(width, height, pin/*, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS*/)
{
  currentEffect = PIXELEFFECT_NONE;

  //Prepare for dissolve effects by randomising the order
  //Create an ordered list
  for(uint8_t i=0; i<64; i++)
  {
    dissolveOrder[i] = i;  
  }

  //Now mix it up, swap the numbers to make certain that we have them all, and none are repeated
  uint8_t first;
  uint8_t second;
  uint8_t swap; 
  randomSeed(analogRead(A0));   
  for(uint8_t i=0; i<64; i++)
  {
     first = random(0, 64);
     second = random(0, 64);

     //Store the value of second
     swap = dissolveOrder[second];
     //Write the value from first into second
     dissolveOrder[second] = dissolveOrder[first];
     //Write the value form second into first
     dissolveOrder[first] = swap;
  }
  Serial.println("Started matrix");
}

PixelEffects::~PixelEffects()
{}

void PixelEffects::fillScreen(const uint32_t color)
{
  for(int i=0; i<64; i++)
  {
    setPixelColor(i, color);  
  }
}

void PixelEffects::setTextColor(const uint32_t color)
{
  uint16_t rgb = (color>>8&0xf800)|(color>>5&0x07e0)|(color>>3&0x001f);
  Adafruit_NeoMatrix::setTextColor(rgb);
}

/**
 * Routes to the update routine of the current effect
 */
bool PixelEffects::updateSFX()
{
  //Check to see if we're ready for an update
  
  if(currentEffect > PIXELEFFECT_NONE && readyTime > millis())
  {
    //Not time yet
    return false;
  }
  
  switch(currentEffect)
  {
    case PIXELEFFECT_NONE:
      //No job, return true
      return true;
      break;  
    case PIXELEFFECT_SCROLL:
      return updateScroll();
      break;
    case PIXELEFFECT_LARSENWIPE:
      return updateLarsen();
      break;
    case PIXELEFFECT_DISSOLVE:
      return updateDissolve();
      break;
    case PIXELEFFECT_DROP:
      return updateDrop();
      break;
    case PIXELEFFECT_WIPE:
      return updateWipe();
      break;
    case PIXELEFFECT_FADEIN:
      return updateFadeIn();
      break;
    case PIXELEFFECT_FADEOUT:
      return updateFadeOut();
      break;
  }
}

/***
 * Start an effect where a message will scroll once across the screen
 */
void PixelEffects::scrollOnce(const char* message)
{
  //Check for zero length string
  if(*message == 0) return;
 
  scrollMessage = message;
  currentEffect = PIXELEFFECT_SCROLL;
  currentOffset = width();
  
  //Wait a minimum of 50 milliseconds
  readyTime = millis() + SFX_SHORTDELAY;
}


/***
 * Starts a wipe effect, the colour will wipe the screen to black
 */
void PixelEffects::larsenWipe(uint32_t colour)
{ 
  //Only work out the dimmed pixel colour once
  wipeColour    = colour;
  dimColour     = dim(wipeColour);
  dimmerColour  = (dim(dimColour));
  dimmestColour = (dim(dimmerColour));

  currentOffset = 0;
  wipeDirection = PIXELEFFECT_DIR_LR;
  currentEffect = PIXELEFFECT_LARSENWIPE;  
  
  //Wait a minimum of 50 milliseconds
  readyTime = millis() + SFX_LONGDELAY;
}

/**
 * Starts a dissolve effect to black
 */
void PixelEffects::dissolve()
{
  //Set the end position to be all black
  for(int i=0; i<64; i++)
  {
    endPosition[i] = BLACK;  
  }  

  currentEffect = PIXELEFFECT_DISSOLVE;
  charIndex = 0;
  
  //Wait a minimum of 50 milliseconds
  readyTime = millis() + SFX_SHORTDELAY;
}


/**
 * Starts a dissolve effect to a new matrix
 */
void PixelEffects::dissolve(uint32_t final[64])
{
  //Copy the array
  for(int i=0; i<64;i++)
  {
    endPosition[i] = final[i];  
  }
  
  currentEffect = PIXELEFFECT_DISSOLVE;
  charIndex = 0;
  
  //Wait a minimum of 50 milliseconds
  readyTime = millis() + SFX_SHORTDELAY;
}

/** 
 *  Starts a drop effect to black
 */
void PixelEffects::drop()
{
  //Set the end position to be all black
  for(int i=0; i<64; i++)
  {
    endPosition[i] = BLACK;  
  }  
  
  currentEffect = PIXELEFFECT_DROP;
  charIndex = 1;
  
  //Wait a minimum of 50 milliseconds
  readyTime = millis() + SFX_SHORTDELAY;
}

/** 
 *  Starts a drop effect to a new matrix
 */
void PixelEffects::drop(uint32_t final[64])
{
  //Copy the array
  for(int i=0; i<64;i++)
  {
    endPosition[i] = final[i];  
  }
  
  currentEffect = PIXELEFFECT_DROP;
  charIndex = 1;
  
  //Wait a minimum of 50 milliseconds
  readyTime = millis() + SFX_SHORTDELAY;
}

/** 
 *  Starts a wipe effect to black
 */
void PixelEffects::wipe()
{
  //Set the end position to be all black
  for(int i=0; i<64; i++)
  {
    endPosition[i] = BLACK;  
  }  
  
  currentEffect = PIXELEFFECT_WIPE;
  charIndex = 1;
  
  //Wait a minimum of 50 milliseconds
  readyTime = millis() + SFX_SHORTDELAY;
}

/** 
 *  Starts a wipe effect to a new matrix
 */
void PixelEffects::wipe(uint32_t final[64])
{
  //Copy the array
  for(int i=0; i<64;i++)
  {
    endPosition[i] = final[i];  
  }
  
  currentEffect = PIXELEFFECT_WIPE;
  charIndex = 1;
  
  //Wait a minimum of 50 milliseconds
  readyTime = millis() + SFX_SHORTDELAY;
}

/** 
 *  Starts a fade effect to black
 */
void PixelEffects::fade()
{
  savedBrightness = getBrightness();
  //Set the end position to be all black
  for(int i=0; i<64; i++)
  {
    endPosition[i] = BLACK;  
  }  
  
  currentEffect = PIXELEFFECT_FADEOUT;
  charIndex = 1;
  
  //Wait a minimum of 50 milliseconds
  readyTime = millis() + SFX_SHORTDELAY;
}

/** 
 *  Starts a fade in effect to a new matrix
 */
void PixelEffects::fade(uint32_t final[64])
{
  savedBrightness = PIXELEFFECT_DEFAULT_BRIGHTNESS;
  //Copy the array
  for(int i=0; i<64;i++)
  {
    endPosition[i] = final[i];  
  }
  
  currentEffect = PIXELEFFECT_FADEIN;
  charIndex = 1;
  
  //Wait a minimum of 50 milliseconds
  readyTime = millis() + SFX_SHORTDELAY;
}

void PixelEffects::updateBoard(uint32_t final[64])
{
  for(int i=0; i<64; i++)
  {
    setPixelColor(i, final[i]);
  }
  
  show();
  currentEffect = PIXELEFFECT_NONE;
}

bool PixelEffects::updateScroll()
{
   //Draw the current character

  fillScreen(BLACK);
  setCursor(currentOffset, 0);
  print(scrollMessage);
  show();

  int16_t cutoff = (int16_t)strlen(scrollMessage) * -6;
  if(currentOffset < cutoff)
  {
    //Finished
    currentEffect = PIXELEFFECT_NONE;
    return true;  
  }

  currentOffset--;
  //Wait a minimum of 50 milliseconds
  readyTime = millis() + SFX_SHORTDELAY;
  return false;
}

bool PixelEffects::updateLarsen()
{
    int8_t thisCol;
    for(int i = 0; i < 8; i++)
    {
      thisCol = currentOffset;

      //If we're off the right, then keep the right edge red
      if(thisCol > 7)
        setPixelColor(i * 8 + 7, wipeColour);       
      else if(thisCol >=0)
        setPixelColor(i * 8 + thisCol, wipeColour);
        
      thisCol -= wipeDirection;
      if(thisCol >=0 && thisCol <=7)
        setPixelColor(i * 8 + thisCol, dimColour);
        
      thisCol -= wipeDirection;
      if(thisCol >=0 && thisCol <=7)
        setPixelColor(i * 8 + thisCol, dimmerColour);
        
      thisCol -= wipeDirection;
      if(thisCol >=0 && thisCol <=7)
        setPixelColor(i * 8 + thisCol, dimmestColour);
      
      thisCol -= wipeDirection;
      if(thisCol >=0 && thisCol <=7)
        setPixelColor(i * 8 + thisCol, BLACK);
    }  
    
    //update the screen
    show();

    //Update the position
    currentOffset += wipeDirection;
    
    //If we hit the right hand side, reverse the direction
    if(currentOffset == 12)
    {
      currentOffset = 7; //We've already drawn col 8 once
      wipeDirection *= -1;
    }
    
    if(currentOffset == -5) 
    {
      //We've finished and left the matrix black
      currentEffect = PIXELEFFECT_NONE;
      return true;  
    }
    else
    {
      //Wait a minimum of 50 milliseconds
      readyTime = millis() + SFX_SHORTDELAY;
      return false;  
    }
}

bool PixelEffects::updateDissolve()
{
    setPixelColor(dissolveOrder[charIndex], endPosition[dissolveOrder[charIndex]]);
    show();
    
    if(charIndex++ == 64)
    {
      //We've finished and left the matrix in the end position
      currentEffect = PIXELEFFECT_NONE;
      return true;  
    }
    else
    {
      //Wait a minimum of 50 milliseconds
      readyTime = millis() + SFX_VERYSHORTDELAY;
      return false;  
    }
}

/**
 * Copy each row to the row beneath, adding the new row at the top
 */
bool PixelEffects::updateDrop()
{
    /*  First run
   *  write colours in locations 56 - 63 into pixels 0-8
   *  second run
   *  write colours in locations 48 - 63 into pixels 0-16
   *  third run
   *  write colours in locations 40 - 63 into pixels 0 24
   *   
   */
  int pixelCount = charIndex * 8;
  int startLocation = 64 - pixelCount;
  
  /*Serial.print("Iteration: ");
  Serial.println(charIndex);
  Serial.print("write colours in locations ");
  Serial.print(startLocation);
  Serial.print(" to 63 into pixels 0-");
  Serial.println(pixelCount);*/
  
  for(int i=0; i<pixelCount; i++)
  {
    setPixelColor(i, endPosition[startLocation + i]);
  }
  
  show();
  
  //Move down 1 row for next executions
  charIndex++;

  if(charIndex == 9)
  {
    //We've finished and left the matrix in the end position
    currentEffect = PIXELEFFECT_NONE;
    return true;  
  }
  else
  {
    //Wait a minimum of 100 milliseconds
    readyTime = millis() + SFX_SHORTDELAY;
    return false;  
  }
}

/**
 * Copy each column to the column on its right, adding the new column at the left
 */
bool PixelEffects::updateWipe()
{
  //Serial.println("Updating wipe");
  uint8_t nextLED = 0;
  uint8_t logicalPos = 7;
  
  //For each visible column...
  for(uint8_t i=charIndex; i>0; i--)
  {  
    //Serial.print("Column ");Serial.println(i);
    //for each row in that column...
    for(uint8_t j=0; j<8; j++)
    {
      logicalPos = (8 - i) + (j * 8);
      nextLED = (8 * j) + i - 1;       
      //Serial,print("Setting LED "); Serial.print(nextLED); Serial.print(" to index "); Serial.println(logicalPos);
      setPixelColor(nextLED, endPosition[logicalPos]);
    }
  }
  
  show();
  
  //Move down 1 row for next executions
  charIndex++;

  if(charIndex == 9)
  {
    //We've finished and left the matrix in the end position
    currentEffect = PIXELEFFECT_NONE;
    return true;  
  }
  else
  {
    //Wait a minimum of 100 milliseconds
    readyTime = millis() + SFX_SHORTDELAY;
    return false;  
  }
}

bool PixelEffects::updateFadeOut()
{
  setBrightness(255 - (charIndex * 20));
  show();
  charIndex++;

  if(charIndex == 9)
  {
    //We've finished and left the matrix in the end position
    currentEffect = PIXELEFFECT_NONE;
    //Actually blank the screen now    
    setBrightness(savedBrightness);
    for(int i=0; i< 64; i++)setPixelColor(i,0);
    show();
    return true;  
  }
  else
  {
    //Wait a minimum of 100 milliseconds
    readyTime = millis() + SFX_SHORTDELAY;
    return false;  
  }
}

bool PixelEffects::updateFadeIn()
{
  uint8_t unit = savedBrightness / 20;
  setBrightness(charIndex * unit);
  //Redraw the colours
  for(int i=0; i<64; i++)
  {
    setPixelColor(i, endPosition[i]);
  }
  show();
  
  charIndex++;

  if(charIndex == 20)
  {
    //We've finished and left the matrix in the end position
    setBrightness(savedBrightness);
    currentEffect = PIXELEFFECT_NONE;
    return true;  
  }
  else
  {
    //Wait a minimum of 100 milliseconds
    readyTime = millis() + SFX_SHORTDELAY;
    return false;  
  }
}

// Returns the Red component of a 32-bit color
uint8_t PixelEffects::red(uint32_t color)
{
    return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t PixelEffects::green(uint32_t color)
{
    return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t PixelEffects::blue(uint32_t color)
{
    return color & 0xFF;
}

uint32_t PixelEffects::dim(uint32_t color)
{
  return dim(color, 0.6);  
}

uint32_t PixelEffects::dim(uint32_t color, double multiplier)
{
    uint32_t dimRed    = (uint32_t)(red(color)   * multiplier);
    uint32_t dimBlue   = (uint32_t)(blue(color)  * multiplier);
    uint32_t dimGreen  = (uint32_t)(green(color) * multiplier);
    uint32_t dimColor = dimRed << 16 | dimGreen << 8 | dimBlue;
    return dimColor;
}
