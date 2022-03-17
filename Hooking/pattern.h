#pragma once

VOID InitPattern(HMODULE hModule);

bool CompareMemory(const UINT64 uAddress, const PBYTE bMask, const char* sMask);

UINT64 FindPattern(const PBYTE bMask, const char* sMask);
