#include "stdafx.h"
#include "TileManager.h"
#include "InvTexture.h"
#include "PirateParams.h"
#include "ItemManager.h"
#include "EngineScreen.h"
#include "Map.h"


static TileBase* TileBases;

static InvTexture* BlackTexture;

static const int FogGap = 0;
static const int FogHeight = 500;

static const bool Hax = false;


//Clockwise
void TileBase::draw(int angle, InvVectorf2 tilepos)
{
	//Rotate walls
	bool hasSide[4];
	for (int c = 0; c < 4; c++)
		hasSide[c] = false;
	hasSide[0] = hasNorth;
	hasSide[1] = hasEast;
	hasSide[2] = hasSouth;
	hasSide[3] = hasWest;

	//Ground
	{
		InvVectorf3 pos = InvVectorf3(tilepos.x * TileSize, tilepos.y * TileSize, 0);

		if (m_Ground != NULL)
			glBindTexture(GL_TEXTURE_2D, *(m_Ground->texId));

		float FogletSize = TileSize / (float)FogUpsample;

		for (int fx = 0; fx < FogUpsample; fx++)
		{
			for (int fy = 0; fy < FogUpsample; fy++)
			{
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
				glTranslatef(pos.x, pos.y, 1);

				glBegin(GL_QUADS);
				if (!Hax && getInstance()->m_Map->getFogletCornerFogged(InvVectorf2(tilepos.x*FogUpsample+fx, tilepos.y*FogUpsample+fy)))
					glColor3f(0, 0, 0);
				else
					glColor3f(1, 1, 1);
				glTexCoord2f(fx / (float)FogUpsample, fy / (float)FogUpsample);
				glVertex3f(fx*FogletSize, fy*FogletSize, pos.z);
				if (!Hax && getInstance()->m_Map->getFogletCornerFogged(InvVectorf2(tilepos.x*FogUpsample+fx+1, tilepos.y*FogUpsample+fy)))
					glColor3f(0, 0, 0);
				else
					glColor3f(1, 1, 1);
				glTexCoord2f((fx+1) / (float)FogUpsample, fy / (float)FogUpsample);
				glVertex3f((fx+1)*FogletSize, fy*FogletSize, pos.z);
				if (!Hax && getInstance()->m_Map->getFogletCornerFogged(InvVectorf2(tilepos.x*FogUpsample+fx+1, tilepos.y*FogUpsample+fy+1)))
					glColor3f(0, 0, 0);
				else
					glColor3f(1, 1, 1);
				glTexCoord2f((fx+1) / (float)FogUpsample, (fy+1) / (float)FogUpsample);
				glVertex3f((fx+1)*FogletSize, (fy+1)*FogletSize, pos.z);
				if (!Hax && getInstance()->m_Map->getFogletCornerFogged(InvVectorf2(tilepos.x*FogUpsample+fx, tilepos.y*FogUpsample+fy+1)))
					glColor3f(0, 0, 0);
				else
					glColor3f(1, 1, 1);
				glTexCoord2f(fx / (float)FogUpsample, (fy+1) / (float)FogUpsample);
				glVertex3f(fx*FogletSize, (fy+1)*FogletSize, pos.z);
				glEnd();

				glColor3f(1, 1, 1);

				glPopMatrix();
			}
		}

		glBindTexture(GL_TEXTURE_2D, NULL);
	}
	
	if (m_Side != NULL && centered
		&& !getInstance()->m_Map->getTileFogged(tilepos))
	{
		m_Side->draw(
			InvVectorf3(TileSize * (tilepos.x + 0.5f), TileSize * (tilepos.y + 1), m_Height),
			InvVectorf3(TileSize * (tilepos.x + 0.5f), TileSize * tilepos.y,       m_Height),
			InvVectorf3(TileSize * (tilepos.x + 0.5f), TileSize * tilepos.y,       0),
			InvVectorf3(TileSize * (tilepos.x + 0.5f), TileSize * (tilepos.y + 1), 0));

		//Fog
		if (!seethrough)
			BlackTexture->draw(
				InvVectorf3(TileSize * (tilepos.x + 0.5f), TileSize * (tilepos.y + 1), FogHeight),
				InvVectorf3(TileSize * (tilepos.x + 0.5f), TileSize * tilepos.y,       FogHeight),
				InvVectorf3(TileSize * (tilepos.x + 0.5f), TileSize * tilepos.y,       m_Height + FogGap),
				InvVectorf3(TileSize * (tilepos.x + 0.5f), TileSize * (tilepos.y + 1), m_Height + FogGap));

		m_Side->draw(
			InvVectorf3(TileSize * tilepos.x,       TileSize * (tilepos.y + 0.5f), m_Height),
			InvVectorf3(TileSize * (tilepos.x + 1), TileSize * (tilepos.y + 0.5f), m_Height),
			InvVectorf3(TileSize * (tilepos.x + 1), TileSize * (tilepos.y + 0.5f), 0),
			InvVectorf3(TileSize * tilepos.x,       TileSize * (tilepos.y + 0.5f), 0));

		//Fog
		if (!seethrough)
			BlackTexture->draw(
				InvVectorf3(TileSize * tilepos.x,       TileSize * (tilepos.y + 0.5f), FogHeight),
				InvVectorf3(TileSize * (tilepos.x + 1), TileSize * (tilepos.y + 0.5f), FogHeight),
				InvVectorf3(TileSize * (tilepos.x + 1), TileSize * (tilepos.y + 0.5f), m_Height + FogGap),
				InvVectorf3(TileSize * tilepos.x,       TileSize * (tilepos.y + 0.5f), m_Height + FogGap));
	}
	else if (m_Side != NULL)
	{
		if (hasSide[(3 - angle + 4) % 4]
			&& !getInstance()->m_Map->getXEdgeFogged(tilepos)) //West
		{
			m_Side->draw(
				InvVectorf3(TileSize * tilepos.x, TileSize * (tilepos.y + 1), m_Height),
				InvVectorf3(TileSize * tilepos.x, TileSize * tilepos.y,       m_Height),
				InvVectorf3(TileSize * tilepos.x, TileSize * tilepos.y,       0),
				InvVectorf3(TileSize * tilepos.x, TileSize * (tilepos.y + 1), 0));

			//Fog
			if (!seethrough)
				BlackTexture->draw(
					InvVectorf3(TileSize * tilepos.x, TileSize * (tilepos.y + 1), FogHeight),
					InvVectorf3(TileSize * tilepos.x, TileSize * tilepos.y,       FogHeight),
					InvVectorf3(TileSize * tilepos.x, TileSize * tilepos.y,       m_Height + FogGap),
					InvVectorf3(TileSize * tilepos.x, TileSize * (tilepos.y + 1), m_Height + FogGap));
		}
		if (hasSide[(1 - angle + 4) % 4]
			&& !getInstance()->m_Map->getXEdgeFogged(InvVectorf2(tilepos.x+1, tilepos.y))) //East
		{
			m_Side->draw(
				InvVectorf3(TileSize * (tilepos.x + 1), TileSize * tilepos.y,       m_Height),
				InvVectorf3(TileSize * (tilepos.x + 1), TileSize * (tilepos.y + 1), m_Height),
				InvVectorf3(TileSize * (tilepos.x + 1), TileSize * (tilepos.y + 1), 0),
				InvVectorf3(TileSize * (tilepos.x + 1), TileSize * tilepos.y,       0));

			//Fog
			if (!seethrough)
				BlackTexture->draw(
					InvVectorf3(TileSize * (tilepos.x + 1), TileSize * tilepos.y,       FogHeight),
					InvVectorf3(TileSize * (tilepos.x + 1), TileSize * (tilepos.y + 1), FogHeight),
					InvVectorf3(TileSize * (tilepos.x + 1), TileSize * (tilepos.y + 1), m_Height + FogGap),
					InvVectorf3(TileSize * (tilepos.x + 1), TileSize * tilepos.y,       m_Height + FogGap));
		}
		if (hasSide[(2 - angle + 4) % 4]
			&& !getInstance()->m_Map->getYEdgeFogged(InvVectorf2(tilepos.x, tilepos.y+1))) //South
		{
			m_Side->draw(
				InvVectorf3(TileSize * tilepos.x,       TileSize * (tilepos.y + 1), m_Height),
				InvVectorf3(TileSize * (tilepos.x + 1), TileSize * (tilepos.y + 1), m_Height),
				InvVectorf3(TileSize * (tilepos.x + 1), TileSize * (tilepos.y + 1), 0),
				InvVectorf3(TileSize * tilepos.x,       TileSize * (tilepos.y + 1), 0));

			//Fog
			if (!seethrough)
				BlackTexture->draw(
					InvVectorf3(TileSize * tilepos.x,       TileSize * (tilepos.y + 1), FogHeight),
					InvVectorf3(TileSize * (tilepos.x + 1), TileSize * (tilepos.y + 1), FogHeight),
					InvVectorf3(TileSize * (tilepos.x + 1), TileSize * (tilepos.y + 1), m_Height + FogGap),
					InvVectorf3(TileSize * tilepos.x,       TileSize * (tilepos.y + 1), m_Height + FogGap));
		}
		if (hasSide[(0 - angle + 4) % 4]
			&& !getInstance()->m_Map->getXEdgeFogged(tilepos)) //North
		{
			m_Side->draw(
				InvVectorf3(TileSize * (tilepos.x + 1), TileSize * tilepos.y, m_Height),
				InvVectorf3(TileSize * (tilepos.x),     TileSize * tilepos.y, m_Height),
				InvVectorf3(TileSize * (tilepos.x),     TileSize * tilepos.y, 0),
				InvVectorf3(TileSize * (tilepos.x + 1), TileSize * tilepos.y, 0));

			//Fog
			if (!seethrough)
				BlackTexture->draw(
					InvVectorf3(TileSize * (tilepos.x + 1), TileSize * tilepos.y, FogHeight),
					InvVectorf3(TileSize * (tilepos.x),     TileSize * tilepos.y, FogHeight),
					InvVectorf3(TileSize * (tilepos.x),     TileSize * tilepos.y, m_Height + FogGap),
					InvVectorf3(TileSize * (tilepos.x + 1), TileSize * tilepos.y, m_Height + FogGap));
		}
	}
}


void loadTiles()
{
	BlackTexture = new InvTexture("Resource/black.png");

	TileBases = new TileBase[TILE_COUNT];

	TileBases[TILE_DIRT].passable = true;
	TileBases[TILE_DIRT].buildable = true;
	TileBases[TILE_DIRT].m_Ground = new InvTexture("Resource/Tile/ground.png");

	TileBases[TILE_BLD1_TL].m_Side = new InvTexture("Resource/Tile/building1_side1.png");
	TileBases[TILE_BLD1_TL].m_Height = 50.0f;
	TileBases[TILE_BLD1_TL].hasNorth = true;
	TileBases[TILE_BLD1_TL].hasWest = true;

	TileBases[TILE_BLD1_T].m_Side = new InvTexture("Resource/Tile/building1_side1.png");
	TileBases[TILE_BLD1_T].m_Height = 50.0f;
	TileBases[TILE_BLD1_T].hasNorth = true;

	TileBases[TILE_BLD1_TB].m_Side = new InvTexture("Resource/Tile/building1_side1.png");
	TileBases[TILE_BLD1_TB].hasNorth = true;
	TileBases[TILE_BLD1_TB].hasSouth = true;
	TileBases[TILE_BLD1_TB].m_Height = 50.0f;

	TileBases[TILE_BLD1_TLR].m_Side = new InvTexture("Resource/Tile/building1_side1.png");
	TileBases[TILE_BLD1_TLR].hasSouth = true;
	TileBases[TILE_BLD1_TLR].hasEast = true;
	TileBases[TILE_BLD1_TLR].hasWest = true;
	TileBases[TILE_BLD1_TLR].m_Height = 50.0f;

	TileBases[TILE_BLD1_MID1].m_Side = new InvTexture("Resource/Tile/building1_side1.png");
	TileBases[TILE_BLD1_MID1].m_Height = 50.0f;

	TileBases[TILE_ROAD].m_Ground = new InvTexture("Resource/Tile/road.png");
	TileBases[TILE_ROAD].passable = true;
	TileBases[TILE_ROAD].buildable = true;

	TileBases[TILE_SIDEWALK].m_Ground = new InvTexture("Resource/Tile/sidewalk.png");
	TileBases[TILE_SIDEWALK].passable = true;
	TileBases[TILE_SIDEWALK].buildable = true;

	TileBases[TILE_CHAINLINK].m_Side = new InvTexture("Resource/Tile/chainlink_side.png");
	TileBases[TILE_CHAINLINK].m_Ground = TileBases[TILE_DIRT].m_Ground;
	TileBases[TILE_CHAINLINK].hasNorth = true;
	TileBases[TILE_CHAINLINK].hasSouth = true;
	TileBases[TILE_CHAINLINK].hasEast = true;
	TileBases[TILE_CHAINLINK].hasWest = true;
	TileBases[TILE_CHAINLINK].m_Height = 30;
	TileBases[TILE_CHAINLINK].seethrough = true;
	TileBases[TILE_CHAINLINK].breakablility = 4.0f;
	TileBases[TILE_CHAINLINK].dropsItem = ITEM_CHAINLINK;

	TileBases[TILE_METALFENCE].m_Side = new InvTexture("Resource/Tile/metalfence_side.png");
	TileBases[TILE_METALFENCE].m_Ground = TileBases[TILE_DIRT].m_Ground;
	TileBases[TILE_METALFENCE].hasNorth = true;
	TileBases[TILE_METALFENCE].hasSouth = true;
	TileBases[TILE_METALFENCE].hasEast = true;
	TileBases[TILE_METALFENCE].hasWest = true;
	TileBases[TILE_METALFENCE].m_Height = 30;
	TileBases[TILE_METALFENCE].centered = true;
	TileBases[TILE_METALFENCE].seethrough = true;
	TileBases[TILE_METALFENCE].breakablility = 6.0f;
	TileBases[TILE_METALFENCE].dropsItem = ITEM_METALBARS;
}


TileBase getTileBase(int tiletype)
{
	return TileBases[tiletype];
}