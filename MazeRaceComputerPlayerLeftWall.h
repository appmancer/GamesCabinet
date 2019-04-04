#ifndef MAZE_RACE_COMPUTER_PLAYER_LEFT_WALL_H
#define MAZE_RACE_COMPUTER_PLAYER_LEFT_WALL_H

#include "MazeRacePlayer.h"

//Implements a computer player of MazeRace who will play a strategy of following the wall to his left

class MazeRaceComputerPlayerLeftWall : public MazeRacePlayer
{
	private:
		uint16_t mCurrentDirection;
    uint8_t mMazeWidth;
    uint8_t mMazeHeight;

  protected:
    void moveForwards();
    char getMazeSquare(char* maze, uint16_t relativeDirection);
    
	public:
		MazeRaceComputerPlayerLeftWall(uint8_t mazeWidth, uint8_t mazeHeight);
		~MazeRaceComputerPlayerLeftWall();

		uint16_t getMove(char* maze);
};

#endif
