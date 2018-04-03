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


class MazeRacePlayer
{
    private:
	uint16_t currentPos;

    public:
      MazeRacePlayer();
      ~MazeRacePlayer();
      virtual uint16_t getMove(String* maze) = 0;
      void setPosition(uint16_t newPos);
      uint16_t getPosition();
};


#endif
