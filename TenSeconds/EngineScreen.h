#pragma once
#ifndef _ENGINESCREEN_H
#define _ENGINESCREEN_H

#include "InvScreenManagement.h"
#include "InvTexture.h"
#include <list>
#include <SDL_mixer.h>

class Player;
class Camera;
class Map;
class Actor;

class EngineScreen : public InvGameScreen
{
public:
	EngineScreen();

	//Construct and load resources needed for this screen
	virtual void _initialize() override;

	//Update logic for this screen
	virtual void _update(double elapsedSeconds);

	//Give input to this screen
	virtual void _handleInput(SDL_Event* ev);

	//Render this screen to the draw surface
	virtual void _draw();

	//Remove and unload resources from this screen
	virtual void _cleanup();

	void spawnBread();

	InvVectorf2 project(InvVectorf3 worldPosition);

	Player* m_Player;

	Camera* m_Camera;

	Map* m_Map;

	std::list<Actor*> m_Enemies;

	InvVectorf2 mouseWorldPos;

	InvTexture* m_Black;

	Mix_Music* bgm;

private:

	InvTexture* m_SpaceTex;
	InvTexture* m_Earthside;
	float spaceangle;
	float fadePos;
	float spawnTimer;
};

EngineScreen* getInstance();

#endif