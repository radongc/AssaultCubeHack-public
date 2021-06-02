#pragma once

#include <cstdint>

class Gun
{
public:
	char pad_0000[4]; //0x0000
	uint8_t gunId; //0x0004
	char pad_0005[23]; //0x0005
	uint16_t shotsTaken; //0x001C
	char pad_001E[41]; //0x001E
}; //Size: 0x0047
