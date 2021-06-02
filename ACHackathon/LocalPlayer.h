#pragma once
#include "Vector3.h"
#include "Entity.h"
#include "Math.h"
#include "Settings.h"

#include <iostream>


class LocalPlayer
{
private:
	Entity* instance;
	Settings* settings;

	Entity* target;

	float followTimer; // used for tracking how long aimbot has been closely following a target; the longer it follows, the more the smoothing factor decreases and the quicker the aimbot moves

public:
	LocalPlayer(Settings* hackSettings);

	Entity* Get();
	Entity* GetTarget();

	void SetTarget(Entity* target);
	void ClearTarget();

	void AimAt(Vector3 dst);
};