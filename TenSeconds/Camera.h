#pragma once

#include "InvSpacialData.h"

class Actor;

class Camera
{
public:

	Camera(Actor* focus);

	void update(double elapsed);

	void setView();

	InvRectangle getBounds();

	void setZeroMatrix() const;

	InvVectorf2 getPosition() const { return m_Position; };
	float getHeight() const { return 255; };

	float distout;
	InvVectorf2 centeroff;

private:

	Actor* m_Focus;

	InvVectorf2 m_Position;
};