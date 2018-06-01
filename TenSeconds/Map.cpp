#include "stdafx.h"
#include "Map.h"
#include "Camera.h"
#include "InvTexture.h"
#include "PirateParams.h"
#include "EngineScreen.h"
#include "Camera.h"
#include "Player.h"
#include "RasterLineIterator.h"
#include "InvTexture.h"
#include <SDL_mixer.h>

static bool init = false;
static InvTexture* tutorials[3];
static Mix_Chunk* breaksound;

Map::Map()
{
	m_Map = new Tile*[MapSizeX];
	for (int c = 0; c < MapSizeX; c++)
		m_Map[c] = new Tile[MapSizeY];

	m_Fog = new bool*[MapSizeX * FogUpsample];
	for (int c = 0; c < MapSizeX * FogUpsample; c++)
		m_Fog[c] = new bool[MapSizeY * FogUpsample];

	if (!init)
	{
		init = true;
		tutorials[0] = new InvTexture("Resource/Ui/instruction1.png");
		tutorials[1] = new InvTexture("Resource/Ui/instruction2.png");
		tutorials[2] = new InvTexture("Resource/Ui/instruction3.png");
		breaksound = Mix_LoadWAV("Resource/Audio/break.wav");
		Mix_VolumeChunk(breaksound, 50);
	}
}

Map::~Map()
{
	for (int c = 0; c < MapSizeX; c++)
		delete m_Map[c];
	delete m_Map;

	for (int c = 0; c < MapSizeX * FogUpsample; c++)
		delete m_Fog[c];
	delete m_Fog;
}

void Map::update(double elapsed)
{
	for (int x = 0; x < MapSizeX; x++)
	{
		for (int y = 0; y < MapSizeY; y++)
		{
			//Break stuff
			if (m_Map[x][y].condition < 0)
			{
				m_Map[x][y].condition = 0;
				m_Map[x][y].ItemType = getTileBase(m_Map[x][y].TileType).dropsItem;
				m_Map[x][y].TileType = TILE_DIRT;

				if ((getInstance()->m_Player->getPosition() - InvVectorf2(x, y) * TileSize).lengthSquared() < 320*320)
				{
					Mix_PlayChannel(0, breaksound, 0);
				}
			}

			//Bread emits light
			if (m_Map[x][y].ItemType == ITEM_SHIELD)
			{
				unfogInRadius(InvVectorf2(x + 0.5f, y + 0.5f) * TileSize, 1);
			}
		}
	}
}

void Map::fogAll()
{
	for (int x = 0; x < MapSizeX * FogUpsample; x++)
		for (int y = 0; y < MapSizeY * FogUpsample; y++)
			m_Fog[x][y] = true;
}

void Map::unfogInRadius(InvVectorf2 position, float radius)
{
	radius *= FogUpsample;
	InvVectorf2 tilepos = position / (TileSize / (float)FogUpsample);
	int maxx = min((float)MapSizeX * FogUpsample - 1, tilepos.x + radius);
	int maxy = min((float)MapSizeY * FogUpsample - 1, tilepos.y + radius);
	for (int x = max(0.0f, tilepos.x - radius); x < maxx; x++)
	{
		for (int y = max(0.0f, tilepos.y - radius); y < maxy; y++)
		{
			if ((InvVectorf2(x, y) - tilepos).lengthSquared() < radius * radius)
				m_Fog[x][y] = false;
		}
	}
}


inline float angdist(float A, float B)
{
	return min(min(abs(A-B), abs(A-(B+2*PI))), abs((A+2*PI)-B));
}

void Map::unfogInCone(InvVectorf2 position, float radius, float anglerad, float arcrad)
{
	radius *= FogUpsample;
	InvVectorf2 tilepos = position / (TileSize / (float)FogUpsample);
	int maxx = min((float)MapSizeX * FogUpsample, tilepos.x + radius);
	int maxy = min((float)MapSizeY * FogUpsample, tilepos.y + radius);
	for (int x = max(0.0f, tilepos.x - radius); x < maxx; x++)
	{
		for (int y = max(0.0f, tilepos.y - radius); y < maxy; y++)
		{
			float angle = atan2f(x - tilepos.x, y - tilepos.y);
			if ((InvVectorf2(x, y) - tilepos).lengthSquared() < radius * radius
				&& angdist(anglerad, angle) < arcrad / 2)
				m_Fog[x][y] = false;
		}
	}
}

bool Map::raycast(InvVectorf2 start, InvVectorf2 end) const
{
	RasterLineIterator it = RasterLineIterator(start / TileSize, end / TileSize);
	while (it.hasNext())
	{
		it.next();
		if (!coordValid(it.current())) return false;
		TileBase tb = getTileBase(m_Map[(int)it.current().x][(int)it.current().y].TileType);
		if (!tb.seethrough && !tb.passable)
			return true;
	}
	return false;
}

void Map::draw(Camera* camera)
{
	int camtilesizex = 10;
	int camtilesizey = 7;
	InvVectorf2 camtilepos = getInstance()->m_Camera->getPosition() / TileSize;

	for (int x = max(0, (int)(camtilepos.x-camtilesizex)); x < min(MapSizeX, (int)(camtilepos.x+camtilesizex)); x++)
	{
		for (int y = max(0, (int)(camtilepos.y-camtilesizey)); y < min(MapSizeY, (int)(camtilepos.y+camtilesizey)); y++)
		{
			//Draw tile
			getTileBase(m_Map[x][y].TileType).draw(m_Map[x][y].angle, InvVectorf2(x, y));

			//Draw item
			if (m_Map[x][y].ItemType > ITEM_NONE
				&& !getTileFogged(InvVectorf2(x, y)))
			{
				getItemBase(m_Map[x][y].ItemType).texture->draw(
					InvVectorf3((x + 0.5) * TileSize, (y + 0.5) * TileSize, 2),
					InvVectorf2(TileSize, TileSize) / 2,
					m_Map[x][y].itemangle);
			}
		}
	}
}

void Map::drawGui()
{
	InvVectorf2 pos;
	InvTexture* tex = NULL;

	if (!fenceGot
		&& !getTileFogged(tutorialFence))
	{
		if (m_Map[(int)tutorialFence.x][(int)tutorialFence.y].TileType == TILE_CHAINLINK)
		{
			tex = tutorials[0];
			pos = tutorialFence * TileSize;
		}
		else if (m_Map[(int)tutorialFence.x][(int)tutorialFence.y].ItemType == ITEM_CHAINLINK)
		{
			tex = tutorials[1];
			pos = tutorialFence * TileSize;
		}
		else
			fenceGot = true;
	}
	else if (!breadGot
		&& !getTileFogged(tutorialBread))
	{
		if (m_Map[(int)tutorialBread.x][(int)tutorialBread.y].ItemType == ITEM_SHIELD)
		{
			tex = tutorials[2];
			pos = tutorialBread * TileSize;
		}
		else
			breadGot = true;
	}

	if (tex != NULL)
	{
		InvVectorf2 scr = getInstance()->project(InvVectorf3(pos.x, pos.y, 0));
		tex->draw(
			InvVectorf3(scr.x, scr.y, 0),
			InvVectorf2(tex->getWidth() / 2, tex->getHeight()));
	}
}

#define RGB(R, G, B) ((B) | ((G) << 8) | ((R) << 16))

void Map::fromImage(const char* file)
{
	SDL_Surface* surface = IMG_Load(file);
	assert(surface != NULL);

	for (int x = 0; x < MapSizeX; x++)
	{
		for (int y = 0; y < MapSizeY; y++)
		{
			 long b = ((long*)surface->pixels)[x + y * MapSizeX];
			 if (b == RGB(136, 0, 21))
				 m_Map[x][y] = Tile(TILE_BLD1_MID1, 0);
			 else if (b == RGB(0, 0, 0) || b == RGB(255, 242, 0))
				 m_Map[x][y] = Tile(TILE_ROAD, 0);
			 else if (b == RGB(255, 127, 39))
				 m_Map[x][y] = Tile(TILE_CHAINLINK, 0);
			 else if (b == RGB(127, 127, 127))
				 m_Map[x][y] = Tile(TILE_SIDEWALK, 0);
			 else if (b == RGB(0, 0, 255))
				 getInstance()->m_Player->SpawnPoint = InvVectorf2(x, y);
			 else if (b == RGB(255, 201, 14))
				m_Map[x][y] = Tile(TILE_METALFENCE, 0);
			 else if (b == RGB(0, 255, 255))
			 {
				 m_Map[x][y].ItemType = ITEM_SHIELD;
				 tutorialBread = InvVectorf2(x, y);
				 breadGot = false;
			 }
			 else if (b == RGB(0, 127, 127))
			 {
				 m_Map[x][y] = Tile(TILE_CHAINLINK, 0);
				 tutorialFence = InvVectorf2(x, y);
				 fenceGot = false;
			 }
		}
	}

	SDL_FreeSurface(surface);

	fixBuildings();
}

void Map::fixBuildings()
{
	for (int x = 0; x < MapSizeX; x++)
	{
		for (int y = 0; y < MapSizeY; y++)
		{
			if (TYPEISBLD1(m_Map[x][y].TileType))
			{
				bool left =   x > 0          && TYPEISBLD1(m_Map[x-1][y].TileType);
				bool right =  x < MapSizeX-1 && TYPEISBLD1(m_Map[x+1][y].TileType);
				bool top =    y < MapSizeY-1 && TYPEISBLD1(m_Map[x][y+1].TileType);
				bool bottom = y > 0          && TYPEISBLD1(m_Map[x][y-1].TileType);
				int count = !!left + !!right + !!top + !!bottom;
				if (count == 4)
					m_Map[x][y].TileType = TILE_BLD1_MID1;
				else if (count == 3)
				{
					m_Map[x][y].TileType = TILE_BLD1_T;
					if (left && right && top)
						m_Map[x][y].angle = 0;
					else if (right && top && bottom)
						m_Map[x][y].angle = 3;
					else if (top && bottom && left)
						m_Map[x][y].angle = 1;
					else if (bottom && left && right)
						m_Map[x][y].angle = 2;
				}
				else if (count == 2)
				{
					m_Map[x][y].TileType = TILE_BLD1_TL;
					if (top && bottom)
					{
						m_Map[x][y].TileType = TILE_BLD1_TB;
						m_Map[x][y].angle = 1;
					}
					else if (left && right)
					{
						m_Map[x][y].TileType = TILE_BLD1_TB;
						m_Map[x][y].angle = 0;
					}
					else if (top)
					{
						m_Map[x][y].angle = (left ? 1 : 0);
					}
					else if (bottom)
					{
						m_Map[x][y].angle = (left ? 2 : 3);
					}
				}
				else if (count == 1)
				{
					if (top)
					{
						m_Map[x][y].TileType = TILE_BLD1_TLR;
						m_Map[x][y].angle = 2;
					}
					else if (bottom)
					{
						m_Map[x][y].TileType = TILE_BLD1_TLR;
						m_Map[x][y].angle = 0;
					}
					else if (left)
					{
						m_Map[x][y].TileType = TILE_BLD1_TLR;
						m_Map[x][y].angle = 3;
					}
					else if (right)
					{
						m_Map[x][y].TileType = TILE_BLD1_TLR;
						m_Map[x][y].angle = 1;
					}
				}
			}
		}
	}
}

void Map::repairAll()
{
	for (int x = 0; x < MapSizeX; x++)
	{
		for (int y = 0; y < MapSizeY; y++)
		{
			m_Map[x][y].condition = getTileBase(m_Map[x][y].TileType).breakablility;
		}
	}
}

bool Map::coordValid(InvVectorf2 coord) const
{
	return coord.x >= 0 && coord.x < MapSizeX
		&& coord.y >= 0 && coord.y < MapSizeY;
}

void Map::makeBuilding(InvRectangle rect)
{
	m_Map[(int)rect.left][(int)rect.bottom] = Tile(TILE_BLD1_TL, 0);
	m_Map[(int)rect.right][(int)rect.bottom] = Tile(TILE_BLD1_TL, 1);
	m_Map[(int)rect.right][(int)rect.top] = Tile(TILE_BLD1_TL, 2);
	m_Map[(int)rect.left][(int)rect.top] = Tile(TILE_BLD1_TL, 3);

	for (int c = rect.bottom + 1; c < rect.top; c++)
	{
		m_Map[(int)rect.left][c] = Tile(TILE_BLD1_T, 3);
		m_Map[(int)rect.right][c] = Tile(TILE_BLD1_T, 1);

		for (int x = rect.left + 1; x < rect.right; x++)
			m_Map[x][c] = Tile(TILE_BLD1_MID1, 0);
	}
	for (int x = rect.left + 1; x < rect.right; x++)
	{
		m_Map[x][(int)rect.top] = Tile(TILE_BLD1_T, 2);
		m_Map[x][(int)rect.bottom] = Tile(TILE_BLD1_T, 0);
	}
}

bool Map::slide(InvVectorf2 &position, InvVectorf2 &velocity, float radius, InvVectorf2 &outtile) const
{
	InvVectorf2 normal;

	InvRectangle slider = InvRectangle(
		(position.x - radius),
		(position.x + radius),
		(position.y - radius),
		(position.y + radius));

	int lefttile   = slider.left   / TileSize;
	int righttile  = slider.right  / TileSize;
	int toptile    = slider.top    / TileSize;
	int bottomtile = slider.bottom / TileSize;

	for (int x = max(0, lefttile); x <= min(MapSizeX-1, righttile); x++)
	{
		for (int y = max(0, toptile); y <= min(MapSizeY-1, bottomtile); y++)
		{
			if (!getTileBase(m_Map[x][y].TileType).passable)
			{
				InvRectangle tile = InvRectangle(
					x * TileSize, (x+1) * TileSize,
					y * TileSize, (y+1) *  TileSize);
				InvRectangle intersect = tile.Intersection(slider);

				if (!intersect.Empty())
				{
					//Set normal
					InvVectorf2 newnorm;
					if (intersect.width() < intersect.height())
					{
						if (tile.center().x > slider.center().x)
							newnorm.x = -1;
						else
							newnorm.x = 1;
						position.x += intersect.width() * newnorm.x;
					}
					else
					{
						if (tile.center().y > slider.center().y)
							newnorm.y = -1;
						else
							newnorm.y = 1;
						position.y += intersect.height() * newnorm.y;
					}

					if (newnorm.x != 0)
						normal.x = newnorm.x;
					if (newnorm.y != 0)
						normal.y = newnorm.y;

					outtile.x = x;
					outtile.y = y;
				}
			}
		}
	}

	//Remove velocity on collided axis
	velocity.x *= 1-abs(normal.x);
	velocity.y *= 1-abs(normal.y);

	return normal != InvVectorf2();
}

Tile Map::getTile(InvVectorf2 tilepos) const
{
	assert(coordValid(tilepos));
	return m_Map[(int)tilepos.x][(int)tilepos.y];
}

bool Map::getFogletCornerFogged(InvVectorf2 fogpos) const
{
	fogpos.x = clamp((int)fogpos.x, 0, MapSizeX * FogUpsample - 1);
	fogpos.y = clamp((int)fogpos.y, 0, MapSizeY * FogUpsample - 1);
	if (m_Fog[(int)fogpos.x][(int)fogpos.y])
		return true;
	if (fogpos.x > 0 && m_Fog[(int)fogpos.x-1][(int)fogpos.y])
		return true;
	if (fogpos.y > 0 && m_Fog[(int)fogpos.x][(int)fogpos.y-1])
		return true;
	if (fogpos.y > 0 && fogpos.x > 0 && m_Fog[(int)fogpos.x-1][(int)fogpos.y-1])
		return true;
	return false;
}

bool Map::getTileFogged(InvVectorf2 tilepos) const
{
	tilepos.x = clamp((int)tilepos.x, 0, MapSizeX-1) * FogUpsample;
	tilepos.y = clamp((int)tilepos.y, 0, MapSizeY-1) * FogUpsample;
	for (int fx = tilepos.x; fx < tilepos.x + FogUpsample; fx++)
		for (int fy = tilepos.y; fy < tilepos.y + FogUpsample; fy++)
			if (!m_Fog[fx][fy])
				return false;
	return true;
}

bool Map::getXEdgeFogged(InvVectorf2 tilepos) const
{
	tilepos.x = clamp((int)tilepos.x, 0, MapSizeX-1) * FogUpsample;
	tilepos.y = clamp((int)tilepos.y, 0, MapSizeY-1) * FogUpsample;
	for (int fy = tilepos.y; fy < tilepos.y + FogUpsample; fy++)
	{
		if (!m_Fog[(int)tilepos.x][fy])
			return false;
		if (tilepos.x > 0 && !m_Fog[(int)tilepos.x-1][fy])
			return false;
	}
	return true;
}

bool Map::getYEdgeFogged(InvVectorf2 tilepos) const
{
	tilepos.x = clamp((int)tilepos.x, 0, MapSizeX-1) * FogUpsample;
	tilepos.y = clamp((int)tilepos.y, 0, MapSizeY-1) * FogUpsample;
	for (int fx = tilepos.x; fx < tilepos.x + FogUpsample; fx++)
	{
		if (!m_Fog[fx][(int)tilepos.y])
			return false;
		if (tilepos.y > 0 && !m_Fog[fx][(int)tilepos.y-1])
			return false;
	}
	return true;
}