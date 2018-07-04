/*
 * Game Selector
 * by 
 * Samuel Pickard
 */
 
#ifndef GAMESELECTOR_H
#define GAMESELECTOR_H

#include "Cabinet.h"
#include "GameBase.h"
#include <avr/pgmspace.h>

#define SELECTOR_NONE              0
#define SELECTOR_CHOOSE            1
#define SELECTOR_DEMO              2
#define SELECTOR_DEMO_PREVIEW      3
#define SELECTOR_DEMO_START        4

const PROGMEM uint32_t battleshipsBoard[64] = {0x80, 0x2E8B57, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x2E8B57, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x2E8B57, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFA8072, 0xFA8072, 0xFA8072, 0x80, 0x800080, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x800080, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x800080, 0x80, 0xB8860B, 0xB8860B, 0xB8860B, 0xB8860B, 0xB8860B, 0x80, 0x800080, 0x80, 0x80, 0x80, 0xFF4500, 0xFF4500, 0x80, 0x80, 0x80, 0x80};
const PROGMEM uint32_t connect4Board[64] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFFFF00, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF0000, 0xFFFF00, 0xFF0000, 0xFF0000, 0xFF0000, 0x0, 0x0, 0x0, 0xFFFF00, 0xFF0000, 0xFF0000, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0x0, 0x0, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFF0000, 0xFF0000, 0xFF0000, 0x0, 0xFF0000};
const PROGMEM uint32_t mazeBoard[64] = {0xFF6347, 0xFF6347, 0xFF6347, 0xFF6347, 0xFF6347, 0xFF6347, 0xFF6347, 0xFF6347, 0x0, 0x0, 0x0, 0xFF6347, 0x8000, 0xFF6347, 0x0, 0x0, 0x0, 0xFF6347, 0x0, 0xFF6347, 0x0, 0xFF6347, 0x0, 0xFF6347, 0x0, 0xFF6347, 0x0, 0xFF6347, 0x0, 0xFF6347, 0x0, 0xFF6347, 0xFF6347, 0xFF6347, 0x0, 0xFF6347, 0x0, 0xFF6347, 0xFF6347, 0xFF6347, 0x0, 0x0, 0x0, 0xFF6347, 0x0, 0xFF6347, 0x0, 0x0, 0x0, 0xFF6347, 0x0, 0xFF6347, 0x0, 0xFF6347, 0x0, 0xFF6347, 0x0, 0xFF6347, 0x0, 0xFF6347, 0x0, 0x0, 0x0, 0xFF6347};
const PROGMEM uint32_t onxBoard[64] = {0xFF, 0xFF, 0x0, 0xFF, 0xFF, 0x0, 0xFFFF00, 0xFFFF00, 0xFF, 0xFF, 0x0, 0xFF, 0xFF, 0x0, 0xFFFF00, 0xFFFF00, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFFFF00, 0xFFFF00, 0x0, 0xFFFF00, 0xFFFF00, 0x0, 0xFF, 0xFF, 0xFFFF00, 0xFFFF00, 0x0, 0xFFFF00, 0xFFFF00, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0xFFFF00, 0xFFFF00, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0xFFFF00, 0xFFFF00, 0x0, 0xFF, 0xFF};
const PROGMEM uint32_t reversiBoard[64] = {0x0, 0x0, 0x0, 0x0, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFFFF00, 0xFFFF00, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFFFF00, 0xFFFF00, 0xFF, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFFFF00, 0xFF, 0xFFFF00, 0x0, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFFFF00, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFFFF00, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

#define DEMO_TIMEOUT 10000

class GameSelector : public GameBase
{
  private:
    uint8_t mSelectedGame;
    uint8_t mDisplayGame;
    int8_t p1hpos;
    int8_t p1vpos;
    int8_t p2hpos;
    int8_t p2vpos;
    
    void startName();

    uint32_t mDemoTime;
    bool mIsDemo;
    uint8_t currentPhase;

    void updateChooseGame();
    void updateScroll();

    char gameName[12];
    
  public:
    GameSelector(Cabinet* cab);
    ~GameSelector();
    void start();
    void reset();
    uint8_t selectedGame();
    void gameloop();
    void startDemo();
    bool isDemo();

};

#endif

