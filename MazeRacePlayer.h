/*****
* An interface that a MazeRace player (human or artificial) must implement
*/

#ifndef MAZERACEPLAYER_H
#define MAZERACEPLAYER_H

#include <Arduino.h>

#define MAZE_MOVE_NORTH 0
#define MAZE_MOVE_EAST  90
#define MAZE_MOVE_SOUTH 180
#define MAZE_MOVE_WEST  270

typedef struct MazeRacePlayerState
{
  //NB  This button state reflects the values returned by the PlayerControl button state method
  uint8_t buttonState;      // bit 7     fire button
                            // bit 6     horizontal encoder
                            // bit 5     vertical encoder
                            // bit 0-4   unused 
};

class MazeRacePlayer
{
    protected:
	    uint16_t mCurrentPos;
      MazeRacePlayerState mPlayerState;

    public:
      MazeRacePlayer();
      ~MazeRacePlayer();
      MazeRacePlayerState* getState();
      virtual uint16_t getMove(char* maze) = 0;
      virtual void setPosition(uint16_t newPos);
      uint16_t getPosition();
};


#endif
