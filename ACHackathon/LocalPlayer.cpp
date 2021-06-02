#include "pch.h"
#include "LocalPlayer.h"
#include "Offsets.h"

LocalPlayer::LocalPlayer(Settings* hackSettings)
{
	this->instance = *(Entity**)offsets::base::m_LocalBase; // localbase is a pointer to the player object, so we are creating a pointer that points to a pointer that points to the player object, and dereferencing that. that's why it's casted to Entity**, because there are 2 levels of dereferencing from our pov
    this->settings = hackSettings;

    this->target = nullptr;
}

Entity* LocalPlayer::Get() // get the Entity* instance of LocalPlayer (represents actual in-game entity)
{
	return this->instance;
}

Entity* LocalPlayer::GetTarget()
{
    return this->target;
}

void LocalPlayer::SetTarget(Entity* target)
{
    this->target = target;
}

void LocalPlayer::ClearTarget()
{
    this->target = nullptr;
}

void LocalPlayer::AimAt(Vector3 dst)
{
    Vector3 src = this->instance->vHeadPos;

    Vector3 angleTo = Math::CalcAngle(src, dst);

    Vector3 diffs = angleTo - Vector3(this->instance->vAngles.x, this->instance->vAngles.y, 0);

    if (!settings->RageMode) // smooth aim
    {
        Math::Normalize(diffs);

        if (fabs(diffs.x) > 0.1f) // print new diffs
        {
            std::cout << "[Current]" << std::endl;
            std::cout << "Pitch: " << this->instance->vAngles.y << ", Yaw: " << this->instance->vAngles.x << std::endl;
            std::cout << "[Desired]" << std::endl;
            std::cout << "Pitch: " << angleTo.y << ", Yaw: " << angleTo.x << std::endl << std::endl;
        }

        float adjustedSmoothing;
        
        if (fabs(diffs.x) <= 10.f && fabs(diffs.x) >= 1.f)
        {
            followTimer++;

            adjustedSmoothing = settings->AimSmoothing / (1 + followTimer / 50);
        }
        else
        {
            followTimer = followTimer != 0 ? 0 : followTimer; // followTimer is equal to 0 if followTimer is not currently 0, otherwise it is equal to whatever it was previously (ternary operator)

            adjustedSmoothing = settings->AimSmoothing - (settings->AimSmoothing / 4) + (diffs.x / 20);
        }

        std::cout << "Adjusted Smoothing: " << adjustedSmoothing << std::endl;

        this->instance->vAngles.x += diffs.x / adjustedSmoothing; // testing algorithms to make smoothing decrease as diffs decrease

        this->instance->vAngles.y += diffs.y / adjustedSmoothing;
    }
    else // instant aim
    {
        this->instance->vAngles.x = angleTo.x;
        this->instance->vAngles.y = angleTo.y;
    }

    if (settings->TriggerBot) // wip, it shouldn't work this way to be a good triggerbot. there should be a way to figure out if there is a player under my cursor, ingame function that does this. otherwise the triggerbot only works with the aimbot
    {
        if (target && fabs(diffs.x) <= settings->TriggerBotThreshold && fabs(diffs.y) <= settings->TriggerBotThreshold)
        {
            if (this->instance->bShooting == 0)
            {
               this->instance->bShooting = 1;
            }
        }
        else // TODO: bug: triggerbot does not stop shooting when the target has died. shaky and junk-code fix in OnDllAttach right above where g_currentTarget is set
        {
            this->instance->bShooting = 0;
        }
    }
}