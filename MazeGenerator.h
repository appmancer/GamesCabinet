/*
 * MazeGenerator
 * by 
 * Samuel Pickard
 */

/*
 * A maze is a char array.  Wrap the string every MAZE_DEFAULT_WIDTH
 * Origin is top left
 */
 
#include <Arduino.h>
#ifndef MAZE_GENERATOR_H
#define MAZE_GENERATOR_H

class MazeGenerator
{
  public:
    MazeGenerator();
    ~MazeGenerator();

    void generate(char* maze, uint8_t width, uint8_t height);

  private:
    uint8_t mWidth;
    uint8_t mHeight;
    void removeBlock(char* maze, uint32_t pos);
    void debugMaze(char* maze);

};
#endif

