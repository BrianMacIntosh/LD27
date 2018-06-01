#include "stdafx.h"
#include "Actor.h"
#include "EngineScreen.h"
#include "Map.h"
#include "PirateParams.h"
#include "InvTexture.h"

static const float ArmPeriod = 0.8f;

Actor::Actor()
{
	m_FellOff = false;
	velZ = posZ = 0;
}

Actor::Actor(InvVectorf2 position)
	: m_Position(position)
{
	m_FellOff = false;
	velZ = posZ = 0;
}

void Actor::update(double elapsed)
{
	if (m_FellOff)
	{
		velZ -= 60 * elapsed;
		posZ += velZ * elapsed;
	}

	InvVectorf2 lastPosition = m_Position;
	m_Position += m_Velocity * elapsed;

	if (!getInstance()->m_Map->coordValid(m_Position / TileSize))
	{
		m_FellOff = true;
	}

	if (!m_FellOff)
	{
		//Sliding collision
		collidedLast = getInstance()->m_Map->slide(m_Position, m_Velocity, m_Radius, collidedLastTile);
	}

	//Animate
	armOsc += (m_Velocity.length() / 75.0f) * elapsed * PI * 2 / ArmPeriod;
	armOsc = fmodf(armOsc, PI * 2);
}

void Actor::draw()
{
	float forwardangle = atan2(m_Velocity.x, m_Velocity.y) * (180 / PI);

	//Legs
	if (m_LegLF != NULL && m_LegRF != NULL)
	{
		InvTexture* leg = m_LegLF;
		float scale = sinf(armOsc);
		if (armOsc > PI)
		{
			leg = m_LegRF;
			scale *= -1;
		}

		float armwid = leg->getWidth() / 2;
		float armht = leg->getHeight() / 2;
		leg->draw(
			InvVectorf3(-armwid,  scale * armht, 2 + posZ),
			InvVectorf3( armwid,  scale * armht, 2 + posZ),
			InvVectorf3( armwid, -scale * armht, 2 + posZ),
			InvVectorf3(-armwid, -scale * armht, 2 + posZ),
			m_Position,
			forwardangle);
	}

	if (m_HipsTexture != NULL)
			m_HipsTexture->draw(
				InvVectorf3(m_Position.x, m_Position.y, 3 + posZ),
				InvVectorf2(m_HipsTexture->getWidth() / 2, m_HipsTexture->getHeight() / 2),
				forwardangle);
}

bool Actor::isVisible() const
{
	return !getInstance()->m_Map->getTileFogged(m_Position / TileSize);
}