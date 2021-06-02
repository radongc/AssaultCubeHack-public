#pragma once
namespace offsets
{
	namespace base
	{
		constexpr int m_ModuleBase = 0x400000;
		constexpr int m_LocalBase = 0x50F4F4;
		constexpr int m_EntityBase = 0x50F4F8;

		constexpr int m_NumOfPlayers = 0x50F500; // int
	}

	namespace functions
	{
		constexpr int m_NewName = 0x0423B40; // void(*)(const char*)
		constexpr int m_Conoutf = 0x046B060; // void(*)(const char*), echoes the parameter message to the game console/chat
		constexpr int m_Hudoutf = 0x04090F0; // void(*)(const char*), echoes to the hud
		constexpr int m_IsVisible = 0x048ABD0; // uint8_t[bool](vec1, vec2).

		constexpr int m_PrintPosIngame = 0x0423650; // void(*)(null) [doesn't work]
		constexpr int m_AddMsg = 0x0423650; // void(*)(int, const char*) [doesn't work]
	}
}