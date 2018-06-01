#pragma once
#ifndef INVSCREENMANAGEMENT_H
#define INVSCREENMANAGEMENT_H

#include "SDL.h"
#include <vector>

/*
SDL Game Screen Management
created: Brian MacIntosh 11/8/12

This system implements a screen-based architecture for games.
*/

class InvGameScreen;

//List/stack of active game screens
static std::vector<InvGameScreen*> screens;

//Get the topmost active screen
InvGameScreen* topScreen();

//Update screen stack
void updateScreens(double elapsedSeconds);

//Send input to screen stack
void screenHandleEvent(SDL_Event* ev);

//Draw screen stack
void drawScreens();

//Push a new game screen
void addGameScreen(InvGameScreen* screen);

//Close an existing game screen
void closeGameScreen(InvGameScreen* screen);

//Instantly remove an existing game screen
void _cleanupScreen(InvGameScreen* screen);

//Program is closing, unload all screens
void _cleanupAllScreens();


/*
Represents one functional "screen" unit
*/
class InvGameScreen
{
	friend void updateScreens(double elapsedSeconds);

public:
	//Does this screen pause screens below it?
	bool blocksUpdate;

	//Does this screen obscure screens below it?
	bool blocksDraw;

	//Set lengths of transition animations
	double transitionOnTime;
	double transitionOffTime;


	InvGameScreen()
	{
		blocksUpdate = true;
		blocksDraw = true;
		transitionOnTime = 0;
		transitionOffTime = 0;
		transitioningOff = false;
		transitioningOn = true;
		currentTransitionTime = 0;

		currentTransitionTime = 0;
	}


	//Construct and load resources needed for this screen
	virtual void _initialize() { };

	//Update logic for this screen
	virtual void _update(double elapsedSeconds) { };

	//Give input to this screen
	virtual void _handleInput(SDL_Event* ev) { };

	//Render this screen to the draw surface
	virtual void _draw() { };

	//Remove and unload resources from this screen
	virtual void _cleanup() { };


	//Tell this screen to exit
	void close()
	{
		transitioningOff = true;
		currentTransitionTime = 0;
	}

	bool transitioningOff;
	bool transitioningOn;

protected:

	//Get state of transitioning from 0 to 1 (0=off, 1=on)
	double transitionPosition()
	{
		if (transitioningOff)
		{
			return 1 - transitionOffTime / currentTransitionTime;
		}
		else if (transitioningOn)
		{
			return transitionOnTime / currentTransitionTime;
		}
		else
		{
			return 1;
		}
	}

private:

	double currentTransitionTime;

	//Update root information like transitions
	void _updateRoot(double elapsedSeconds)
	{
		if (transitioningOff)
		{
			currentTransitionTime += elapsedSeconds;
			if (currentTransitionTime >= transitionOnTime)
			{
				transitioningOff = false;
				_cleanupScreen(this);
			}
		}
		else if (transitioningOn)
		{
			currentTransitionTime += elapsedSeconds;
			if (currentTransitionTime >= transitionOnTime) transitioningOn = false;
		}
	}
};

#endif