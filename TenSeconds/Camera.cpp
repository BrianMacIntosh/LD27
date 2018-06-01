#include "stdafx.h"
#include "Camera.h"
#include <SDL_opengl.h>
#include "PirateParams.h"
#include "Actor.h"
#include "EngineScreen.h"
#include "Map.h"

static const float EdgeOffset = 30;

Camera::Camera(Actor* focus)
{
	m_Focus = focus;
}

void Camera::update(double elapsed)
{
	m_Position = m_Focus->getPosition();

	//Move position away from map center
	InvVectorf2 center = InvVectorf2(MapSizeX, MapSizeY) * TileSize / 2;
	centeroff = m_Position - center;
	distout = min(1.0f, centeroff.length() / (float)center.length());
	distout = 1 - (1-distout)*(1-distout);

	m_Position += centeroff.normalize() * distout * EdgeOffset;
}

void Camera::setView()
{
	glTranslatef(-m_Position.x, -m_Position.y, -getHeight());
}

InvRectangle Camera::getBounds()
{
	return InvRectangle(m_Position.x, screenWidth + m_Position.x, screenHeight + m_Position.y, m_Position.y);
}

void Camera::setZeroMatrix() const
{
	glTranslatef(0, 0, -1);
}