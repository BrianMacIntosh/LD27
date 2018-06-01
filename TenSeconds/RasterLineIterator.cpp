#include "stdafx.h"
#include "RasterLineIterator.h"
#include <cmath>

RasterLineIterator::RasterLineIterator(InvVectorf2 start, InvVectorf2 end)
	:cur(start)
	,end(end)
{
	dir = (end - start).getSigns();

	incX = 1 / (double)abs(end.x-start.x);
	incY = 1 / (double)abs(end.y-start.y);
	//incZ = 1 / (double)abs(end.z-start.z);

	nextX = incX;
	nextY = incY;
	//nextZ = incZ;

	strline = abs(dir.x)+abs(dir.y) == 1;

	next();
}

void RasterLineIterator::next()
{
	if (!hasNext()) return;

	if (dir.y == 0)
	{
		cur.x += dir.x;
	}
	else if (dir.x == 0)
	{
		cur.y += dir.y;
	}
	else
	{
		//Go to next axis cross
		if (nextX < nextY)
		{
			nextX += incX;
			cur.x += dir.x;
		}
		else
		{
			nextY += incY;
			cur.y += dir.y;
		}
	}
}

bool RasterLineIterator::hasNext() const
{
	if (strline)
		return abs(cur.x-end.x)+abs(cur.y-end.y) > 0;
	else
		return abs(cur.x-end.x)+abs(cur.y-end.y) > 1;
}

InvVectorf2 RasterLineIterator::current() const
{
	return cur;
}