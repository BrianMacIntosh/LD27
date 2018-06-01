#include "stdafx.h"
#include "InvSpacialData.h"
#include <cmath>
#include "PirateParams.h"


InvVectorf2::InvVectorf2()
{
	x = 0;
	y = 0;
}

InvVectorf2::InvVectorf2(float ix, float iy)
{
	x = ix;
	y = iy;
}

float InvVectorf2::length() const
{
	return std::sqrt(x*x + y*y);
}

float InvVectorf2::lengthSquared() const
{
	return x*x + y*y;
}

float InvVectorf2::dotProduct(InvVectorf2 other) const
{
	return other.x*x + other.y*y;
}

InvVectorf2 InvVectorf2::getSigns() const
{
	return InvVectorf2(signum(x), signum(y));
}

InvVectorf2 InvVectorf2::operator -(const InvVectorf2& other) const
{
	return InvVectorf2(x - other.x, y - other.y);
}

InvVectorf2 InvVectorf2::operator +(const InvVectorf2& other) const
{
	return InvVectorf2(x + other.x, y + other.y);
}

InvVectorf2& InvVectorf2::operator -=(const InvVectorf2& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

InvVectorf2& InvVectorf2::operator +=(const InvVectorf2& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

InvVectorf2 InvVectorf2::operator *(float other) const
{
	return InvVectorf2(other * x, other * y);
}

InvVectorf2 InvVectorf2::operator /(float other) const
{
	return InvVectorf2(x / other, y / other);
}

bool InvVectorf2::operator ==(const InvVectorf2& other) const
{
	return other.x == x && other.y == y;
}

bool InvVectorf2::operator !=(const InvVectorf2& other) const
{
	return other.x != x || other.y != y;
}

InvVectorf2 InvVectorf2::normalize()
{
	float tot = length();
	if (tot == 0)
		return InvVectorf2(0, 0);
	else
		return InvVectorf2(x / tot, y / tot);
}


InvVectorf3::InvVectorf3()
{
	x = 0;
	y = 0;
	z = 0;
}

InvVectorf3::InvVectorf3(float ix, float iy, float iz)
{
	x = ix;
	y = iy;
	z = iz;
}

float InvVectorf3::length() const
{
	return std::sqrt(x*x + y*y + z*z);
}

float InvVectorf3::lengthSquared() const
{
	return x*x + y*y + z*z;
}

InvVectorf3 InvVectorf3::operator -(const InvVectorf3& other) const
{
	return InvVectorf3(x - other.x, y - other.y, z - other.z);
}

InvVectorf3 InvVectorf3::operator +(const InvVectorf3& other) const
{
	return InvVectorf3(x + other.x, y + other.y, z + other.z);
}

InvVectorf3& InvVectorf3::operator -=(const InvVectorf3& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

InvVectorf3& InvVectorf3::operator +=(const InvVectorf3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

InvVectorf3 InvVectorf3::operator *(float other) const
{
	return InvVectorf3(other * x, other * y, other * z);
}

InvVectorf3 InvVectorf3::operator /(float other) const
{
	return InvVectorf3(x / other, y / other, z / other);
}

bool InvVectorf3::operator ==(const InvVectorf3& other) const
{
	return other.x == x && other.y == y && other.z == z;
}

bool InvVectorf3::operator !=(const InvVectorf3& other) const
{
	return other.x != x || other.y != y || other.z != z;
}

InvVectorf3 InvVectorf3::normalize()
{
	float tot = length();
	return InvVectorf3(x / tot, y / tot, z / tot);
}


float dotVectorf2(InvVectorf2 a, InvVectorf2 b)
{
	return a.x * b.x + a.y * b.y;
}

float dotVectorf3(InvVectorf3 a, InvVectorf3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}


InvRectangle::InvRectangle(float l, float r, float t, float b)
{
	left = l;
	right = r;
	top = t;
	bottom = b;
}

InvRectangle InvRectangle::Intersection(InvRectangle other) const
{
	InvRectangle intersect = InvRectangle(
		max(left,   other.left),
		min(right,  other.right),
		max(top,    other.top),
		min(bottom, other.bottom));
	if (intersect.right > intersect.left
		&& intersect.bottom > intersect.top)
		return intersect;
	else
		return InvRectangle(0, 0, 0, 0);
}

InvVectorf2 InvRectangle::center() const
{
	return InvVectorf2(left + right, top + bottom) / 2;
}

bool InvRectangle::Empty() const
{
	return width() == 0 && height() == 0;
}

float InvRectangle::width() const
{
	return right - left;
}

float InvRectangle::height() const
{
	return bottom - top;
}