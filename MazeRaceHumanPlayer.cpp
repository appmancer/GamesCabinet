#include "MazeRaceHumanPlayer.h"

MazeRaceHumanPlayer::MazeRaceHumanPlayer(PlayerControl* control, uint8_t mazeWidth, uint8_t mazeHeight) : playerControl(control), mMazeWidth(mazeWidth), mMazeHeight(mazeHeight)
{

}

MazeRaceHumanPlayer::~MazeRaceHumanPlayer()
{

}


void MazeRacePlayer::setPosition(uint16_t newPos)
{
	currentPos = newPos;
	//We need to convert the position into an X and Y to set the encoder values
	uint8_t row = newPos / mMazeWidth;
	uint8_t col = newPos % mMazeWidth;
	playerControl->hEncoder.write(col);
	playerControl->vEncoder.write(row);
}


uint16_t MazeRaceHumanPlayer::getMove(String* maze)
{  
  //TODO - is there a chance that if you turn a dial *really* quickly it will burst through a wall?

  int8_t hPos = playerControl->hEncoder.read() / 4; //Each encoder 'click' is 4 readable positions
  int8_t vPos = playerControl->vEncoder.read() / 4;

  //Check to see if we can move to this new position
  uint16_t newPos = vPos * mMazeWidth + hPos; 
  if(maze.charAt(' '))
  {
    //OK, its an empty space
    currentPos = newPos;
  }
  else
  {
    //Our way is blocked- write the old position back
    playerControl->hEncoder.write(currentPos % mMazeWidth);
    playerControl->vEncoder.write(currentPos / mMazeWidth);
  }

  return currentPos;
}

