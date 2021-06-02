#pragma once

class Settings
{
public:
	bool TargetTeammates = false;
	bool RageMode = false;
	bool FovMode = true; // aim only at players onscreen
	bool Recoil = false; // recoil hack
	bool TriggerBot = false; // trigger bot on/off

	float AimSmoothing = 4.f; // smoothing factor for aimbot
	float AimFov = 90.f; // maximum fov for aimbot
	float AimDistance = 1000.f; // maximum distance to aim at
	float TriggerBotThreshold = 2.5f; // how close to the target angle do we need to be to start shooting?
};