#include "stdafx.h"
#include "MenuScreen.h"
#include "PirateParams.h"
#include "InvControls.h"
#include "InvScreenManagement.h"
#include "EngineScreen.h"
#include "Quit.h"

MenuScreen::MenuScreen()
{
	blocksDraw = true;
	blocksUpdate = true;
}

//Construct and load resources needed for this screen
void MenuScreen::_initialize()
{
	globe = new InvTexture("Resource/Menu/globe.png");
	globe_blur = new InvTexture("Resource/Menu/globeblur.png");
	cursor = new InvTexture("Resource/Menu/cursor.png");
	words = new InvTexture("Resource/Menu/words.png");
	black = new InvTexture("Resource/black.png");

	selectSfx = Mix_LoadWAV("Resource/Audio/bing.wav");
	Mix_VolumeChunk(selectSfx, 40);

	transition = false;
	fadePos = 0;

	blurOscPos = 0;
	cursorOscPos = 0;
}

//Update logic for this screen
void MenuScreen::_update(double elapsedSeconds)
{
	cursorOscPos += elapsedSeconds * 15;
	cursorOscPos = fmodf(cursorOscPos, 2*PI);
	blurOscPos += elapsedSeconds * 25;
	blurOscPos = fmodf(blurOscPos, PI);

	if (transition)
	{
		fadePos += elapsedSeconds;
		if (fadePos > 1)
		{
			addGameScreen(new EngineScreen());
			transition = false;
			fadePos = 0;
		}
	}

	if (keyPressed(SDLK_ESCAPE))
		quit();
	if (anyKeyPressed() && !transition)
	{
		Mix_PlayChannel(0, selectSfx, 0);
		transition = true;
		fadePos = 0;
	}
}

//Give input to this screen
void MenuScreen::_handleInput(SDL_Event* ev)
{

}

//Render this screen to the draw surface
void MenuScreen::_draw()
{
	//GUI
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0f, screenWidth, screenHeight, 0.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	words->draw(
		InvVectorf3(0, 0, 0),
		InvVectorf2(), 0);
	if (cursorOscPos > PI)
		cursor->draw(
		InvVectorf3(0, 0, 0),
		InvVectorf2(), 0);

	globe->draw(
		InvVectorf3(0, screenHeight, 0),
		InvVectorf2(0, globe->getHeight()));
	glColor4f(1, 1, 1, sinf(blurOscPos) * 0.25f);
	globe_blur->draw(
		InvVectorf3(0, screenHeight, -0.1),
		InvVectorf2(0, globe_blur->getHeight()));
	glColor4f(1, 1, 1, 1);

	if (transition)
	{
		glColor4f(1, 1, 1, fadePos);
		black->draw(
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
void MenuScreen::_cleanup()
{
	delete globe;
	delete globe_blur;
	delete cursor;
	delete words;
	delete black;
	Mix_FreeChunk(selectSfx);
}