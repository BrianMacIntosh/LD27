#pragma once
#ifndef INVTEXTURE_H
#define INVTEXTURE_H

#include "SDL.h"
#include "SDL_opengl.h"
#include "InvSpacialData.h"


/*
Struct representing RBGA color data
*/

struct InvColor
{
public:
	InvColor();

	InvColor(float red, float green, float blue);

	InvColor(float red, float green, float blue, float alpha);

	void glSet() const;

	float r, g, b, a;
};


/*
This class represents a texture with pixel data in memory.
*/

class InvTexture
{
public:
	InvTexture(const char* path);

	InvTexture(GLuint width, GLuint height, GLuint color = 0xffffffff);

	InvTexture(GLuint width, GLuint height, GLuint* data, GLenum format = GL_RGBA);

	InvTexture(SDL_Surface* surface);

	~InvTexture();

	GLuint GetTextureId() const;

	//Set up draw
	void setColor(InvColor c);
	void setScale(float s);
	void setFlipX(bool flip);
	void setFlipY(bool flip);

	void draw(InvVectorf3 pos, InvVectorf2 origin, GLfloat angle = 0);

	void draw(InvRectangle rect, InvVectorf2 origin, GLfloat angle = 0);

	void draw(InvVectorf3 tl, InvVectorf3 tr, InvVectorf3 br, InvVectorf3 bl);
	void draw(InvVectorf3 tl, InvVectorf3 tr, InvVectorf3 br, InvVectorf3 bl, InvVectorf2 position, float angle);

	GLuint getWidth() { return width; };
	GLuint getHeight() { return height; };

	//OpenGL identifier for this texture
	GLuint* texId; //Ahhh why is this public?!!?

protected:
	//Draw setup data
	InvColor color;
	float scale;
	bool flipx;
	bool flipy;

	void startDraw();
	void resetDraw();

	GLuint width;
	GLuint height;

	void _free();

	void _bindDataFromSurface(SDL_Surface* surface);
	void _bindData(GLuint width, GLuint height, void* data, GLenum format = GL_RGBA);
};

#endif