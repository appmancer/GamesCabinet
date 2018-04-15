#include "MazeRaceComputerPlayerRandomMouse.h"


MazeRaceComputerPlayerRandomMouse::MazeRaceComputerPlayerRandomMouse(uint8_t mazeWidth, uint8_t mazeHeight) : mMazeWidth(mazeWidth), mMazeHeight(mazeHeight)
{
	mCurrentDirection = MAZE_MOVE_SOUTH;
	mReadyTime = millis();
}

MazeRaceComputerPlayerRandomMouse::~MazeRaceComputerPlayerRandomMouse()
{

}

char MazeRaceComputerPlayerRandomMouse::getMazeSquare(char* maze, uint16_t relativeDirection)
{
  //Covert the relative direction to an absolute direction
  uint16_t absDirection = (mCurrentDirection + relativeDirection) % 360;
  //Serial.print("Looking ");Serial.println(absDirection);
  //Serial.print("Current Pos:");Serial.println(mCurrentPos);
  bool valid = false;
  uint16_t squarePos = 0;
  switch(absDirection)
  {
    case MAZE_MOVE_SOUTH:
      //Serial.println("Looking south");
      if((mCurrentPos + mMazeWidth) < mMazeWidth * mMazeHeight)
      {
        squarePos = mCurrentPos + mMazeWidth;
        valid = true;     
      }
      break;
    case MAZE_MOVE_EAST:
      //If we are not already on the eastern wall
      //Serial.println("Looking east");
      if((mCurrentPos + 1) % mMazeWidth > 0)
      {
        squarePos = mCurrentPos + 1;
        valid = true; 
      } 
      break;
    case MAZE_MOVE_NORTH:
      //Serial.println("Looking north");
      if(mCurrentPos > mMazeWidth)
      {
        squarePos = mCurrentPos - mMazeWidth;
        valid = true; 
      } 
      break;
    case MAZE_MOVE_WEST:
      //Serial.println("Looking west");
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
  //Serial.print("Returning char at position ");Serial.println(squarePos);
  return maze[squarePos];
}

//IMPORTANT - this move needs to have been validated.  No validation is repeated in this method
void MazeRaceComputerPlayerRandomMouse::moveForwards()
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
  //Choose a random speed to walk at, between 100ms and 700ms
  mReadyTime = millis() + random(100, 700);
}

uint16_t MazeRaceComputerPlayerRandomMouse::getMove(char* maze)
{
    if(mReadyTime > millis()) return mCurrentPos;

    bool moved = false;
    char northSquare = getMazeSquare(maze, MAZE_MOVE_NORTH);
    char westSquare = getMazeSquare(maze, MAZE_MOVE_WEST);
    char eastSquare = getMazeSquare(maze, MAZE_MOVE_EAST);

    //Check for dead end
    if(northSquare == '#' && westSquare == '#' && eastSquare == '#')
    {
	//Dead end - turn around
        mCurrentDirection = (mCurrentDirection + MAZE_MOVE_SOUTH) % 360;
	moveForwards();
	moved = true;
    }    
    else
    {
	while(!moved)
	{
		//Choose a random direction.  We don't need to reseed
		uint16_t dir = random(3);
		if(dir == 0 && northSquare == ' ')
		{
			moveForwards();
			moved = true;
		}
		else if(dir == 1 && westSquare == ' ')
		{
			mCurrentDirection = (mCurrentDirection + MAZE_MOVE_WEST) % 360;
			moveForwards();
			moved = true;
		}
		else if(dir == 2 && eastSquare == ' ')
		{
			mCurrentDirection = (mCurrentDirection + MAZE_MOVE_EAST) % 360;
			moveForwards();
			moved = true;
		}
	}
    }
  //Serial.print("Move completed:");Serial.println(mCurrentPos);
  return mCurrentPos;
}
