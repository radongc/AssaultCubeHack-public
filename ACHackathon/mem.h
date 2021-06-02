#pragma once
#include <Windows.h>
#include <vector>

namespace mem
{
	void Patch(BYTE* src, BYTE* dst, unsigned int size);
	void Nop(BYTE* dst, unsigned int size);

	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
}