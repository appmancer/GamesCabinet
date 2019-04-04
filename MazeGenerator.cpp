#include "MazeGenerator.h"

MazeGenerator::MazeGenerator()
{
}

MazeGenerator::~MazeGenerator()
{
}

void MazeGenerator::generate(char* maze, uint8_t width, uint8_t height)
{
  Serial.println(F("Generating a new maze"));
  Serial.print("Width:");Serial.print(width);Serial.print(" Height:"); Serial.println(height);
  uint16_t mazeSize = width * height; 
 
  mWidth = width;
  mHeight = height;

  for(int i=0; i<mazeSize; i++)
  {
    maze[i] = '#';  
  }

  Serial.println(F("Maze reset"));
  debugMaze(maze);

  //Pick a start point on the top of the maze
  randomSeed(analogRead(0));
  Serial.println("Choosing start");
  uint16_t start = random(width - 4) + 2;
  start |= 0x01; //Ensure that the start position is odd
  //Knock out the start block
  maze[start] = ' ';
  start += mWidth;
  maze[start] = ' ';

  Serial.print("Starting in column :");Serial.println(start);
  
  //Generate a path in the maze
  removeBlock(maze, start);

  debugMaze(maze);
  Serial.println("Choosing an exit");
  //Pick an endpoint
  uint16_t exitPoint = 0;
  while(exitPoint == 0)
  {
    exitPoint = mazeSize - random(width - 2) + 1;
    if(maze[exitPoint - width] == ' ')
    {
      //Knock out exit
      maze[exitPoint] = ' ';
      Serial.print("Exit in column ");Serial.println(exitPoint % 8);
    }
    else
    {
      //Choose a new exit
      exitPoint = 0;  
    }
  }
  debugMaze(maze);
}

void MazeGenerator::removeBlock(char* maze, uint32_t pos)
{
  uint8_t avail = 0;
  uint8_t north = 0x01;
  uint8_t south = 0x02;
  uint8_t east  = 0x04;
  uint8_t west  = 0x08;

  uint16_t mazeSize = mWidth * mHeight;
  
  //If there are no available directions, then exit from this recursion
  while(1)
  {
    avail = 0;
    //Look north
    if((pos + (mWidth * 2)) < mazeSize)
    {
      if(maze[pos + mWidth] == '#' && maze[pos + (mWidth * 2)] == '#')
      {
         avail += north;
      }
    }
    //Look south
    if((pos - (mWidth * 2)) > mWidth)
    {
      if(maze[pos - mWidth] == '#' && maze[pos - (mWidth * 2)] == '#')
      {
         avail += south;
      }
    }
    //Look east
    if(((pos + 1) % mWidth) < (mWidth-1) && ((pos + 2) % mWidth) < (mWidth-1))
    {
      if(maze[pos + 1] == '#' && maze[pos + 2] == '#')
      {
        avail += east;  
      }
    }
    //Look west
    if((pos % mWidth) > 2)
    {
      if(maze[pos - 1] == '#' && maze[pos - 2] == '#')
      {
        avail += west;  
      }  
    }
  
    //Serial.print("Avail:0x");Serial.println(avail, HEX);

    if(avail == 0x00)
    {
      //debugMaze(maze);
      break;
    }
  
   /* Serial.print("Traveller, exits are available to the ");
    if(avail & north) Serial.print("north ");
    if(avail & south) Serial.print("south ");
    if(avail & east) Serial.print("east ");
    if(avail & west) Serial.print("west ");
    Serial.println();*/

    uint8_t dir = 0;
    uint16_t newPos = 0;
    randomSeed(analogRead(0));
    while(dir == 0)
    {
        dir = random(4) + 1;
        if(dir == 1 && avail & north)
        {
          //Move north
          maze[pos + mWidth] = ' ';
          maze[pos + mWidth * 2] = ' ';
  
          newPos = pos + mWidth * 2;
        }
        else if(dir == 2 && avail & south)
        {
          //Move south
          maze[pos - mWidth] = ' ';
          maze[pos - mWidth * 2] = ' ';
  
          newPos = pos - mWidth * 2;
        }
        else if(dir == 3 && avail & east)
        {
          //Move east  
          maze[pos + 1] = ' ';
          maze[pos + 2] = ' ';
          newPos = pos + 2;
        }
        else if(dir == 4 && avail & west)
        {
          //Move west  
          maze[pos - 1] = ' ';
          maze[pos - 2] = ' ';
          newPos = pos - 2;
        }
        else
        {
          //Choose a different direction
          dir = 0;  
        }
    }
    //  debugMaze(maze);
    //This step is done, recurse in for a new step
    removeBlock(maze, newPos);
  }
  //debugMaze(maze);
  Serial.print(".");
}

void MazeGenerator::debugMaze(char* maze)
{
   uint16_t start = 0;
   char buff[mWidth + 1];
   for(int i=0; i<mHeight; i++)
   {
     memcpy(buff, maze + (mWidth * i), mWidth);
     buff[mWidth] = '\0';
     Serial.println(buff); 
   }  
   Serial.println();
}
