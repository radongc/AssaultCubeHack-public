#pragma once
#include "Vector3.h"

#include <cstdint>

class Math
{
public:
	static Vector3 CalcAngle(Vector3 src, Vector3 dst);
	static void Normalize(Vector3& angle);
};