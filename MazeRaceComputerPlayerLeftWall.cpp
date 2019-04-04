#include "MazeRaceComputerPlayerLeftWall.h"


MazeRaceComputerPlayerLeftWall::MazeRaceComputerPlayerLeftWall(uint8_t mazeWidth, uint8_t mazeHeight) : mMazeWidth(mazeWidth), mMazeHeight(mazeHeight)
{
	mCurrentDirection = MAZE_MOVE_SOUTH;
}

MazeRaceComputerPlayerLeftWall::~MazeRaceComputerPlayerLeftWall()
{

}

char MazeRaceComputerPlayerLeftWall::getMazeSquare(char* maze, uint16_t relativeDirection)
{
  //Covert the relative direction to an absolute direction
  uint16_t absDirection = (mCurrentDirection + relativeDirection) % 360;
  Serial.print("Looking ");Serial.println(absDirection);
  Serial.print("Current Pos:");Serial.println(mCurrentPos);
  bool valid = false;
  uint16_t squarePos = 0;
  switch(absDirection)
  {
    case MAZE_MOVE_SOUTH:
      Serial.println("Looking south");
      if((mCurrentPos + mMazeWidth) < mMazeWidth * mMazeHeight)
      {
        squarePos = mCurrentPos + mMazeWidth;
        valid = true;     
      }
      break;
    case MAZE_MOVE_EAST:
      //If we are not already on the eastern wall
      Serial.println("Looking east");
      if((mCurrentPos + 1) % mMazeWidth > 0)
      {
        squarePos = mCurrentPos + 1;
        valid = true; 
      } 
      break;
    case MAZE_MOVE_NORTH:
      Serial.println("Looking north");
      if(mCurrentPos > mMazeWidth)
      {
        squarePos = mCurrentPos - mMazeWidth;
        valid = true; 
      } 
      break;
    case MAZE_MOVE_WEST:
      Serial.println("Looking west");
      //eg mCurrentPos = 46, mMazeWidth = 15
      // 45 % 15 == 0, left hand wall (which is fine)
      //   mCurrentPos = 45, mMazeWidth = 15
      // 44 % 15 == 14, wrapped aroung to the right
      if((mCurrentPos - 1) % mMazeWidth < mMazeWidth - 1)
      {
        squarePos = mCurrentPos - 1;
        valid = true; 
      } 
      break;
  }
  if(!valid)
  {
    Serial.println(F("You seem to have left the maze"));
    return '!';
  }
  Serial.print("Returning char at position ");Serial.println(squarePos);
  return maze[squarePos];
}

//IMPORTANT - this move needs to have been validated.  No validation is repeated in this method
void MazeRaceComputerPlayerLeftWall::moveForwards()
{
  switch(mCurrentDirection)
  {
    case MAZE_MOVE_NORTH:
      mCurrentPos -= mMazeWidth;
      break;
    case MAZE_MOVE_EAST:
      mCurrentPos++;
      break;
    case MAZE_MOVE_WEST:
      mCurrentPos--;
      break;
    case MAZE_MOVE_SOUTH:
      mCurrentPos += mMazeWidth;
      break;
  }
}

uint16_t MazeRaceComputerPlayerLeftWall::getMove(char* maze)
{
  bool moved = false;
  while(!moved)
  {
    //Serial.println("Looking at the square to my left");
  	//Do we still have a wall to the left?
    char westSquare = getMazeSquare(maze, MAZE_MOVE_WEST);
    //Serial.print("Char to the west: '");Serial.print(westSquare);Serial.println("'");
    if(westSquare == '#')
    {
      //We have wall to the left, continue in the same direction (NORTH means 0 change to direction)
      char northSquare = getMazeSquare(maze, MAZE_MOVE_NORTH);
      //Serial.print("Char to the north: '");Serial.print(northSquare);Serial.println("'");
      if(northSquare == ' ')
      {
        //We can continue
        //Serial.println("moving forwards");
        moveForwards();
        moved = true;
      }
      else if(northSquare == '#')
      {
        //Serial.println("Blocked - lets turn right and try again");
        //We need to rotate right and start all over again  
        mCurrentDirection = (mCurrentDirection + 90) % 360;
        //And let the while loop take us back to the start
      }
    }
    else if(westSquare == ' ')
    {
      //Serial.println("We've lost the wall to the left");       
      //We need to rotate left and move forwards
      mCurrentDirection = (mCurrentDirection + 270) % 360;
      moveForwards();
      moved = true;
    }
  }

  //Serial.print("Move completed:");Serial.println(mCurrentPos);
  return mCurrentPos;
}
