#include "MazeRacePlayer.h"

MazeRacePlayer::MazeRacePlayer()
{
	currentPos = 65535; //Use max long to denote an unset position
}

MazeRacePlayer::~MazeRacePlayer()
{

}

void MazeRacePlayer::setPosition(uint16_t newPos)
{
	currentPos = newPos;
}

uint16_t MazeRacePlayer::getPosition()
{
	return currentPos;
}
