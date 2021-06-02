#pragma once

#include <cstdint>
#include <math.h>

class Vector3
{
public:
	float x;
	float y;
	float z;

	Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	float GetDistance(Vector3& other)
	{
		Vector3 delta = Vector3(other.x - this->x, other.y - this->y, other.z - this->z);

		return (float)sqrt(delta.x*delta.x + delta.y*delta.y + delta.z*delta.z);
	}

	Vector3& operator +(Vector3 other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
	}

	Vector3& operator -(Vector3 other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;

		return *this;
	}

	Vector3& operator +=(const Vector3 other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;

		return *this;
	}

	bool operator ==(Vector3 other)
	{
		if (this->x == other.x && this->y == other.y && this->z == other.z)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};