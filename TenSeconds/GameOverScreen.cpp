#include "stdafx.h"
#include "GameOverScreen.h"
#include "InvControls.h"
#include "PirateParams.h"


GameOverScreen::GameOverScreen()
{
	blocksDraw = true;
	blocksUpdate = true;
}

//Construct and load resources needed for this screen
void GameOverScreen::_initialize()
{
	words = new InvTexture("Resource/Menu/gameover.png");
	cursor = new InvTexture("Resource/Menu/gameover_cursor.png");

	selectSfx = Mix_LoadWAV("Resource/Audio/bing.wav");
	Mix_VolumeChunk(selectSfx, 40);

	cursorOscPos = 0;
}

//Update logic for this screen
void GameOverScreen::_update(double elapsedSeconds)
{
	cursorOscPos += elapsedSeconds * 15;
	cursorOscPos = fmodf(cursorOscPos, 2*PI);

	if (anyKeyPressed())
	{
		Mix_PlayChannel(0, selectSfx, 0);
		close();
	}
}

//Give input to this screen
void GameOverScreen::_handleInput(SDL_Event* ev)
{

}

//Render this screen to the draw surface
void GameOverScreen::_draw()
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
		InvVectorf3(screenWidth / 2, screenHeight / 2, 0),
		InvVectorf2(words->getWidth(), words->getHeight()) / 2, 0);
	if (cursorOscPos > PI)
		cursor->draw(
			InvVectorf3(screenWidth / 2, screenHeight / 2, 0),
			InvVectorf2(words->getWidth(), words->getHeight()) / 2, 0);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

//Remove and unload resources from this screen
void GameOverScreen::_cleanup()
{
	delete words;
	delete cursor;
	Mix_FreeChunk(selectSfx);
}