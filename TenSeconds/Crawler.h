#pragma once

#include "Enemy.h"
#include <SDL_mixer.h>

class InvTexture;

class Crawler : public Enemy
{
public:

	Crawler(InvVectorf2 position);
	~Crawler();

	virtual void update(double elapsed) override;

	virtual void draw() override;

	float getMaxSpeed();

private:

	InvTexture* m_Texture;
	InvTexture* m_ShadowTexture;

	Mix_Chunk* growl1;
	Mix_Chunk* growl2;

	InvVectorf2 target;
	bool targetIsPlayer;

	float retargetTime;

	float angle;
};