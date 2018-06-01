#include "stdafx.h"
#include "Crawler.h"
#include "InvTexture.h"
#include <math.h>
#include "EngineScreen.h"
#include "Player.h"
#include "Map.h"

static const float MaxMaxSpeed = 130;
static const float MinSpeed = 25;
static const float Acceleration = 280;

static const float SightRange = 240;
static const float AggroRange = 180;

Crawler::Crawler(InvVectorf2 position)
	: Enemy(position)
{
	m_Texture = new InvTexture("Resource/Enemy/crawler.png");
	m_ShadowTexture = new InvTexture("Resource/Enemy/crawler_shadow.png");

	m_HipsTexture = new InvTexture("Resource/Enemy/crawler_hips.png");

	m_LegRF = new InvTexture("Resource/Enemy/crawler_leg_rf.png");
	m_LegLF = new InvTexture("Resource/Enemy/crawler_leg_lf.png");

	growl1 = Mix_LoadWAV("Resource/Audio/growl1.wav");
	growl2 = Mix_LoadWAV("Resource/Audio/growl2.wav");
	Mix_VolumeChunk(growl1, 90);
	Mix_VolumeChunk(growl2, 90);

	m_Radius = 12;
	attackSpeed = 3;

	retargetTime = 0.01;
}

Crawler::~Crawler()
{
	delete m_Texture;
	delete m_ShadowTexture;
	delete m_HipsTexture;
	delete m_LegRF;
	delete m_LegLF;
	Mix_FreeChunk(growl1);
	Mix_FreeChunk(growl2);
}

void Crawler::update(double elapsed)
{
	//TODO: they fall off
	posZ = velZ = 0;

	InvVectorf2 steer = target - m_Position;

	angle = atan2f(steer.x, steer.y) * (180.0f / PI);

	//Player takes priority
	if ((getInstance()->m_Player->getPosition() - m_Position).lengthSquared() < SightRange * SightRange
		&& !getInstance()->m_Map->raycast(m_Position, getInstance()->m_Player->getPosition()))
	{
		if (!targetIsPlayer)
		{
			Mix_PlayChannel(0, (rand() % 2 == 1 ? growl1 : growl2), 0);
		}
		target = getInstance()->m_Player->getPosition();
		targetIsPlayer = true;
	}

	if (retargetTime > 0)
	{
		retargetTime -= elapsed;
		if (retargetTime < 0)
		{
			targetIsPlayer = false;
			target = InvVectorf2(
				rand() % MapSizeX, rand() % MapSizeY) * TileSize;
		}
	}
	else
	{
		m_Velocity += steer.normalize() * Acceleration * elapsed;
		if (m_Velocity.lengthSquared() > getMaxSpeed()*getMaxSpeed())
		{
			m_Velocity = m_Velocity.normalize() * getMaxSpeed();
		}
	}

	InvVectorf2 signToTarget = steer.getSigns();

	Enemy::update(elapsed);

	if (signToTarget != (target - m_Position) || collidedLast)
	{
		retargetTime = rand() % 7;
	}
}

float Crawler::getMaxSpeed()
{
	if (targetIsPlayer)
	{
		float dist = (m_Position - getInstance()->m_Player->getPosition()).length();
		return clamp((1-(dist / AggroRange)) * MaxMaxSpeed, MinSpeed, MaxMaxSpeed);
	}
	else
		return MinSpeed;
}

void Crawler::draw()
{
	Player* player = getInstance()->m_Player;

	//Draw dude
	if (isVisible())
	{
		m_Texture->draw(
			InvVectorf3(m_Position.x, m_Position.y, 10),
			InvVectorf2(m_Texture->getWidth(), m_Texture->getHeight()) / 2,
			angle);
		Enemy::draw();
	}

	float shadowLength = 80;

	//Draw shadow
	InvVectorf2 disp = m_Position - player->getPosition();
	InvVectorf2 disp1 = InvVectorf2(-disp.y, disp.x).normalize() * m_Radius;
	InvVectorf2 disp2 = disp1.normalize() * (1 + shadowLength / disp.length()) * m_Radius;
	InvVectorf2 pos2 = m_Position + disp.normalize() * (disp.length() + shadowLength);

	m_ShadowTexture->draw(
		InvVectorf3(
			pos2.x - disp1.x,
			pos2.y - disp1.y,
			5.0f),
		InvVectorf3(
			pos2.x + disp1.x,
			pos2.y + disp1.y,
			5.0f),
		InvVectorf3(
			m_Position.x + disp1.x,
			m_Position.y + disp1.y,
			5.0f),
		InvVectorf3(
			m_Position.x - disp1.x,
			m_Position.y - disp1.y,
			5.0f));
}