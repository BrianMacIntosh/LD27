#ifndef RASTERLINEITERATOR_H
#define RASTERLINEITERATOR_H

#include "InvSpacialData.h"

class RasterLineIterator
{
public:
	RasterLineIterator(InvVectorf2 start, InvVectorf2 end);

	void next();

	bool hasNext() const;

	InvVectorf2 current() const;

private:
	bool strline;

	double nextX;
	double nextY;
	double nextZ;

	double incX;
	double incY;
	double incZ;

	InvVectorf2 cur;
	InvVectorf2 dir;
	InvVectorf2 end;

	double m1;
	double b1;
};

#endif