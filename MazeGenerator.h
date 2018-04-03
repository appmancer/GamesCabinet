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

#define MAZE_DEFAULT_WIDTH 63
#define MAZE_DEFAULT_HEIGHT 63

class MazeGenerator
{
  public:
    MazeGenerator();
    ~MazeGenerator();

    void generate(String* maze);
    void generate(String* maze, uint8_t width, uint8_t height);

  private:
    uint8_t mWidth;
    uint8_t mHeight;
    void removeBlock(String* maze, uint32_t pos);
    void debugMaze(String* maze);

};
#endif

