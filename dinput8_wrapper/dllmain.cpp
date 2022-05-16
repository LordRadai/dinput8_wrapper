#include <windows.h>
#include <thread>
#include <iostream>
using namespace std;

HINSTANCE hinst_dll = 0;
std::thread begin_thread;

extern "C" UINT_PTR directinput_create_proc = 0;
extern "C" __declspec(dllexport) HRESULT __cdecl DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter);
static decltype(&DirectInput8Create) original_dinput8_create;

bool Begin(uint64_t qModuleHandle) {

    char dllpath[MAX_PATH];
    GetSystemDirectoryA(dllpath, MAX_PATH);
    strcat_s(dllpath, "\\dinput8.dll");
    hinst_dll = LoadLibraryA(dllpath);

    if (!hinst_dll) {
        MessageBoxA(NULL, "Failed to load original DLL", "Error", MB_ICONERROR);
        return false;
    };

    original_dinput8_create = (decltype(&DirectInput8Create))GetProcAddress(hinst_dll, "DirectInput8Create");
    if (!original_dinput8_create) {
        MessageBoxA(NULL, "Failed to load original DLL", "Error", MB_ICONERROR);
        return false;
    }

    // Do whatever here, entry point to the entire mod
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    printf_s("Working fine so far");
    MessageBoxA(NULL, "", "", MB_ICONERROR);


    return true;
};


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {

    switch (fdwReason) {
        case (DLL_PROCESS_ATTACH): {
            DisableThreadLibraryCalls(hinstDLL);
            begin_thread = std::thread(Begin, (uint64_t)hinstDLL);
            break;
        };
        case (DLL_PROCESS_DETACH): {
            begin_thread.detach();
            FreeLibrary(hinst_dll);
            break;
        };
    }
    return TRUE;
}

// Define original dll export and call the original function
extern "C" __declspec(dllexport) HRESULT __cdecl DirectInput8Create(
    HINSTANCE hinst,
    DWORD dwVersion,
    REFIID riidltf,
    LPVOID * ppvOut,
    LPUNKNOWN punkOuter)
{
    return original_dinput8_create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}