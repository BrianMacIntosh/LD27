#include "stdafx.h"
#include "InvTexture.h"

InvTexture::InvTexture(const char* path)
{
	texId = NULL;

	SDL_Surface* texture = IMG_Load(path);
	assert(texture != NULL);
	
	_bindDataFromSurface(texture);

	SDL_FreeSurface(texture);

	resetDraw();
}


InvTexture::InvTexture(GLuint width, GLuint height, GLuint color)
{
	texId = NULL;

	//Make pixel buffer
	GLuint* tempbuffer = (GLuint*)calloc(width * height, sizeof(GLuint));

	//Fill it with input color
	for (GLuint c = 0; c < width * height; c++) tempbuffer[c] = color;

	_bindData(width, height, tempbuffer);
}


InvTexture::InvTexture(GLuint width, GLuint height, GLuint* data, GLenum format)
{
	texId = NULL;

	_bindData(width, height, data, format);
}


InvTexture::InvTexture(SDL_Surface* surface)
{
	texId = NULL;

	_bindDataFromSurface(surface);
}


InvTexture::~InvTexture()
{
	_free();
}


GLuint InvTexture::GetTextureId() const
{
	return *texId;
}


void InvTexture::setColor(InvColor c)
{
	color = c;
	color.glSet();
}

void InvTexture::setScale(float s)
{
	scale = s;
}

void InvTexture::setFlipX(bool flip)
{
	flipx = flip;
}

void InvTexture::setFlipY(bool flip)
{
	flipy = flip;
}

void InvTexture::startDraw()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, *texId);
}

void InvTexture::resetDraw()
{
	scale = 1;
	flipx = false;
	flipy = false;

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, NULL);
}


void InvTexture::draw(InvVectorf3 pos, InvVectorf2 origin, GLfloat angle)
{
	startDraw();

	glScalef(1 * (flipx ? -1 : 1), 1 * (flipy ? -1 : 1), 1);
	glTranslatef(pos.x, pos.y, 1);
	glRotatef(angle, 0, 0, -1);
	glTranslatef(-origin.x, -origin.y, 0);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(0, 0, pos.z);
	glTexCoord2f(1, 0); glVertex3f(width * scale, 0, pos.z);
	glTexCoord2f(1, 1); glVertex3f(width * scale, height * scale, pos.z);
	glTexCoord2f(0, 1); glVertex3f(0, height * scale, pos.z);
	glEnd();

	resetDraw();
}

void InvTexture::draw(InvRectangle rect, InvVectorf2 origin, GLfloat angle)
{
	startDraw();

	glScalef(1 * (flipx ? -1 : 1), 1 * (flipy ? -1 : 1), 1);
	glTranslatef(rect.left, rect.top, 1);
	glRotatef(angle, 0, 0, -1);
	glTranslatef(-origin.x, -origin.y, 0);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
	glTexCoord2f(1, 0); glVertex3f(rect.width() * scale, 0, 0);
	glTexCoord2f(1, 1); glVertex3f(rect.width() * scale, rect.height() * scale, 0);
	glTexCoord2f(0, 1); glVertex3f(0, rect.height() * scale, 0);
	glEnd();

	resetDraw();
}

void InvTexture::draw(InvVectorf3 tl, InvVectorf3 tr, InvVectorf3 br, InvVectorf3 bl)
{
	startDraw();

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(tl.x, tl.y, tl.z);
	glTexCoord2f(1, 0); glVertex3f(tr.x, tr.y, tr.z);
	glTexCoord2f(1, 1); glVertex3f(br.x, br.y, br.z);
	glTexCoord2f(0, 1); glVertex3f(bl.x, bl.y, bl.z);
	glEnd();

	resetDraw();
}

void InvTexture::draw(InvVectorf3 tl, InvVectorf3 tr, InvVectorf3 br, InvVectorf3 bl, InvVectorf2 position, float angle)
{
	startDraw();

	glTranslatef(position.x, position.y, 0);
	glRotatef(angle, 0, 0, -1);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(tl.x, tl.y, tl.z);
	glTexCoord2f(1, 0); glVertex3f(tr.x, tr.y, tr.z);
	glTexCoord2f(1, 1); glVertex3f(br.x, br.y, br.z);
	glTexCoord2f(0, 1); glVertex3f(bl.x, bl.y, bl.z);
	glEnd();

	resetDraw();
}


void InvTexture::_free()
{
	if (texId != NULL)
    {
        glDeleteTextures( 1, texId );
        delete texId;
		texId = NULL;
    }
}


void InvTexture::_bindDataFromSurface(SDL_Surface* surface)
{
	//Determine image data format
	GLenum textureFormat;

	switch (surface->format->BytesPerPixel)
	{
	case 4:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			textureFormat = GL_BGRA;
		else
			textureFormat = GL_RGBA;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			textureFormat = GL_BGR;
		else
			textureFormat = GL_RGB;
		break;
	}

	//Port texture to GL
	_bindData(surface->w, surface->h, surface->pixels, textureFormat);
}


void InvTexture::_bindData(GLuint width, GLuint height, void* data, GLenum format)
{
	_free();

	texId = new GLuint();
	glGenTextures( 1, texId );
	glBindTexture( GL_TEXTURE_2D, *texId );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data );

	this->width = width;
	this->height = height;

	glBindTexture( GL_TEXTURE_2D, NULL );
}


InvColor::InvColor()
{
	r = g = b = a = 1;
}

InvColor::InvColor(float red, float green, float blue)
{
	r = red;
	g = green;
	b = blue;
	a = 1;
}

InvColor::InvColor(float red, float green, float blue, float alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}

void InvColor::glSet() const
{
	glColor4f(r, g, b, a);
}