#include "MazeRacePlayer.h"

MazeRacePlayer::MazeRacePlayer()
{
	mCurrentPos = 65535; //Use max long to denote an unset position
}

MazeRacePlayer::~MazeRacePlayer()
{

}

void MazeRacePlayer::setPosition(uint16_t newPos)
{
	mCurrentPos = newPos;
}

uint16_t MazeRacePlayer::getPosition()
{
	return mCurrentPos;
}

MazeRacePlayerState* MazeRacePlayer::getState()
{
 return &mPlayerState;
}
