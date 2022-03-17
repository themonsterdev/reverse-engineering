#include "stdafx.h"

// Base
static UINT64 g_addressBegin    = 0;
static UINT64 g_sizeOfCode      = 0;
static UINT64 g_sizeOfImage     = 0;

VOID InitPattern(HMODULE hModule)
{
    const IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(hModule);
    const IMAGE_NT_HEADERS* ntHeader = reinterpret_cast<const IMAGE_NT_HEADERS64*>(
        reinterpret_cast<const uint8_t*>(dosHeader) + dosHeader->e_lfanew
    );

    g_addressBegin  = reinterpret_cast<UINT64>(hModule);
    g_sizeOfCode    = g_addressBegin + ntHeader->OptionalHeader.SizeOfCode;
    g_sizeOfImage   = ntHeader->OptionalHeader.SizeOfImage;

    printf("g_addressBegin: %" PRIu64 " \n", g_addressBegin);
}

bool CompareMemory(const UINT64 uAddress, const PBYTE bMask, const char* sMask)
{
    auto pData = reinterpret_cast<PUINT8>(uAddress);
    auto wsMask = reinterpret_cast<PUINT8>(bMask);

    for (; *sMask; ++sMask, ++pData, ++wsMask)
    {
        if (*sMask == 'x' && *pData != *wsMask)
        {
            return false;
        }
    }

    return *sMask == NULL;
}

UINT64 FindPattern(const PBYTE bMask, const char* sMask)
{
    UINT64 uAddress = 0;

    for (UINT32 offset = 0; offset < g_sizeOfImage; offset++)
    {
        uAddress = g_addressBegin + offset;

        if (CompareMemory(uAddress, bMask, sMask))
        {
            return uAddress;
        }
    }

    return 0;
}
