#include "stdafx.h"
#include "InvScreenManagement.h"
#include "InvControls.h"
#include "PirateParams.h"
#include "MenuScreen.h"
#include "Quit.h"
#include "TileManager.h"
#include "ItemManager.h"

//Game info
const char* windowBarName = "LD27 - 10 Seconds";

//Target frames per second, -1 is uncapped
float targetFrameRate = 60;

//Camera settings
float fov = 75;
float camNear = 1;
float camFar = 355;


bool gameRunning = true;
bool windowInactive = false;

//Screen drawing surface
SDL_Surface* surface;


/*
Handles SDL input and device events
*/
void handleEvent(SDL_Event* ev)
{
	if (ev->type == SDL_QUIT)
		//|| (ev->type == SDL_KEYDOWN && ev->key.keysym.sym == SDLK_ESCAPE))
	{
		quit();
	}
	else if (ev->type == SDL_ACTIVEEVENT)
	{
		if (ev->active.state & SDL_APPACTIVE)
		{
			if (ev->active.gain == 0)
				windowInactive = true;
			else
				windowInactive = false;
		}
	}
	else
	{
		screenHandleEvent(ev);
		controls_handleEvent(ev);
	}
}


void quit()
{
	gameRunning = false;
}


/*
Loads game structures
*/
void initialize()
{
	loadTiles();
	loadItems();

	controls_init();
	restrictEntryLen(16);

	addGameScreen(new MenuScreen());
}


/*
Handles game logic update each frame
*/
void update(double elapsedSeconds)
{
	updateScreens(elapsedSeconds);
}


/*
Render game to main surface
*/
void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawScreens();

	SDL_GL_SwapBuffers();
	//SDL_Flip(surface);
}


/*
Removes game structures
*/
void cleanup()
{
	_cleanupAllScreens();
}


/*
Entry point
*/
int main(int argc, char* argv[])
{
	srand(time(0));

	//Load SDL
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) return 1;
	surface = SDL_SetVideoMode(
		screenWidth, screenHeight, screenDepth,
		SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0));
	assert(surface != NULL);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	//Load SDLIMG
	IMG_Init(IMG_INIT_PNG);

	//Load TTF
	assert(TTF_Init() >= 0);

	//Load mixer
	Mix_Init(MIX_INIT_OGG);
	assert(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) >= 0);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.1);
	glEnable(GL_ALPHA_TEST);

	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0, 0, 0, 0);
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat aspectRatio = (screenWidth > screenHeight) ? float(screenWidth)/float(screenHeight) : float(screenHeight)/float(screenWidth);
    GLfloat fH = tan( float(fov / 360.0f * 3.14159f) ) * camNear;
    GLfloat fW = fH * aspectRatio;
    glFrustum(-fW, fW, fH, -fH, camNear, camFar);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//Config windows
	SDL_WM_SetCaption(windowBarName, "");

	//Load game
	initialize();

	//Initialize loop
	clock_t ticksPerFrame = CLOCKS_PER_SEC / targetFrameRate;
	clock_t lastUpdate = clock();
	clock_t lastDraw = 0;

	//Game loop
	while (gameRunning)
	{
		//Handle events
		controls_step();
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
			handleEvent(&ev);

		//Update logic
		clock_t thisUpdate = lastUpdate;
		lastUpdate = clock();
		if (!windowInactive)
			update((lastUpdate - thisUpdate) / (1.0 * CLOCKS_PER_SEC));

		//Draw to screen
		if (targetFrameRate == -1 || clock() - lastDraw >= ticksPerFrame)
		{
			lastDraw = clock();
			draw();
		}
	}

	//Free surfaces
	cleanup();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_Quit();

	return 0;
}