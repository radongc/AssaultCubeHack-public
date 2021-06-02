// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Offsets.h"
#include "Entity.h"
#include "mem.h"
#include "LocalPlayer.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

/* TODO: Finish conversion to OOP: for now, dllmain.cpp will contain hack logic but later this should be contained within its own class.
 anything not directly related to the flow of the hack itself should be self contained somewhere else. also, classes should have more self-
 contained functionality and act less like wrappers/namespaces. */

// TODO: also see if nop'ing out the cooldown ms for smg actually works for multiplayer. AOB scan bytes: 89 0A 8B 76 14

Settings g_hackSettings; // global hack settings object
LocalPlayer g_localPlayer = LocalPlayer(&g_hackSettings);

EntList* g_entList = *(EntList**)offsets::base::m_EntityBase; // entlist ptr

int* numOfPlayers = (int*)(offsets::base::m_NumOfPlayers);

void(*Hudout)(const char*) = (void(*)(const char*))offsets::functions::m_Hudoutf; // Hudout function; prints a message to the console & the hud
void(*Conout)(const char*) = (void(*)(const char*))offsets::functions::m_Conoutf; // Conout function; prints a message to the console
void(*NewName)(const char*) = (void(*)(const char*))offsets::functions::m_NewName; // NewName function; changes localplayer's name

struct traceresult_s // used for IsVisible
{
    Vector3 end;
    bool collided;
};

bool IsInFOV(Entity* ent) // target loss bug when angles go down from 0 to 359 was fixed with angle normalization.
{
    Vector3 origin = g_localPlayer.Get()->vPos;
    Vector3 dest = ent->vPos;

    Vector3 angleTo = Math::CalcAngle(origin, dest);

    Vector3 diff = angleTo - g_localPlayer.Get()->vAngles;

    Math::Normalize(diff); // makes sure diffs are not being calculated as being more than 180 degrees, which causes bugs. ex: if the diff is 180+, because it is going to 20 from 350 or to 350 from 20, fix this

    if (diff.x > (g_hackSettings.AimFov / 2) || diff.x < -(g_hackSettings.AimFov / 2) || diff.y > (g_hackSettings.AimFov / 2) || diff.y < -(g_hackSettings.AimFov / 2)) // checking if the target is within the fov on either side (side 1x = 45 deg. side 1y = 45 deg etc if fov is 90)
    {
        return false; 
    }
    else
    {
        return true;
    }
}

// credit to rake on GH
bool IsVisible(Entity* ent)
{
    DWORD traceline = 0x048A310; // function address
    traceresult_s traceresult; // struct used in function parameters & necessary to call func
    traceresult.collided = false;
    Vector3 from = g_localPlayer.Get()->vHeadPos; // origin
    Vector3 to = ent->vHeadPos; // destination

    //inline asm, calling the function by pushing all of its parameters and then calling
    // rake's reasoning for using inline asm here was that they are using their own calling convention (which is unknown) and so it cannot be called by creating a function pointer in C++ (because we don't know the calling convention)
    __asm
    {
        push 0; bSkipTags
        push 0; bCheckPlayers
        push g_localPlayer
        push to.z
        push to.y
        push to.x
        push from.z
        push from.y
        push from.x
        lea eax, [traceresult]
        call traceline;
        add esp, 36
    }
    return !traceresult.collided; // whether or not the traceline between the 2 positions collided with anything or not is stored in the traceresult (it is a reference)
}

bool IsEntValid(Entity* ent)
{
    if (ent)
    {
        if (ent->vTable == 0x4E4A98 || ent->vTable == 0x4E4AC0) // player & bot, respectively
        {
            return true;
        }
    }

    return false;
}

Entity* GetBestPlayer()
{
    float closestDist = g_hackSettings.AimDistance;
    int closestIndex = -1;

    for (int i = 0; i < (*numOfPlayers); i++)
    {
        Entity* curPlayer = g_entList->ents[i];
        
        if (!IsEntValid(curPlayer)) // skip to next iteration if ent isnt valid
        {
            continue;
        }

        if (!IsVisible(curPlayer))
        {
            continue;
        }

        if (!IsInFOV(curPlayer) && g_hackSettings.FovMode) // skip if not within field of view
        {
            continue;
        }

        if (curPlayer->team == g_localPlayer.Get()->team && !g_hackSettings.TargetTeammates || curPlayer->state != PlayerState::ALIVE) // skip to next iteration if we cannot or should not be shooting at this player
        {
            continue;
        }

        Vector3 origin = g_localPlayer.Get()->vPos;
        float currentDist = origin.GetDistance(curPlayer->vHeadPos);

        if (currentDist < closestDist)
        {
            closestDist = currentDist;
            closestIndex = i;
        }
    }

    if (closestIndex == -1)
    {
        return NULL;
    }
    else
    {
        std::cout << "---[TARGET FOUND]---" << std::endl;
        std::cout << "NAME:  " << g_entList->ents[closestIndex]->name << std::endl << std::endl;
        return g_entList->ents[closestIndex];
    }
}

bool IsGoodTarget(Entity* target)
{
    if (!IsEntValid(target))
    {
        return false;
    }

    if (!IsVisible(target))
    {
        return false;
    }

    if (!IsInFOV(target) && g_hackSettings.FovMode)
    {
        return false;
    }

    if (target->state != PlayerState::ALIVE || target->health < 1 || target->health > 100) // make sure player is alive, check state, check health (both if it is under 1, and above 100 in the case that it gets set to a weird value after death)
    {
        return false;
    }

    if (target->team == g_localPlayer.Get()->team && !g_hackSettings.TargetTeammates)
    {
        return false;
    }

    return true;
}

DWORD WINAPI OnDllAttach(LPVOID param) // cast bools as uint8_t*, or byte pointers
{
#ifdef _DEBUG
    AllocConsole();
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    SetConsoleTitleA("ACHackathon - Debug");
#endif // DEBUG

    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

    bool bSpeedhacking = false;

    bool bAimbotEnabled = false;
    bool bSpeedhackEnabled = false;

    (*Hudout)("\f2Hack loaded");
    (*Conout)("Controls:");
    (*Conout)("Shift - Speedhack");
    (*Conout)("F - Toggle Aimbot");
    (*Conout)("F1 - Toggle Speedhack On/Off");
    (*Conout)("F2 - Toggle Recoil Hack");
    (*Conout)("F3 - Toggle Target Teammates");
    (*Conout)("F4 - Toggle Instant Lock Mode");
    (*Conout)("F5 - Toggle FOV Mode");
    (*Conout)("F6 - Toggle Trigger Bot");
    (*Conout)("(+) - Increase Aim Smoothing");
    (*Conout)("(-) - Decrease Aim Smoothing");

    while (true)
    {
        if (GetAsyncKeyState(VK_LSHIFT) & 1 && bSpeedhackEnabled) // speedhack
        {
            bSpeedhacking = true;
        }
        else
        {
            bSpeedhacking = false;
        }

        if (GetAsyncKeyState(0x46) & 1) // 'F' key
        {
            bAimbotEnabled = !bAimbotEnabled;

            switch (bAimbotEnabled)
            {
            case TRUE:
                (*Hudout)("\f3Aimbot enabled");
                break;
            case FALSE:
                (*Hudout)("\f3Aimbot disabled");
                if (g_localPlayer.GetTarget() && g_hackSettings.TriggerBot) // another hacky fix for triggerbot not stopping shooting -- this will only get called when the aimbot is disabled once
                {
                    g_localPlayer.Get()->bShooting = 0;
                }

                if (g_localPlayer.GetTarget())
                {
                    g_localPlayer.ClearTarget();
                }
                break;
            }
        }

        if (GetAsyncKeyState(VK_F1) & 1)
        {
            bSpeedhackEnabled = !bSpeedhackEnabled;

            switch (bSpeedhackEnabled)
            {
            case TRUE:
                (*Hudout)("Speedhack enabled");
                break;
            case FALSE:
                (*Hudout)("Speedhack disabled");
                break;
            }
        }

        if (GetAsyncKeyState(VK_F2) & 1)
        {
            g_hackSettings.Recoil = !g_hackSettings.Recoil;

            switch (g_hackSettings.Recoil)
            {
            case TRUE:
                (*Hudout)("Recoil hack enabled");
                break;
            case FALSE:
                (*Hudout)("Recoil hack disabled");
            }

            if (g_hackSettings.Recoil)
            {
                mem::Nop((BYTE*)(moduleBase + 0x63786), 10);
            }
            else
            {
                mem::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10); // patch original bytes
            }
        }

        if (GetAsyncKeyState(VK_F3) & 1)
        {
            g_hackSettings.TargetTeammates = !g_hackSettings.TargetTeammates;

            switch (g_hackSettings.TargetTeammates)
            {
            case TRUE:
                (*Hudout)("Targetting teammates enabled");
                break;
            case FALSE:
                (*Hudout)("Targetting teammates disabled");
                break;
            }
        }

        if (GetAsyncKeyState(VK_F4) & 1)
        {
            g_hackSettings.RageMode = !g_hackSettings.RageMode;

            switch (g_hackSettings.RageMode)
            {
            case TRUE:
                (*Hudout)("Rage mode enabled");
                break;
            case FALSE:
                (*Hudout)("Rage mode disabled");
            }
        }

        if (GetAsyncKeyState(VK_F5) & 1)
        {
            g_hackSettings.FovMode = !g_hackSettings.FovMode;

            switch (g_hackSettings.FovMode)
            {
            case TRUE:
                (*Hudout)("Fov mode enabled");
                break;
            case FALSE:
                (*Hudout)("Fov mode disabled");
            }
        }

        if (GetAsyncKeyState(VK_F6) & 1)
        {
            g_hackSettings.TriggerBot = !g_hackSettings.TriggerBot;

            switch (g_hackSettings.TriggerBot)
            {
            case TRUE:
                (*Hudout)("Triggerbot enabled");
                break;
            case FALSE:
                (*Hudout)("Triggerbot disabled");
                break;
            }
        }

        if (GetAsyncKeyState(VK_OEM_MINUS) & 1)
        {
            g_hackSettings.AimSmoothing -= 0.5f;

            if (g_hackSettings.AimSmoothing < 1)
            {
                g_hackSettings.AimSmoothing = 1;
            }

            (*Conout)(("Aim Smoothing: " + std::to_string(g_hackSettings.AimSmoothing)).c_str());
        }

        if (GetAsyncKeyState(VK_OEM_PLUS) & 1)
        {
            g_hackSettings.AimSmoothing += 0.5f;

            if (g_hackSettings.AimSmoothing > 15)
            {
                g_hackSettings.AimSmoothing = 15;
            }

            (*Conout)(("Aim Smoothing: " + std::to_string(g_hackSettings.AimSmoothing)).c_str());
        }

        if (bSpeedhacking)
        {
            g_localPlayer.Get()->speed = 3;
        }

        if (bAimbotEnabled && g_localPlayer.Get()->state == PlayerState::ALIVE) // only aim if bot is enabled, & player is not dead
        {
            if (g_localPlayer.GetTarget() && IsGoodTarget(g_localPlayer.GetTarget())) // aim at current target if it isn't null and if it's a good target. fov is checked in IsGoodTarget if fovBot is on
            {
                g_localPlayer.AimAt(g_localPlayer.GetTarget()->vHeadPos);
            }
            else
            {
                if (g_localPlayer.GetTarget() && g_hackSettings.TriggerBot) // hacky fix for triggerbot bug where it doesn't stop shooting -- this only gets called right after the most recent target died because g_currentTarget hasn't been reassigned yet
                {
                    g_localPlayer.Get()->bShooting = 0;
                }

                std::cout << "********************" << std::endl;
                std::cout << "***FINDING TARGET***" << std::endl;
                std::cout << "********************" << std::endl;

                g_localPlayer.SetTarget(GetBestPlayer()); // find closest player
            }
        }

        // break from loop
        if (GetAsyncKeyState(VK_DELETE) & 1)
        {
            (*Hudout)("\f2Hack unloaded.");
            break;
        }

        Sleep(10);
    }

    /* RESET RECOIL HACK */
    if (g_hackSettings.Recoil)
    {
        mem::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10);
    }

    /* TURN OFF TRIGGERBOT */
    if (g_hackSettings.TriggerBot)
    {
        g_localPlayer.Get()->bShooting = 0;
    }

    // Free the DLL (unload hack)
    FreeLibraryAndExitThread((HMODULE)param, NULL);

    return NULL;
}

VOID WINAPI OnDllDetach()
{
#ifdef _DEBUG
    fclose((FILE*)stdin);
    fclose((FILE*)stdout);

    HWND hw_ConsoleHwnd = GetConsoleWindow();
    FreeConsole();
    PostMessageW(hw_ConsoleHwnd, WM_CLOSE, 0, 0);
#endif // _DEBUG

}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, OnDllAttach, hModule, 0, 0); // Added hModule to be passed to hackthread
        break;

    case DLL_PROCESS_DETACH:
        OnDllDetach();
        break;
    }
    return TRUE;
}