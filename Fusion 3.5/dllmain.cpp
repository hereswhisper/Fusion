// Fusion 3.5
#include <Windows.h>
#include "Helpers.h"
#include "SDK.hpp"
#include "Memory.h"
#include "MinHook.h"
#include "ProcessEvent.h"
using namespace SDK;
DWORD WINAPI Main(LPVOID) {
    GeneralHelpers::startConsole();
    // print debug information
    GeneralHelpers::PrintDebugInfo();

    UObject::GObjects = reinterpret_cast<decltype(UObject::GObjects)>(Memory::FindPattern("48 8B 05 ? ? ? ? 48 8D 1C C8 81 4B ? ? ? ? ? 49 63 76 30", true, 3));
    printf("\n UObject::GObjects\n");
    std::cout << "\n GObjects: " << __int64(UObject::GObjects) << std::endl;
    FMemory_Free = reinterpret_cast<decltype(FMemory_Free)>(Memory::FindPattern("48 85 C9 74 1D 4C 8B 05 ? ? ? ? 4D 85 C0 0F 84 ? ? ? ? 49 8B 00 48 8B D1 49 8B C8 48 FF 60 20 C3"));
    printf("\n FMemory_Free\n");
    std::cout << "\n FMemory_Free: " << __int64(FMemory_Free) << std::endl;
    FMemory_Malloc = reinterpret_cast<decltype(FMemory_Malloc)>(Memory::FindPattern("4C 8B C9 48 8B 0D ? ? ? ? 48 85 C9 75 08 49 8B C9 E9 ? ? ? ?"));
    printf("\n FMemory_Malloc\n");
    std::cout << "\n FMemory_Malloc: " << __int64(FMemory_Malloc) << std::endl;
    FMemory_Realloc = reinterpret_cast<decltype(FMemory_Realloc)>(Memory::FindPattern("4C 8B D1 48 8B 0D ? ? ? ? 48 85 C9 75 08 49 8B CA E9 ? ? ? ? 48 8B 01 45 8B C8 4C 8B C2 49 8B D2 48 FF 60 18"));
    printf("\n FMemory_Realloc\n");
    std::cout << "\n FMemory_Realloc: " << __int64(FMemory_Realloc) << std::endl;
    FNameToString = reinterpret_cast<decltype(FNameToString)>(Memory::FindPattern("48 89 5C 24 ? 57 48 83 EC 40 83 79 04 00 48 8B DA 48 8B F9"));
    printf("\n FNameToString\n");
    std::cout << "\n FNameToString: " << __int64(FNameToString) << std::endl;
    ProcessEvent = reinterpret_cast<decltype(ProcessEvent)>(GameHelpers::GetEngine()->Vtable[0x40]);
    printf("\n ProcessEvent\n");
    std::cout << "\n ProcessEvent: " << __int64(ProcessEvent) << std::endl;

    std::cout << "\n GObjects: " << __int64(UObject::GObjects) << std::endl;

    MH_Initialize(); // Initialize Minhook

    MH_CreateHook((void*)GameHelpers::GetEngine()->Vtable[0x40], ProcessEventHook, (void**)&ProcessEvent);
    MH_EnableHook(GameHelpers::GetEngine()->Vtable[0x40]);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Main, 0, 0, 0);
        break;
    }
    return TRUE;
}

