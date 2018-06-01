#pragma once

#include "InvSpacialData.h"

class InvTexture;

#define ITEM_NONE       -1
#define ITEM_CHAINLINK  0
#define ITEM_METALBARS  1
#define ITEM_FLASHLIGHT 2
#define ITEM_SHIELD     3

#define ITEM_COUNT 4

class ItemBase
{
public:

	ItemBase()
		: weight(1), buildsTile(-1), flashlight(false),
		shield(false), consumed(false)
	{
	}

	int weight;

	int buildsTile;

	bool flashlight;
	bool shield;
	bool consumed;

	InvTexture* texture;
};


void loadItems();

ItemBase getItemBase(int itemtype);