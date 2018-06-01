#pragma once
#ifndef SPACIALDATA_H
#define SPACIALDATA_H

struct InvVectorf3;

struct InvVectorf2
{
public:
	float x;
	float y;

	InvVectorf2();
	InvVectorf2(float ix, float iy);

	float length() const;
	float lengthSquared() const;

	float dotProduct(InvVectorf2 other) const;

	InvVectorf2 getSigns() const;

	InvVectorf2 operator -(const InvVectorf2& other) const;
	InvVectorf2 operator +(const InvVectorf2& other) const;

	InvVectorf2& operator -=(const InvVectorf2& other);
	InvVectorf2& operator +=(const InvVectorf2& other);

	InvVectorf2 operator *(float other) const;
	InvVectorf2 operator /(float other) const;

	bool operator ==(const InvVectorf2& other) const;
	bool operator !=(const InvVectorf2& other) const;

	InvVectorf2 normalize();
};

struct InvVectorf3
{
public:
	float x;
	float y;
	float z;

	InvVectorf3();
	InvVectorf3(float ix, float iy, float iz);

	float length() const;
	float lengthSquared() const;

	InvVectorf3 operator -(const InvVectorf3& other) const;
	InvVectorf3 operator +(const InvVectorf3& other) const;

	InvVectorf3& operator -=(const InvVectorf3& other);
	InvVectorf3& operator +=(const InvVectorf3& other);

	InvVectorf3 operator *(float other) const;
	InvVectorf3 operator /(float other) const;

	bool operator ==(const InvVectorf3& other) const;
	bool operator !=(const InvVectorf3& other) const;

	InvVectorf3 normalize();
};

float dotVectorf2(InvVectorf2 a, InvVectorf2 b);

float dotVectorf3(InvVectorf3 a, InvVectorf3 b);

struct InvRectangle
{
public:
	float left;
	float right;
	float top;
	float bottom;

	InvRectangle(float l, float r, float t, float b);

	InvRectangle Intersection(InvRectangle other) const;

	InvVectorf2 center() const;

	bool Empty() const;

	float width() const;
	float height() const;
};

#endif