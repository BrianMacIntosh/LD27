#pragma once

#include "TileManager.h"
#include "ItemManager.h"

#define FogUpsample 5

class Camera;
class InvTexture;

class Tile
{
public:

	Tile()
		: TileType(TILE_DIRT), ItemType(ITEM_NONE), angle(0), condition(0)
	{
		init();
	};

	Tile(int type, int iangle)
		: TileType(type), ItemType(ITEM_NONE), angle(iangle), condition(0)
	{
		init();
	};

	void init()
	{
		itemangle = 360 * (rand() % 1000) / 1000.0f;
	};

	void Build(int itemtype)
	{
		TileType = getItemBase(itemtype).buildsTile;
		angle = 0;
		condition = getTileBase(TileType).breakablility;
	};

	int TileType;
	int ItemType;

	int angle;

	float itemangle;

	float condition;
};

class Map
{
public:

	Map();
	~Map();

	void update(double elapsed);

	void fogAll();
	void unfogInRadius(InvVectorf2 position, float radius);
	void unfogInCone(InvVectorf2 position, float radius, float anglerad, float arcrad);

	bool raycast(InvVectorf2 start, InvVectorf2 end) const;

	void draw(Camera* camera);

	void drawGui();

	void fromImage(const char* file);

	void fixBuildings();

	void repairAll();

	bool coordValid(InvVectorf2 coord) const;

	void makeBuilding(InvRectangle rect);

	bool slide(InvVectorf2 &position, InvVectorf2 &velocity, float radius, InvVectorf2 &outtile) const;

	Tile getTile(InvVectorf2 tilepos) const;
	bool getFogletCornerFogged(InvVectorf2 fogpos) const;
	bool getTileFogged(InvVectorf2 tilepos) const;
	bool getXEdgeFogged(InvVectorf2 tilepos) const;
	bool getYEdgeFogged(InvVectorf2 tilepos) const;

	Tile** m_Map;
	bool** m_Fog;

	InvVectorf2 tutorialBread;
	bool breadGot;
	InvVectorf2 tutorialFence;
	bool fenceGot;
};