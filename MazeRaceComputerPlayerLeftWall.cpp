#include "MazeRaceComputerPlayerLeftWall.h"


MazeRaceComputerPlayerLeftWall::MazeRaceComputerPlayerLeftWall()
{
	currentDirection = MAZE_MOVE_NORTH;
}

MazeRaceComputerPlayerLeftWall::~MazeRaceComputerPlayerLeftWall()
{

}

uint16_t MazeRaceComputerPlayerLeftWall::getMove(String* maze)
{
	uint16_t newPos;
	//Do we still have a wall to the left?
	uint8_t leftWallDirection = (currentDirection + 270) % 360;
	uint16_t leftWallPos = 0;
	bool valid = false;
	switch(leftWallDirection)
	{
		case MAZE_MOVE_NORTH:
			if((currentPos - mMazeWidth) >= 0)
			{
				leftWallPos = currentPos - mMazeWidth;
				valid = true;			
			}
			break;
		case MAZE_MOVE_EAST:
			//If we are not already on the eastern wall
			if((currentPos + 1) % mMazeWidth > 0)
			{
				leftWallPos = currentPos + 1;
				valid = true; 
			} 
			break;
		case MAZE_MOVE_SOUTH:
			if((currentPos + mMazeWidth) < mMazeWidth * mMazeHeight)
			{
				leftWallPos = currentPos + mMazeWidth;
				valid = true; 
			} 
			break;
		case MAZE_MOVE_WEST:
			//If we are not already on the western wall
			if((currentPos - 1) % mMazeWidth > 1)
			{
				leftWallPos = currentPos - 1;
				valid = true; 
			} 
			break;
		
	}

	if(!valid)
	{
		//Change direction and look again
		currentDirection = (currentDirection + 90) % 360;
		return getMove(maze);
	}
	else
	{
		//Get the char at leftWallPos
		char leftWall = maze.charAt(leftWallPos);
		//Is it a wall #?
		if(leftWall == '#')
		{
			//Go forwards
			switch(currentDirection)
			{
				case MAZE_MOVE_NORTH:
					
				//I've stopped here because I need to generalise 2 rules.  I need a method to determine a 
				//position to the left (relative position)
				//I also need to determine the square in front of me
				/*
					method to test to the left
					method to test to the front
					method to rotate right
				*/
			}			
		}
		//Is it a space ' '?
			//Turn left
	}
}
