#include "stdafx.h"
#include "ItemManager.h"
#include "InvTexture.h"
#include "TileManager.h"


static ItemBase itembases[ITEM_COUNT];

void loadItems()
{
	itembases[ITEM_CHAINLINK].buildsTile = TILE_CHAINLINK;
	itembases[ITEM_CHAINLINK].weight = 1;
	itembases[ITEM_CHAINLINK].texture = new InvTexture("Resource/Item/chainlink.png");

	itembases[ITEM_METALBARS].buildsTile = TILE_METALFENCE;
	itembases[ITEM_METALBARS].weight = 1;
	itembases[ITEM_METALBARS].texture = new InvTexture("Resource/Item/metalbars.png");

	itembases[ITEM_FLASHLIGHT].buildsTile = -1;
	itembases[ITEM_FLASHLIGHT].weight = 1;
	itembases[ITEM_FLASHLIGHT].flashlight = true;
	itembases[ITEM_FLASHLIGHT].texture = new InvTexture("Resource/Item/flashlight.png");

	itembases[ITEM_SHIELD].buildsTile = -1;
	itembases[ITEM_SHIELD].weight = 1;
	itembases[ITEM_SHIELD].consumed = true;
	itembases[ITEM_SHIELD].shield = true;
	itembases[ITEM_SHIELD].texture = new InvTexture("Resource/Item/bread.png");
}

ItemBase getItemBase(int itemtype)
{
	return itembases[itemtype];
}