#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <stdlib.h>

VOID GetProcessEntry32ByName(const char* szProcessName, DWORD th32ProcessID, LPPROCESSENTRY32 lpProcessEntry32);

HANDLE GetHandleByProcessId(DWORD processId);

LPVOID GetVirtualAllocAddr(HANDLE hProcess, size_t dwSize);

LPVOID GetVirtualAllocAddrAndWriteMemory(HANDLE hProcess, const char* filename);

LPTHREAD_START_ROUTINE GetLoadLibraryAddress();

BOOL CreateRemoteThread(HANDLE hProcess, LPTHREAD_START_ROUTINE loadLibraryAddr, LPVOID pDllFilenameAllocAddr);

BOOL InjectModule(DWORD processId, const char* filename);
