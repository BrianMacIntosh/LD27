#include "stdafx.h"
#include "EngineScreen.h"
#include "InvControls.h"
#include "PirateParams.h"

#include "Player.h"
#include "Camera.h"
#include "Map.h"
#include "Crawler.h"

static const float SpaceRotSpeed = 0.2;

static const float SpawnTime = 10.0f;

EngineScreen* instance;

EngineScreen* getInstance() { return instance; }

EngineScreen::EngineScreen()
{
	spawnTimer = SpawnTime;

	blocksDraw = true;
	blocksUpdate = true;

	instance = this;

	m_Player = new Player();
	m_Camera = new Camera(m_Player);
	m_Map = new Map();

	m_Map->fromImage("Resource/level.bmp");
	m_Map->repairAll();

	spawnBread();

	m_Player->spawn();

	m_SpaceTex = new InvTexture("Resource/space.png");
	m_Earthside = new InvTexture("Resource/earthside.png");
	m_Black = new InvTexture("Resource/black.png");
	spaceangle = 0;

	fadePos = 1;

	for (int c = 0; c < 40; c++)
		m_Enemies.push_back(new Crawler(InvVectorf2(
			rand() % (MapSizeX / 2)*TileSize + MapSizeX*TileSize / 2,
			rand() % MapSizeY*TileSize)));

	bgm = Mix_LoadMUS("Resource/audio/final.mp3");
	Mix_VolumeMusic(55);
	Mix_PlayMusic(bgm, -1);
}

//Construct and load resources needed for this screen
void EngineScreen::_initialize()
{
	
}

InvVectorf2 EngineScreen::project(InvVectorf3 worldPosition)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	m_Camera->setView();
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glPopMatrix();

	double x, y, z;
	gluProject(worldPosition.x, worldPosition.y, worldPosition.z,
		modelview,
		projection,
		viewport,
		&x,
		&y,
		&z);
	return InvVectorf2(x + screenWidth / 2, -(y - screenHeight) + screenHeight / 2);
}

//Update logic for this screen
void EngineScreen::_update(double elapsedSeconds)
{
	if (keyPressed(SDLK_ESCAPE))
		close();

	spaceangle += elapsedSeconds * SpaceRotSpeed;

	spawnTimer -= elapsedSeconds;
	if (spawnTimer <= 0)
	{
		spawnTimer = SpawnTime;
		m_Enemies.push_back(new Crawler(InvVectorf2(rand() % MapSizeX*TileSize, rand() % MapSizeY*TileSize)));
	}

	//Calculate mouse world pos
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	m_Camera->setZeroMatrix();
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glPopMatrix();

	GLdouble projx, projy, projz = 0;
	gluUnProject(mouseX(), (float)viewport[3] - mouseY(), 1, modelview, projection, viewport, &projx, &projy, &projz);

	mouseWorldPos.x = m_Camera->getPosition().x - m_Camera->getHeight() * projx / projz;
	mouseWorldPos.y = m_Camera->getPosition().y - m_Camera->getHeight() * projy / projz;

	//Fade
	if (fadePos > 0)
	{
		fadePos -= elapsedSeconds;
	}

	//Update
	m_Map->fogAll();

	m_Player->update(elapsedSeconds);
	m_Map->update(elapsedSeconds);
	m_Camera->update(elapsedSeconds);
	for (auto it = m_Enemies.begin(); it != m_Enemies.end(); it++)
		(*it)->update(elapsedSeconds);
}

//Give input to this screen
void EngineScreen::_handleInput(SDL_Event* ev)
{

}

//Render this screen to the draw surface
void EngineScreen::_draw()
{
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	m_Camera->setView();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	m_Map->draw(m_Camera);
	m_Player->draw();
	for (auto it = m_Enemies.begin(); it != m_Enemies.end(); it++)
		(*it)->draw();

	//Space
	m_SpaceTex->draw(
		InvVectorf3(-screenWidth / 2, -screenWidth / 2, -99),
		InvVectorf3( screenWidth / 2, -screenWidth / 2, -99),
		InvVectorf3( screenWidth / 2,  screenWidth / 2, -99),
		InvVectorf3(-screenWidth / 2,  screenWidth / 2, -99),
		m_Camera->getPosition(),
		spaceangle);

	//The edges of the earth
	InvVectorf3 corepoint = InvVectorf3(
		m_Camera->getPosition().x + m_Camera->centeroff.normalize().x * m_Camera->distout * 70,
		m_Camera->getPosition().y + m_Camera->centeroff.normalize().y * m_Camera->distout * 70,
		-95);
	m_Earthside->draw(
		InvVectorf3(0, 0, 0),
		InvVectorf3(MapSizeX * TileSize, 0, 0),
		corepoint,
		corepoint);
	m_Earthside->draw(
		InvVectorf3(0, 0, 0),
		InvVectorf3(0, MapSizeY * TileSize, 0),
		corepoint,
		corepoint);
	m_Earthside->draw(
		InvVectorf3(MapSizeX * TileSize, MapSizeY * TileSize, 0),
		InvVectorf3(MapSizeX * TileSize, 0, 0),
		corepoint,
		corepoint);
	m_Earthside->draw(
		InvVectorf3(MapSizeX * TileSize, MapSizeY * TileSize, 0),
		InvVectorf3(0, MapSizeY * TileSize, 0),
		corepoint,
		corepoint);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//GUI
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0f, screenWidth, screenHeight, 0.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	m_Player->drawGui();
	m_Map->drawGui();

	//Fader
	if (fadePos > 0)
	{
		glColor4f(1, 1, 1, fadePos);
		m_Black->draw(
			InvVectorf3(0,           0,            -1),
			InvVectorf3(screenWidth, 0,            -1),
			InvVectorf3(screenWidth, screenHeight, -1),
			InvVectorf3(0,           screenHeight, -1));
		glColor4f(1, 1, 1, 1);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

//Remove and unload resources from this screen
void EngineScreen::_cleanup()
{
	delete m_Camera;
	delete m_Player;
	delete m_Map;
	delete m_Black;
	delete m_SpaceTex;
	delete m_Earthside;
	Mix_FreeMusic(bgm);
}

void EngineScreen::spawnBread()
{
	int breakout = 100;
	while (breakout > 0)
	{
		breakout--;
		int x = rand() % MapSizeX;
		int y = rand() % MapSizeY;
		if (m_Map->m_Map[x][y].ItemType == ITEM_NONE
			&& getTileBase(m_Map->m_Map[x][y].TileType).buildable)
		{
			m_Map->m_Map[x][y].ItemType = ITEM_SHIELD;
			return;
		}
	}
}