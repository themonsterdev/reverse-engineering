#include "stdafx.h"

// Template for the original function
typedef int (*SUM)(int x, int y);

static DWORD g_dwThreadId   = 0;
HANDLE g_hThread            = nullptr;

// Base
static uintptr_t g_addressBegin = 0;
static uintptr_t g_sizeOfCode   = 0;
static uintptr_t g_sizeOfImage  = 0;

// Module (Application)
static uintptr_t g_pAddressOfSum    = 0;
static SUM g_pSumTarget             = nullptr;
static SUM g_pSumOriginal           = nullptr;

int HookPayloadRetOriginal(int x, int y)
{
    printf("sum hacked\n");

    // Manipulate
    x += 500;
    y += 500;

    return g_pSumOriginal(x, y);
}

VOID InitPattern(HMODULE hModule)
{
    const IMAGE_DOS_HEADER* dosHeader   = reinterpret_cast<const IMAGE_DOS_HEADER*>(hModule);
    const IMAGE_NT_HEADERS* ntHeader    = reinterpret_cast<const IMAGE_NT_HEADERS64*>(
        reinterpret_cast<const uint8_t*>(dosHeader) + dosHeader->e_lfanew
    );

    g_addressBegin  = reinterpret_cast<uintptr_t>(hModule);
    g_sizeOfCode    = g_addressBegin + ntHeader->OptionalHeader.SizeOfCode;
    g_sizeOfImage   = ntHeader->OptionalHeader.SizeOfImage;
}

bool CompareMemory(const uint8_t* pData, const uint8_t* bMask, const char* sMask)
{
    for (; *sMask; ++sMask, ++pData, ++bMask)
    {
        if (*sMask == 'x' && *pData != *bMask)
        {
            return false;
        }
    }

    return *sMask == NULL;
}

uintptr_t FindPattern(const char* bMask, const char* sMask)
{
    uintptr_t address = 0;

    for (uint32_t offset = 0; offset < g_sizeOfImage; offset++)
    {
        address = g_addressBegin + offset;

        if (CompareMemory((uint8_t*)address, (uint8_t*)bMask, sMask))
        {
            return address;
        }
    }

    return 0;
}

DWORD WINAPI MainThread(HMODULE hModule)
{
    InitPattern(GetModuleHandle(0));
    printf("g_pAddressModule : 0x%p\n", (LPVOID)g_addressBegin);

    // Store the offset of sum(x, y) in Hook.exe here.
    g_pAddressOfSum = FindPattern("\x89\x54\x24\x10\x89\x4C\x24\x08\x55", "xxxxxxxxx");
    printf("g_pAddressOfSum: 0x%08x\n", g_pAddressOfSum);

    // Store the sum.
    g_pSumTarget = reinterpret_cast<SUM>(g_pAddressOfSum);
    printf("g_pSumTarget: %p\n", (LPVOID)g_pSumTarget);

    // install hook
    InstallHook(
        g_pSumTarget,
        HookPayloadRetOriginal,
        reinterpret_cast<LPVOID*>(&g_pSumOriginal)
    );

    while (true)
    {
        if (GetAsyncKeyState(VK_ESCAPE))
        {
            printf("Exit Hack\n");
            break;
        }

        Sleep(50);
    }

    UninstallHook(g_pSumTarget);

    CloseHandle(g_hThread);
    FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        g_hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, &g_dwThreadId);
        break;
    case DLL_PROCESS_DETACH:
        if (g_hThread != nullptr)
        {
            CloseHandle(g_hThread);
        }
        break;
    }
    return TRUE;
}
