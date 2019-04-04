/***
 * An interface that Battleships players (human or artificial) must implement
 */

#ifndef BATTLESHIPSPLAYER_H
#define BATTLESHIPSPLAYER_H


    //Piece constants.  If the byte & WATER = WATER, then this
    //square is not a hit.  

//Squares which cannot be hit
#define WATER       0x01
#define DESTROYED   0x03
#define DAMAGED     0x05
#define LAND        0x07
#define VALIDPOS    0x09 // Used during game setup
#define INVALIDPOS  0x0B // Used during game setup
#define MISSED      0x0D

//Square which can be hit
#define CARRIER     0x04 // 5 squares
#define BATTLESHIP  0x06 // 4 squares
#define DESTROYER   0x08 // 3 squares
#define SUBMARINE   0x0A // 3 squares
#define CRUISER     0x0C // 2 squares
#define DEATHSTAR   0x0E // End marker, not actually selectable

//Lengths of each ship
#define CARRIER_LENGTH      5
#define BATTLESHIP_LENGTH   4
#define DESTROYER_LENGTH    3
#define SUBMARINE_LENGTH    3
#define CRUISER_LENGTH      2
#define DEATHSTAR_LENGTH    1

#define ROTATION_HORIZONTAL 0
#define ROTATION_VERTICAL   1
  
//The placement order of the ships
const uint8_t SHIPORDER[6]    = {CARRIER,
                                 BATTLESHIP,
                                 DESTROYER,
                                 SUBMARINE,
                                 CRUISER,
                                 DEATHSTAR};

//Lengths of each type
const uint8_t SHIPSIZES[6]    = {CARRIER_LENGTH, 
                                 BATTLESHIP_LENGTH,
                                 DESTROYER_LENGTH, 
                                 SUBMARINE_LENGTH,
                                 CRUISER_LENGTH,
                                 DEATHSTAR_LENGTH};

typedef struct PlayerState
{
  uint8_t selectedShip;     // Bottom 4 bits are the index of the current ship being placed, top 4 are the length of the ship
                            // If the selected ship is DEATHSTAR, this indicates that the player has placed all of their ships 
                            // and is ready to play
  uint8_t positionState;    // Bottom 6 are the pixel index (00..63)
                            // bit 1 is the validity of the current position (1 if valid, 0 if invalid)
                            // bit 0 (MSB) is rotation (0 is horizontal, 1 is vertical)
  uint8_t controlState;     // bit 0,1,2 horizontal position (0..8)
                            // bit 3     horizontal button state
                            // bit 4,5,6 vertical position (0..8)
                            // bit 7     vertical button state

  //NB  This button state reflects the values returned by the PlayerControl button state method
  uint8_t buttonState;      // bit 7     fire button
                            // bit 6     horizontal encoder
                            // bit 5     vertical encoder
                            // bit 0-4   unused 

  uint8_t shipPositions[6]; //Copies of the positionState for each placed ship
};

class BattleshipsPlayer
{
  public:  
    virtual bool hasMoved() = 0;
    virtual PlayerState* getState() = 0;
    virtual void incrementState() = 0;
    virtual bool hasPressedButton(uint8_t button) = 0;
    virtual void reset() = 0;
    virtual uint8_t* getBoard() = 0;
    virtual uint8_t* getTrackingBoard() = 0;

    virtual void hit() = 0;
    virtual bool isDestroyed() = 0;
    virtual void debugPositions();

    virtual void opponentHasBeenHit() {}
    virtual void opponentShipDestroyed() {}
    
  protected:
    PlayerState playerState;
    uint8_t playerBoard[64];
    uint8_t trackingBoard[64];
    uint8_t remainingShips;
};
 #endif
