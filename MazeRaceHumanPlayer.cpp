#include "MazeRaceHumanPlayer.h"

MazeRaceHumanPlayer::MazeRaceHumanPlayer(PlayerControl* control, uint8_t mazeWidth, uint8_t mazeHeight) : playerControl(control), mMazeWidth(mazeWidth), mMazeHeight(mazeHeight)
{

}

MazeRaceHumanPlayer::~MazeRaceHumanPlayer()
{

}


void MazeRaceHumanPlayer::setPosition(uint16_t newPos)
{
	mCurrentPos = newPos;
	//We need to convert the position into an X and Y to set the encoder values
	uint8_t row = newPos / mMazeWidth;
	uint8_t col = newPos % mMazeWidth;
  //Serial.print("Setting initial position row:");Serial.print(row);Serial.print(" col:");Serial.println(col);
	playerControl->hEncoder.write(col * 4);
	playerControl->vEncoder.write(row * 4);
}


uint16_t MazeRaceHumanPlayer::getMove(char* maze)
{  
  //TODO - is there a chance that if you turn a dial *really* quickly it will burst through a wall?

  int8_t hPos = playerControl->hEncoder.read() / 4; //Each encoder 'click' is 4 readable positions
  int8_t vPos = playerControl->vEncoder.read() / 4;

  //Check to see if we can move to this new position
  uint16_t newPos = vPos * mMazeWidth + hPos; 
  if(newPos == mCurrentPos) return mCurrentPos; //Abort
  
  //Serial.print("Player pos: ");Serial.print(hPos);Serial.print(", ");Serial.println(vPos);

  if(hPos < 0) hPos = 0;
  if(hPos > mMazeWidth) hPos = mMazeWidth;
  if(vPos < 0) vPos = 0;
  if(vPos > mMazeHeight) vPos = mMazeHeight;

  //Serial.print("Can we move to the new square ");Serial.println(newPos);
  if(maze[newPos] == ' ')
  {
    //Serial.println("Its empty, lets move there");
    //OK, its an empty space
    mCurrentPos = newPos;
  }
  else
  {
    //Serial.println("Oops its blocked, back to where you were");
    //Our way is blocked- write the old position back
    playerControl->hEncoder.write((mCurrentPos % mMazeWidth) * 4);
    playerControl->vEncoder.write((mCurrentPos / mMazeWidth) * 4);
  }

  //Serial.print("Human player new position ");Serial.println(mCurrentPos);
  return mCurrentPos;
}

