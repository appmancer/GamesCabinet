#ifndef MAZE_RACE_HUMAN_PLAYER_H


#include "MazeRacePlayer.h"
#include "PlayerControl.h"

class MazeRaceHumanPlayer : public MazeRacePlayer
{
	private:
    PlayerControl* playerControl;
		uint8_t mCurrentDirection;
		uint8_t mMazeWidth;
		uint8_t mMazeHeight;
	
	public:
		MazeRaceHumanPlayer(PlayerControl* control, uint8_t mazeWidth, uint8_t mazeHeight);
		~MazeRaceHumanPlayer();
    uint16_t getMove(char* maze);
    //We need to set the initial position
    void setPosition(uint16_t newPos);
};


#endif
