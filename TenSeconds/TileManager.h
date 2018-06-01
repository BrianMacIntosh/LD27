#pragma once

#include "InvSpacialData.h"

class InvTexture;


#define TILE_DIRT       0
#define TILE_BLD1_TL    1
#define TILE_BLD1_T     2
#define TILE_BLD1_TB    3
#define TILE_BLD1_TLR   4
#define TILE_BLD1_MID1  5
#define TILE_CHAINLINK  6
#define TILE_ROAD       7
#define TILE_SIDEWALK   8
#define TILE_METALFENCE 9

#define TILE_COUNT   10

#define TYPEISBLD1(X) ((X) >= TILE_BLD1_TL && (X) <= TILE_BLD1_MID1)

class TileBase
{
public:

	TileBase()
		: m_Side(NULL), m_Ground(NULL), passable(false), m_Height(0),
		hasNorth(false), hasSouth(false), hasEast(false), hasWest(false),
		centered(false), seethrough(false), breakablility(0), dropsItem(-1), buildable(false)
	{

	}

	void draw(int angle, InvVectorf2 tilepos);

	InvTexture* m_Side;
	InvTexture* m_Ground;

	bool hasNorth;
	bool hasSouth;
	bool hasEast;
	bool hasWest;

	bool centered;
	bool seethrough;

	float breakablility;
	int dropsItem;
	bool buildable; //Can build things on top

	float m_Height;

	bool passable;
};


void loadTiles();

TileBase getTileBase(int tiletype);