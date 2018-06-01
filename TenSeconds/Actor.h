#pragma once

#include "InvSpacialData.h"

class InvTexture;

//Base class for a thing that walks around
class Actor
{
public:

	Actor();

	Actor(InvVectorf2 position);

	virtual void update(double elapsed);

	virtual void draw();

	InvVectorf2 getPosition() { return m_Position; };
	void setPosition(InvVectorf2 pos) { m_Position = pos; };

	bool isVisible() const;

	float m_Radius;

protected:

	InvVectorf2 m_Position;
	float posZ;

	InvVectorf2 m_Velocity;
	float velZ;

	bool m_FellOff;

	float m_Angle;

	//Had a collision last frame?
	bool collidedLast;
	InvVectorf2 collidedLastTile;

	//Leg drawing
	InvTexture* m_HipsTexture;
	InvTexture* m_LegLF;
	InvTexture* m_LegRF;
	float armOsc;
};