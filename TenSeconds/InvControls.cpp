#include "stdafx.h"
#include "InvControls.h"
#include <list>
#include <map>

//New keys down on this frame
static std::list<SDL_keysym> newKeysDown;
static std::list<Uint8> newButtonsDown;
static std::list<Uint8> newJoyDown;

//New keys up on this frame
static std::list<SDL_keysym> newKeysUp;
static std::list<Uint8> newButtonsUp;
static std::list<Uint8> newJoyUp;

//Keys held down
static std::list<SDL_keysym> keysDown;
static std::list<Uint8> buttonsDown;
static std::list<Uint8> joyDown;

//Joy directional data
static std::map<Uint8, Uint8> joyHats;
static std::map<Uint8, Sint16> joyAxes;

//Mouse position
static int mousemovex;
static int mousemovey;
static int mousex;
static int mousey;

//Text input
static bool takeText = false;
static char* entryBuffer = NULL;
static int bufferLength = 1;
static bool entryNew = false;
static int entryMaxLen = -1;

//Stick input
static std::list<SDL_Joystick*> joysticks;


void controls_init()
{
	clearEntry();

	//Init sticks
	for (int c = 0; c < SDL_NumJoysticks(); c++)
	{
		SDL_Joystick* stick = SDL_JoystickOpen(c);
		joysticks.push_back(stick);
	}
}

//Feed an event into the controls system
void controls_handleEvent(SDL_Event* e)
{
	switch (e->type)
	{
	case SDL_KEYDOWN:
		newKeysDown.push_back(e->key.keysym);
		keysDown.push_back(e->key.keysym);
		
		if (takeText)
		{
			//Buffer text input
			Uint16 chr = e->key.keysym.unicode;
			if (chr >= 32 && chr < 127 && (entryMaxLen == -1 || bufferLength < entryMaxLen + 1))
			{
				entryNew = true;

				char* newBuffer = (char*)calloc(bufferLength+1, sizeof(char));
				if (entryBuffer != NULL)
					for (int c = 0; c < bufferLength; c++)
						newBuffer[c] = entryBuffer[c];
				bufferLength++;
				newBuffer[bufferLength - 2] = chr;
				newBuffer[bufferLength - 1] = '\0';

				//Realloc (TODO: don't realloc all the time (extra space))
				if (entryBuffer != NULL) free(entryBuffer);
				entryBuffer = newBuffer;
			}
			else if (e->key.keysym.sym == SDLK_BACKSPACE)
			{
				entryNew = true;

				bufferLength--;
				if (bufferLength < 1) bufferLength = 1;
				entryBuffer[bufferLength - 1] = '\0';
			}
		}
		break;
	case SDL_KEYUP:
		newKeysUp.push_back(e->key.keysym);

		//Remove from down list
		for (auto it = keysDown.begin(); it != keysDown.end(); it++)
			if ((*it).scancode == e->key.keysym.scancode)
			{
				keysDown.erase(it);
				break;
			}
		break;
	case SDL_MOUSEMOTION:
		mousemovex = e->motion.xrel;
		mousemovey = e->motion.yrel;
		mousex = e->motion.x;
		mousey = e->motion.y;
		//SDL_WarpMouse(200,200);
		break;
	case SDL_MOUSEBUTTONDOWN:
		newButtonsDown.push_back(e->button.button);
		buttonsDown.push_back(e->button.button);
		break;
	case SDL_MOUSEBUTTONUP:
		newButtonsUp.push_back(e->button.button);

		//Remove from down list
		for (auto it = buttonsDown.begin(); it != buttonsDown.end(); it++)
			if ((*it) == e->button.button)
			{
				buttonsDown.erase(it);
				break;
			}
		break;
	case SDL_JOYAXISMOTION:
		if (e->jaxis.value > 0 || e->jaxis.value < 0)
		printf("%d", e->jaxis.axis);
		break;
	case SDL_JOYBALLMOTION:
		if (e->jball.xrel > 0 || e->jball.xrel < 0)
		printf("%d", e->jball.which);
		break;
	case SDL_JOYHATMOTION:
		joyHats[e->jhat.which] = e->jhat.value;
		break;
	case SDL_JOYBUTTONDOWN:
		newJoyDown.push_back(e->jbutton.button);
		joyDown.push_back(e->jbutton.button);
		break;
	case SDL_JOYBUTTONUP:
		newJoyUp.push_back(e->jbutton.button);

		//Remove from down list
		for (auto it = joyDown.begin(); it != joyDown.end(); it++)
			if ((*it) == e->jbutton.button)
			{
				joyDown.erase(it);
				break;
			}
		break;
	}
}


//New frame, flip controls
void controls_step()
{
	newKeysDown.clear();
	newKeysUp.clear();
	newButtonsDown.clear();
	newButtonsUp.clear();
	newJoyDown.clear();
	newJoyUp.clear();
	mousemovex = 0;
	mousemovey = 0;
	entryNew = false;
}



bool keyPressed(SDLKey key)
{
	for (auto it = newKeysDown.begin(); it != newKeysDown.end(); it++)
		if ((*it).sym == key) return true;
	return false;
}

bool keyDown(SDLKey key)
{
	for (auto it = keysDown.begin(); it != keysDown.end(); it++)
		if ((*it).sym == key) return true;
	return false;
}

bool keyReleased(SDLKey key)
{
	for (auto it = newKeysUp.begin(); it != newKeysUp.end(); it++)
		if ((*it).sym == key) return true;
	return false;
}

bool keyUp(SDLKey key)
{
	return !keyDown(key);
}

bool anyKeyPressed()
{
	return newKeysDown.size() > 0;
}

bool mousePressed(Uint8 button)
{
	for (auto it = newButtonsDown.begin(); it != newButtonsDown.end(); it++)
		if ((*it) == button) return true;
	return false;
}

bool mouseDown(Uint8 button)
{
	for (auto it = buttonsDown.begin(); it != buttonsDown.end(); it++)
		if ((*it) == button) return true;
	return false;
}

bool mouseReleased(Uint8 button)
{
	for (auto it = newButtonsUp.begin(); it != newButtonsUp.end(); it++)
		if ((*it) == button) return true;
	return false;
}

bool mouseUp(Uint8 button)
{
	return !mouseDown(button);
}


int mouseX()
{
	return mousex;
}

int mouseY()
{
	return mousey;
}

int mouseMoveX()
{
	return mousemovex;
}

int mouseMoveY()
{
	return mousemovey;
}

bool joyButtonPressed(Uint8 key)
{
	for (auto it = newJoyDown.begin(); it != newJoyDown.end(); it++)
		if ((*it) == key) return true;
	return false;
}

bool joyButtonDown(Uint8 key)
{
	for (auto it = joyDown.begin(); it != joyDown.end(); it++)
		if ((*it) == key) return true;
	return false;
}

bool joyButtonReleased(Uint8 key)
{
	for (auto it = newJoyUp.begin(); it != newJoyUp.end(); it++)
		if ((*it) == key) return true;
	return false;
}

bool joyButtonUp(Uint8 key)
{
	return !joyButtonDown(key);
}

bool joyHatContains(Uint8 hat, Uint8 dir)
{
	return joyHats.count(hat) > 0 && joyHats[hat] & dir;
}

bool joyHatMatches(Uint8 hat, Uint8 dir)
{
	return joyHats.count(hat) > 0 && joyHats[hat] == dir;
}


char* getEntryBuffer()
{
	return entryBuffer;
}

void enableEntry()
{
	takeText = true;
	keysDown.clear();
	SDL_EnableUNICODE( SDL_ENABLE );
}

void disableEntry()
{
	takeText = false;
	SDL_EnableUNICODE( SDL_DISABLE );
}

void clearEntry()
{
	bufferLength = 1;
	if (entryBuffer != NULL) free(entryBuffer);
	entryBuffer = (char*)malloc(1);
	entryBuffer[0] = '\0';
}

bool entryHasNew()
{
	return entryNew;
}

void restrictEntryLen(int len)
{
	entryMaxLen = len;
}