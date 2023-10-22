// Fusion (INJECTION)
#include <Windows.h>
#include "Memory.h"
#include "Structs.h"
Unreal::FString(*GEV)();

namespace Helpers {
    float GetVersion() {
        uintptr_t GVAddr = Memory::FindPattern("40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8B C8 41 B8 04 ? ? ? 48 8B D3");
        if ((Memory::FindPattern("48 85 C9 74 2E 53 48 83 EC 20 48 8B D9 48 8B 0D ? ? ? ? 48 85 C9 75 0C") != 0 && Memory::FindPattern("48 8D 0D ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 48 8B D6", true, 3) != 0)) {
            //Scuffed AF but working
            if (Memory::FindPattern("48 8B 1D ? ? ? ? ? ? ? 10 4C 8D 4D ? 4C", true, 3) != 0) {
                return 1.8f;
            }
        }
        else {
            GEV = decltype(GEV)(GVAddr);
            std::string FNVer = GEV().ToString();

            FNVer.erase(0, FNVer.find_last_of("-", FNVer.length() - 1) + 1);

            return std::stof(FNVer);
        }
    }
}
BOOL StartDLL(const LPCWSTR dllPath) {
    HMODULE hModule = LoadLibrary(dllPath);
    if (hModule != NULL)
    {
        // Locate the DllMain function address
        FARPROC DllMainAddress = GetProcAddress(hModule, "DllMain");
        if (DllMainAddress != NULL)
        {
            // Manually call the DllMain function
            return ((BOOL(WINAPI*)(HMODULE, DWORD, LPVOID))DllMainAddress)(hModule, DLL_PROCESS_ATTACH, NULL);
        }
    }
    return FALSE;
}

BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        AllocConsole();
        FILE* Handle;
        freopen_s(&Handle, "CONOUT$", "w", stdout);

        std::cout << Helpers::GetVersion() << std::endl;

        LPCWSTR targetDLLPath = L"Fusion-Test.dll";
        if (StartDLL(targetDLLPath))
        {
            // The DllMain process was started successfully
        }
        else
        {
            // There was an issue starting the DllMain process
        }
        break;
    }
    return TRUE;
}

