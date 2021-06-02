#pragma once

#include "Vector3.h"

#include <cstdint>

enum class PlayerState : int8_t
{
	ALIVE = 0,
	DEAD = 1,
	GHOST = 5
};

/* // reclass generated class; char pad_xxx[y] is used to pad memory so offsets are not needed 
aside from the localplayer, entitylist addresses etc. a pointer to a localplayer will be created
with the value of the main localplayer pointer in memory casted to a DWORD ptr and then dereferenced,
so, with all of the data types of Entity variables matching the ones in game memory (and as a consequence, 
the spaces in between each var will be correct), the address being pointed to by this class will equal the localplayer
address in the game, and the vTable will be located at it's ingame offset, the vPos will be located at
its ingame offset etc. meaning aside from creating an entity ptr called localPlayer and assigning it
the value of the dereferenced LocalPlayer game pointer, nothing else has to be done here and it is ready 
to be used. ReClass is awesome!!!
*/
class Entity 
{
public:
	int32_t vTable; //0x0000 vTable = 0x4E4A98 is player, vTable = 0x4E4AC0 is bot
	Vector3 vHeadPos; //0x0004
	char pad_0010[36]; //0x0010
	Vector3 vPos; //0x0034
	Vector3 vAngles; //0x0040
	char pad_004C[8]; //0x004C
	float upwardSpeed; //0x0054
	char pad_0058[4]; //0x0058
	float height; //0x005C anything below 4.5 is crouch
	char pad_0060[32]; //0x0060
	int32_t speed; //0x0080
	char pad_0084[108]; //0x0084
	int32_t visibilityTimer; //0x00F0 seems to count the amount of time the player has been onscreen for localplayer
	char pad_00F4[4]; //0x00F4
	int32_t health; //0x00F8
	int32_t armor; //0x00FC
	char pad_0100[12]; //0x0100
	int32_t bAkimbo; //0x010C
	char pad_0110[4]; //0x0110
	int32_t pistolAmmoRes; //0x0114
	int32_t carbineAmmoRes; //0x0118
	int32_t shotgunAmmoRes; //0x011C
	int32_t smgAmmoRes; //0x0120
	int32_t sniperAmmoRes; //0x0124
	int32_t assaultRifleAmmoRes; //0x0128
	char pad_012C[16]; //0x012C
	int32_t pistolAmmoMag; //0x013C
	int32_t carbineAmmoMag; //0x0140
	int32_t shotgunAmmoMag; //0x0144
	int32_t smgAmmoMag; //0x0148
	int32_t sniperAmmoMag; //0x014C
	int32_t assaultRifleAmmoMag; //0x0150
	char pad_0154[4]; //0x0154
	int32_t grenadeAmmo; //0x0158
	int32_t akimboAmmo; //0x015C
	char pad_0160[156]; //0x0160
	int32_t kills; //0x01FC
	char pad_0200[4]; //0x0200
	int32_t deaths; //0x0204
	char pad_0208[28]; //0x0208
	uint8_t bShooting; //0x0224 set to 1 to shoot
	char name[16]; //0x0225
	char pad_0235[247]; //0x0235
	int16_t team; //0x032C
	char pad_032E[10]; //0x032E
	PlayerState state; //0x0338
	char pad_0339[59]; //0x0339
	class Gun* curWep; //0x0374
	char pad_0378[144]; //0x0378
	Vector3 vOnScreenPos; //0x0408 useful for checking if a ply is onscreen
	char pad_0414[416]; //0x0414
}; //Size: 0x05B4

struct EntList
{
	Entity* ents[31];
};