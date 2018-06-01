#pragma once

#include "InvScreenManagement.h"
#include "InvTexture.h"
#include <list>
#include <SDL_mixer.h>

class Player;
class Camera;
class Map;
class Actor;

class MenuScreen : public InvGameScreen
{
public:
	MenuScreen();

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

private:

	InvTexture* globe;
	InvTexture* globe_blur;
	InvTexture* words;
	InvTexture* cursor;
	InvTexture* black;

	Mix_Chunk* selectSfx;

	bool transition;
	float fadePos;

	float blurOscPos;
	float cursorOscPos;
};