// Fusion (INJECTION)
#include <Windows.h>

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

