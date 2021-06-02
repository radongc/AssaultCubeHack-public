#include "pch.h"
#include "Math.h"

#define PI 3.14159265f

Vector3 Math::CalcAngle(Vector3 src, Vector3 dst)
{
    Vector3 delta = Vector3(dst.x - src.x, dst.y - src.y, dst.z - src.z);
    float deltaLen = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);

    float pitch = (float)asin(delta.z / deltaLen) * (180 / PI);
    float yaw = (float)atan2(delta.y, delta.x) * (180 / PI) + 90.f; // assault cube 0 is mapped 90 degrees clockwise from atan2, so add 90 degrees to the result to get correct value

    /*if (yaw < 0.0f) // and, atan2's range is from -180 to 180 while AC's is 0-359, so correct yaw
    {
        //yaw += 360.f;
    }*/

    return Vector3(yaw, pitch, 0);
}

void Math::Normalize(Vector3& angle)
{
    /* below fixes the "bug" (logical error) where the bot will travel ~360 degrees to go from 359 to 1 */
    /* normalize angles - prevent aimbot from ever traveling more than 180 degrees to aim at a player (there is no reason to do this) */
    if (angle.x > 180.f) // x = yaw
    {
        angle.x -= 360.f;
    }

    if (angle.x < -180.f)
    {
        angle.x += 360.f;
    }
}
