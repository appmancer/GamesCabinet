#ifndef MAZE_RACE_COMPUTER_PLAYER_LEFT_WALL.H
#define MAZE_RACE_COMPUTER_PLAYER_LEFT_WALL.H

#include "MazeRacePlayer.h"

//Implements a computer player of MazeRace who will play a strategy of following the wall to his left

class MazeRaceComputerPlayerLeftWall : public MazeRacePlayer
{
	private:
		uint8_t mCurrentDirection;
	
	public:
		MazeRaceComputerPlayerLeftWall();
		~MazeRaceComputerPlayerLeftWall();

		uint16_t getMove(String* maze);
};

#endif
