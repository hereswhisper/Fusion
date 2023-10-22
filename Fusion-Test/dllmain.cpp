// Fusion (Tester)
#include <Windows.h>
#include <cstdio>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        AllocConsole();
        FILE* Handle;
        freopen_s(&Handle, "CONOUT$", "w", stdout);

        printf("\n\n Working \n\n");
        break;
    }
    return TRUE;
}

