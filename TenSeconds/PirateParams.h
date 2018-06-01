#pragma once
#ifndef _PIRATEPARAMS_H
#define _PIRATEPARAMS_H

//#pragma warning (disable : 4244 4305)

#include <math.h>

#define screenWidth 1024
#define screenHeight 768
#define screenDepth 32
#define fullscreen true

#define MapSizeX 64
#define MapSizeY 64
#define TileSize 32


#define PI 3.14159265f
inline float toDeg(float x) { return x * 180 / PI; };
inline int signum(float x) { return x < 0 ? -1 : (x > 0 ? 1 : 0); };
//inline float abs(float x) { return signum(x) * x; };
inline float min(float A, float B) { return A > B ? B : A; };
inline float max(float A, float B) { return A > B ? A : B; };
//inline int abs(int x) { return signum(x) * x; };
inline int min(int A, int B) { return A > B ? B : A; };
inline int max(int A, int B) { return A > B ? A : B; };
inline float clamp(float A, float min, float max)
{
	if (A > max)
		A = max;
	else if (A < min)
		A = min;
	return A;
}
inline int clamp(int A, int min, int max)
{
	if (A > max)
		A = max;
	else if (A < min)
		A = min;
	return A;
}

inline float wraprad(float x)
{
	/*float* bah = new float();
	float ret = modf(x / (PI * 2), bah);
	delete bah;
	return ret;*/
	if (x < 0) x += PI * 2;
	if (x > PI * 2) x -= PI * 2;
	return x;
};

#endif