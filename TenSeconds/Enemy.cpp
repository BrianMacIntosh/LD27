#include "stdafx.h"
#include "Enemy.h"
#include "EngineScreen.h"
#include "Player.h"
#include "Map.h"
#include <SDL_mixer.h>

static const float ConditionDamage = 0.5f;

static bool init = false;
static Mix_Chunk* damagesound;
static Mix_Chunk* walldamagesound;


Enemy::Enemy(InvVectorf2 position)
	:Actor(position)
{
	attackCooldown = 0;

	if (!init)
	{
		init = true;
		damagesound = Mix_LoadWAV("Resource/Audio/damage2.wav");
		walldamagesound = Mix_LoadWAV("Resource/Audio/damage.wav");
		Mix_VolumeChunk(damagesound, 75);
		Mix_VolumeChunk(walldamagesound, 75);
	}
}

void Enemy::update(double elapsed)
{
	//Attack player
	if (attackCooldown <= 0)
	{
		if ((m_Position - getInstance()->m_Player->getPosition()).length() < 
			(m_Radius + getInstance()->m_Player->m_Radius))
		{
			attackCooldown = attackSpeed;
			getInstance()->m_Player->damage();
			Mix_PlayChannel(0, damagesound, 0);
		}
	}

	//Attack walls
	if (attackCooldown <= 0 && collidedLast && getInstance()->m_Map->coordValid(collidedLastTile)
		&& getTileBase(getInstance()->m_Map->m_Map[(int)collidedLastTile.x][(int)collidedLastTile.y].TileType).breakablility > 0)
	{
		attackCooldown = attackSpeed;
		getInstance()->m_Map->m_Map[(int)collidedLastTile.x][(int)collidedLastTile.y].condition -= ConditionDamage;

		float dist = (getInstance()->m_Player->getPosition() - m_Position).length();
		if (dist < 320)
		{
			Mix_VolumeChunk(walldamagesound, (1 - dist / 320.0f) * 100);
			Mix_PlayChannel(0, walldamagesound, 0);
		}
	}

	if (attackCooldown > 0)
	{
		attackCooldown -= elapsed;
	}

	Actor::update(elapsed);
}

void Enemy::draw()
{
	Actor::draw();
}