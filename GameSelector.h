/*
 * Game Selector
 * by 
 * Samuel Pickard
 */
 
#ifndef GAMESELECTOR_H
#define GAMESELECTOR_H

#include "Cabinet.h"
#include "GameBase.h"


class GameSelector : public GameBase
{
  private:
    uint8_t mSelectedGame;
    uint8_t mDisplayGame;
    int8_t p1hpos;
    int8_t p1vpos;
    int8_t p2hpos;
    int8_t p2vpos;
    
    const char* BATTLESHIPS = "Battleships";
    const char* CONNECT4 ="Connect 4";
    const char* ONX = "Noughts and Crosses";
    const char* REVERSI = "Reversi";
    const char* SELECTYOURGAME = "Select your game";
    void startName();
    
  public:
    GameSelector(Cabinet* cab);
    ~GameSelector();
    void start();
    void reset();
    uint8_t selectedGame();
    void gameloop();

};

#endif

