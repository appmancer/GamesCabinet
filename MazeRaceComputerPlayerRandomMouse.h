#ifndef MAZE_RACE_COMPUTER_PLAYER_RANDOM_MOUSE_H
#define MAZE_RACE_COMPUTER_PLAYER_RANDOM_MOUSE_H

#include "MazeRacePlayer.h"

//Implements a computer player of MazeRace who will play a strategy of following the wall to his left

class MazeRaceComputerPlayerRandomMouse : public MazeRacePlayer
{
	private:
		uint16_t mCurrentDirection;
    uint8_t mMazeWidth;
    uint8_t mMazeHeight;
		uint16_t mReadyTime;

  protected:
    void moveForwards();
    char getMazeSquare(char* maze, uint16_t relativeDirection);
    
	public:
		MazeRaceComputerPlayerRandomMouse(uint8_t mazeWidth, uint8_t mazeHeight);
		~MazeRaceComputerPlayerRandomMouse();

		uint16_t getMove(char* maze);
};

#endif
