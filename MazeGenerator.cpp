#include "MazeGenerator.h"

MazeGenerator::MazeGenerator()
{
}

MazeGenerator::~MazeGenerator()
{
}

void MazeGenerator::generate(String* maze, uint8_t width, uint8_t height)
{
  uint16_t mazeSize = width * height; 
  
  if(maze->length() != mazeSize) return;

  mWidth = width;
  mHeight = height;
  
  //Reset the maze
  for(int i=0; i < mazeSize; i++)
  {
    maze->setCharAt(i,'#');  
  }  
  Serial.println(F("Maze reset"));

  //Pick a start point on the bottom of the maze
  randomSeed(analogRead(0));
  uint16_t start = random(width - 2) + 1;
  //Knock out the start block
  maze->setCharAt(start, ' ');

  //Generate a path in the maze
  removeBlock(maze, start + width);

  //Pick an endpoint
  uint16_t exitPoint = 0;
  while(exitPoint == 0)
  {
    exitPoint = random(width - 2) + 1;
    if(maze->charAt(exitPoint - width) == ' ')
    {
      //Knock out exit
      maze->setCharAt(exitPoint, ' ');
    }
    else
    {
      //Choose a new exit
      exitPoint = 0;  
    }
  }
}

void MazeGenerator::removeBlock(String* maze, uint32_t pos)
{
  uint8_t avail = 0;
  uint8_t north = 0x01;
  uint8_t east  = 0x02;
  uint8_t west  = 0x04;

  uint16_t mazeSize = mWidth * mHeight;
  
  //Look north
  if((pos + (mWidth * 2)) < mazeSize)
  {
    if(maze->charAt(pos + mWidth) == '#' && maze->charAt(pos + (mWidth * 2)) == '#')
    {
       avail += north;
    }
  }
  //Look east
  if((pos + 2) < mazeSize && (pos % mWidth) + 2 <= mWidth)
  {
    if(maze->charAt(pos + 1) == '#' && maze->charAt(pos + 2) == '#')
    {
      avail += east;  
    }
  }
  //Look west
  if((pos % mWidth) > 1)
  {
    if(maze->charAt(pos - 1) == '#' && maze->charAt(pos - 2) == '#')
    {
      avail += west;  
    }  
  }

  //If there are no available directions, then exit from this recursion
  if(avail == 0)
    return;

  uint8_t dir = 0;
  randomSeed(analogRead(0));
  while(dir == 0)
  {
      dir = random(3);
      if(dir == 1 && avail & 0x01)
      {
        //Move north
        maze->setCharAt(pos + mWidth    ,' ');
        maze->setCharAt(pos + mWidth * 2,' ');

        pos = pos + mWidth * 2;
      }
      else if(dir == 2 && avail & 0x02)
      {
        //Move east  
        maze->setCharAt(pos + 1, ' ');
        maze->setCharAt(pos + 2, ' ');

        pos += 2;
      }
      else if(dir == 3 && avail & 0x04)
      {
        //Move west  
        maze->setCharAt(pos - 1, ' ');
        maze->setCharAt(pos - 2, ' ');

        pos -= 2;
      }
      else
      {
        //Choose a different direction
        dir = 0;  
      }
  }

  debugMaze(maze);
  //This step is done, recurse in for a new step
  removeBlock(maze, pos);
}

void MazeGenerator::debugMaze(String* maze)
{
   uint16_t start = 0;
   for(int i=0; i<mHeight; i++)
   {
     Serial.println(maze->substring(mWidth*i, mWidth*(i+1))); 
   }  
}
