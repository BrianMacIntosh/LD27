#pragma once
#ifndef INVCONTROLS_H
#define INVCONTROLS_H

#include "SDL.h"

#define XBOX_A 0
#define XBOX_B 1
#define XBOX_X 2
#define XBOX_Y 3
#define XBOX_BUMPER_L 4
#define XBOX_BUMPER_R 5
#define XBOX_BACK 6
#define XBOX_START 7
#define XBOX_STICK_L 8
#define XBOX_STICK_R 9

#define XBOX_JOY_LSTICK_X 0
#define XBOX_JOY_LSTICK_Y 1
#define XBOX_JOY_TRIGGERS 2
#define XBOX_JOY_RIGHT_X 3
#define XBOX_JOY_RIGHT_Y 4

#define XBOX_HAT_DPAD 0

#define XBOX_JOY_NEUTRAL 8000

void controls_init();

//Feed an event into the controls system
void controls_handleEvent(SDL_Event* e);

//New frame, flip controls
void controls_step();

//Get control states
bool keyPressed(SDLKey key);
bool keyDown(SDLKey key);
bool keyReleased(SDLKey key);
bool keyUp(SDLKey key);
bool anyKeyPressed();

bool mousePressed(Uint8 button);
bool mouseDown(Uint8 button);
bool mouseReleased(Uint8 button);
bool mouseUp(Uint8 button);

bool joyButtonPressed(Uint8 button);
bool joyButtonDown(Uint8 button);
bool joyButtonReleased(Uint8 button);
bool joyButtonUp(Uint8 button);

bool joyHatContains(Uint8 hat, Uint8 dir);
bool joyHatMatches(Uint8 hat, Uint8 dir);

//Track mouse motion
int mouseX();
int mouseY();
int mouseMoveX();
int mouseMoveY();

//Take text input
char* getEntryBuffer();
void enableEntry();
void disableEntry();
void clearEntry();
bool entryHasNew();
void restrictEntryLen(int len);

#endif