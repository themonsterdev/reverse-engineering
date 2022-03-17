#include "stdafx.h"

// Template for the original function
typedef int (*SUM)(int x, int y);

static DWORD g_dwThreadId   = 0;
HANDLE g_hThread            = nullptr;

// Module (Application)
static UINT64 g_pAddressOfSum   = 0;
static SUM g_pSumTarget         = nullptr;
static SUM g_pSumOriginal       = nullptr;

int HookPayloadRetOriginal(int x, int y)
{
    // Manipulate
    x += 500;
    y += 500;

    return g_pSumOriginal(x, y);
}

VOID RewriteSumPrint()
{
    UINT64 sumTextAddr = FindPattern((PBYTE)"\x73\x75\x6D\x20\x35\x20\x2B\x20\x35\x20\x3D\x20\x25\x64\x0A", "xxxxxxxxxxxxxxx");
    WriteVirtualMemory((LPVOID)sumTextAddr, (LPVOID)"sum hacked %d\n", strlen("sum hacked %d\n") + 1);
}

VOID InitHooking()
{
    // Store the offset of sum(x, y) in Hook.exe here.
    g_pAddressOfSum = FindPattern((PBYTE)"\x89\x54\x24\x10\x89\x4C\x24\x08\x55", "xxxxxxxxx");
    printf("g_pAddressOfSum: %" PRIu64 " \n", g_pAddressOfSum);

    // Store the sum.
    g_pSumTarget = reinterpret_cast<SUM>(g_pAddressOfSum);
    printf("g_pSumTarget: %p\n", g_pSumTarget);

    // install hook
    InstallHook(
        g_pSumTarget,
        HookPayloadRetOriginal,
        reinterpret_cast<LPVOID*>(&g_pSumOriginal)
    );
}

DWORD WINAPI MainThread(HMODULE hModule)
{
    InitPattern(GetModuleHandle(0));
    RewriteSumPrint();
    InitHooking();

    while (true)
    {
        if (GetAsyncKeyState(VK_ESCAPE))
        {
            printf("Exit Hack\n");
            break;
        }

        Sleep(50);
    }

    FreeLibrary(hModule);
    return 0;
}

VOID ProcessAttach(HMODULE hModule)
{
    DisableThreadLibraryCalls(hModule);
    g_hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, &g_dwThreadId);
}

VOID ProcessDetach()
{
    UninstallHook(g_pSumTarget);

    if (g_hThread != nullptr)
    {
        CloseHandle(g_hThread);
    }

    ExitThread(0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ProcessAttach(hModule);
        break;
    case DLL_PROCESS_DETACH:
        ProcessDetach();
        break;
    }
    return TRUE;
}
