#include "injector.h"

VOID GetProcessEntry32ByName(const char* szProcessName, DWORD th32ProcessID, LPPROCESSENTRY32 lpProcessEntry32)
{
	// Take a snapshot of all processes in the system.
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, th32ProcessID);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		printf("[!]CreateToolhelp32Snapshot (of processes)\n");
	else
	{
		PROCESSENTRY32 pe32{ 0 };

		// Set the size of the structure before using it.
		pe32.dwSize = sizeof(PROCESSENTRY32);

		// Retrieve information about the first process
		if (Process32First(hProcessSnap, &pe32) == 0)
			printf("[!]Failed to gather information on system processes!\n");	// show cause of failure
		else
		{
			// Now walk the snapshot of processes
			do
			{
				if (strcmp(szProcessName, pe32.szExeFile) == 0)
				{
					memcpy(lpProcessEntry32, &pe32, pe32.dwSize);
					break;
				}
			} while (Process32Next(hProcessSnap, &pe32));
		}

		CloseHandle(hProcessSnap);
	}
}

HANDLE GetHandleByProcessId(DWORD processId)
{
	printf("[+]Opening process by process id (0x%08x).\n\n", processId);

	// Retrieving a handle to the target process.
	HANDLE hProcess = OpenProcess(
		PROCESS_ALL_ACCESS, // The access to the process object.
		FALSE,				// Processes do not inherit this handle.
		processId			// The identifier of the local process to be opened.
	);

	if (hProcess == nullptr)
	{
		printf("[!]OpenProcess failed with error (%d).\n", GetLastError());
		return FALSE;
	}

	printf("[+]Succesfully open handle (0x08%p).\n\n", hProcess);
	return hProcess;
}

LPVOID GetVirtualAllocAddr(HANDLE hProcess, size_t dwSize)
{
	// Allocating memory in the target process
	printf("[+]Reserve memory in the virtual address space of the target process.\n");
	LPVOID pDllFilenameAllocAddr = VirtualAllocEx(
		hProcess,					// The function allocates memory within the virtual address space of this process.
		nullptr,					// The pointer that specifies a desired starting address for the region of pages that you want to allocate.
		dwSize,						// The size of the region of memory to allocate, in bytes.
		MEM_COMMIT | MEM_RESERVE,	// The type of memory allocation.
		PAGE_READWRITE				// The memory protection for the region of pages to be allocated.
	);
	if (pDllFilenameAllocAddr == nullptr)
	{
		printf("[!]VirtualAllocEx failed with error (%d).\n", GetLastError());
		return nullptr;
	}
	printf("[+]Succesfully reserve memory in the virtual address space of the target process.\n\n");

	return pDllFilenameAllocAddr;
}

LPVOID GetVirtualAllocAddrAndWriteMemory(HANDLE hProcess, const char* filename)
{
	size_t dwFilepathSize = strlen(filename) + 1;

	LPVOID pDllFilenameAllocAddr = GetVirtualAllocAddr(hProcess, dwFilepathSize);
	if (pDllFilenameAllocAddr != nullptr)
	{
		printf("[+]Unprotect virtual memory for target process handle.\n");
		DWORD lpflOldProtect = 0; // A pointer to a variable that receives the previous access protection of the first page in the specified region of pages.
		BOOL success = VirtualProtectEx(
			hProcess,				// A handle to the process whose memory protection is to be changed.
			pDllFilenameAllocAddr,	// A pointer to the base address of the region of pages whose access protection attributes are to be changed.
			dwFilepathSize,			// The size of the region whose access protection attributes are changed, in bytes.
			PAGE_EXECUTE_READWRITE,	// The memory protection option.
			&lpflOldProtect
		);
		if (!success)
		{
			printf("[!]VirtualProtectEx failed with error (%d).\n", GetLastError());
			return nullptr;
		}
		printf("[+]Succesfully set virtual protect (0x%08x).\n", PAGE_EXECUTE_READWRITE);

		// Writing the dll path into that memory.
		printf("\t[+]Writes module filename to an area of memory in a target process.\n");
		SIZE_T lpNumberOfBytesWritten;
		success = WriteProcessMemory(
			hProcess,				// A handle to the process memory to be modified.
			pDllFilenameAllocAddr,	// A pointer to the base address in the specified process to which data is written.
			filename,				// A pointer to the buffer that contains data to be written in the address space of the specified process.
			dwFilepathSize,			// The number of bytes to be written to the specified process.
			&lpNumberOfBytesWritten	// A pointer to a variable that receives the number of bytes transferred into the specified process.
		);
		if (!success)
		{
			printf("\t[!]WriteProcessMemory failed with error (%d).\n", GetLastError());
			return nullptr;
		}
		else if (lpNumberOfBytesWritten != dwFilepathSize)
		{
			printf("\t[!]WriteProcessMemory failed not set total length.\n");
			return nullptr;
		}
		printf("\t[+]Succesfully write memory in handle process.\n");

		// Reset VirtualProtectEx
		printf("\t[+]Reset protect virtual memory for target process handle.\n");
		success = VirtualProtectEx(
			hProcess,				// A handle to the process whose memory protection is to be changed.
			pDllFilenameAllocAddr,	// A pointer to the base address of the region of pages whose access protection attributes are to be changed.
			dwFilepathSize,			// The size of the region whose access protection attributes are changed, in bytes.
			lpflOldProtect,			// [Reset] The memory protection option.
			&lpflOldProtect
		);
		if (!success)
		{
			printf("[!]VirtualProtectEx failed with error (%d).\n", GetLastError());
			return nullptr;
		}
		printf("[+]Succesfully reset virtual protect (0x%08x).\n\n", lpflOldProtect);

		return pDllFilenameAllocAddr;
	}

	return nullptr;
}

LPTHREAD_START_ROUTINE GetLoadLibraryAddress()
{
	// GetProcAddress
	HMODULE hModuleKernel32 = LoadLibraryA("kernel32");
	if (hModuleKernel32 == 0)
	{
		printf("[!]Failed to load module kernel32\n");
		return FALSE;
	}
	// Getting LoadLibraryA address (same across all processes) to start execution at it
	return (LPTHREAD_START_ROUTINE)GetProcAddress(hModuleKernel32, "LoadLibraryA");
}

BOOL CreateRemoteThread(HANDLE hProcess, LPTHREAD_START_ROUTINE loadLibraryAddr, LPVOID pDllFilenameAllocAddr)
{
	// Starting a remote execution thread at LoadLibraryA and passing the dll path as an argument.
	printf("[+]Creating remote thread in target Process.\n");
	DWORD lpThreadId;
	HANDLE hThread = CreateRemoteThread(
		hProcess,				// A handle to the process in which the thread is to be created.
		nullptr,				// A pointer to a SECURITY_ATTRIBUTES structure.
		0,						// The initial size of the stack, in bytes. 
		loadLibraryAddr,		// A pointer to the application-defined function of type LPTHREAD_START_ROUTINE to be executed by the thread and represents the starting address of the thread in the remote process.
		pDllFilenameAllocAddr,	// A pointer to a variable to be passed to the thread function.
		0,						// The flags that control the creation of the thread.
		&lpThreadId				// A pointer to a variable that receives the thread identifier.
	);

	if (hThread == nullptr)
	{
		printf("[!]CreateRemoteThread failed with error (%d).\n", GetLastError());
		CloseHandle(hProcess);
		return FALSE;
	}

	printf("[+]Succesfully Create Remote thread in target Process.\n");

	printf("[+]Wait thread is loaded module as terminate.\n\n");
	WaitForSingleObject(hThread, INFINITE); // Waiting for it to be finished
	CloseHandle(hThread);					// Freeing the injected thread handle

	return TRUE;
}

BOOL InjectModule(DWORD processId, const char* filename)
{
	BOOL success = FALSE;

	// Retrieving a handle to the target process.
	HANDLE hProcess = GetHandleByProcessId(processId);
	if (hProcess != nullptr)
	{
		LPVOID pDllFilenameAllocAddr = GetVirtualAllocAddrAndWriteMemory(hProcess, filename);
		if (pDllFilenameAllocAddr != nullptr)
		{
			LPTHREAD_START_ROUTINE loadLibraryAddr = (LPTHREAD_START_ROUTINE)GetLoadLibraryAddress();
			if (loadLibraryAddr != nullptr)
			{
				success = CreateRemoteThread(hProcess, loadLibraryAddr, pDllFilenameAllocAddr);
			}

			VirtualFreeEx(hProcess, pDllFilenameAllocAddr, 0, MEM_RELEASE);	// The memory allocated for the DLL filepath
			success = true;
		}

		CloseHandle(hProcess); // The handle for the target process
	}

	return success;
}
