#pragma once
#ifndef INVEMPTYSCREEN_H
#define INVEMPTYSCREEN_H

#include "InvScreenManagement.h"


class InvEmptyScreen : public InvGameScreen
{
public:
	InvEmptyScreen()
		: InvGameScreen()
	{

	}


	//Construct and load resources needed for this screen
	virtual void _initialize() override
	{

	}

	//Update logic for this screen
	virtual void _update(double elapsedSeconds) override
	{

	}

	//Give input to this screen
	virtual void _handleInput(SDL_Event* ev) override
	{

	}

	//Render this screen to the draw surface
	virtual void _draw() override
	{

	}

	//Remove and unload resources from this screen
	virtual void _cleanup() override
	{

	}
};

#endif