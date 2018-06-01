#include "stdafx.h"
#include "InvScreenManagement.h"

/*
SDL Game Screen Management
created: Brian MacIntosh 11/8/12
*/

static std::vector<InvGameScreen*> newscreens;

//Get the topmost active screen
InvGameScreen* topScreen()
{
	return screens.back();
}

//Update screen stack
void updateScreens(double elapsedSeconds)
{
	bool blocked = false;
	for (auto it = screens.rbegin(); it != screens.rend(); it++)
	{
		if (!(*it)->transitioningOff)
		{
			(*it)->_updateRoot(elapsedSeconds);
			if (!blocked) (*it)->_update(elapsedSeconds);
			if ((*it)->blocksUpdate) blocked = true;
		}
	}

	//Introduce new screens
	for (auto it = newscreens.begin(); it != newscreens.end(); it++)
	{
		screens.push_back((*it));
		newscreens.erase(it);
		break;
	}

	//Remove closed screens
	for (auto it = screens.begin(); it != screens.end(); it++)
	{
		if ((*it)->transitioningOff)
		{
			_cleanupScreen(*it);
			break;
		}
	}
}

//Feed input to screen stack
void screenHandleEvent(SDL_Event* ev)
{
	for (auto it = screens.rbegin(); it != screens.rend(); it++)
	{
		(*it)->_handleInput(ev);
		if ((*it)->blocksUpdate) return;
	}
}

//Draw screen stack
void drawScreens()
{
	//HACK: figure out breaking and precedence
	for (auto it = screens.rbegin(); it != screens.rend(); it++)
	{
		(*it)->_draw();
		if ((*it)->blocksDraw) return;
	}
}

//Push a new game screen
void addGameScreen(InvGameScreen* screen)
{
	screen->_initialize();
	newscreens.push_back(screen);
}

//Close an existing game screen
void closeGameScreen(InvGameScreen* screen)
{
	screen->close();
}

//Instantly remove an existing game screen
void _cleanupScreen(InvGameScreen* screen)
{
	screen->_cleanup();
	for (auto it = screens.begin(); it != screens.end(); it++)
	{
		if ((*it) == screen)
		{
			screens.erase(it);
			break;
		}
	}
}

//Program is closing, unload all screens
void _cleanupAllScreens()
{
	for (auto it = screens.begin(); it != screens.end(); it++)
	{
		(*it)->_cleanup();
	}
	screens.clear();
}