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
		MazeRaceHumanPlayer(PlayerControl* control);
		~MazeRaceHumanPlayer();
		uint8_t getMove(String* maze);
};


#endif
