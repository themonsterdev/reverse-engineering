#pragma once

LPVOID AllocatePageNearAddress(LPVOID pTarget);
VOID WriteAbsoluteJump64(LPVOID absoluteJumpMemory, LPVOID addrToJumpTo);
VOID WriteVirtualMemory(LPVOID pDest, LPVOID pSrc, SIZE_T ulSize);

VOID InstallHook(LPVOID pTarget, LPVOID pRetour, LPVOID* ppOriginal);
VOID UninstallHook(LPVOID pTarget);
