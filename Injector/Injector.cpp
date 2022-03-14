#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <stdlib.h>

static char* GetAbsoluteFilename(char* filepath, const char* filename)
{
	size_t slash = -1;
	for (size_t i = 0; i < strlen(filepath); i++)
	{
		if (filepath[i] == '/' || filepath[i] == '\\')
		{
			slash = i;
		}
	}

	if (slash != -1)
	{
		filepath[slash + 1] = '\0';

		static char buffer[MAX_PATH];
		strcpy_s(buffer, filepath);
		strcat_s(buffer, filename);
		return buffer;
	}

	return nullptr;
}

VOID GetProcessEntry32(const char* processName, DWORD th32ProcessID, LPPROCESSENTRY32 lpProcessEntry32)
{
	// Take a snapshot of all processes in the system.
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, th32ProcessID);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		printf("CreateToolhelp32Snapshot (of processes)\n");
	else
	{
		PROCESSENTRY32 pe32 { 0 };

		// Set the size of the structure before using it.
		pe32.dwSize = sizeof(PROCESSENTRY32);

		// Retrieve information about the first process
		if (Process32First(hProcessSnap, &pe32) == 0)
			printf("Failed to gather information on system processes!\n");	// show cause of failure
		else
		{
			// Now walk the snapshot of processes
			do
			{
				if (strcmp(processName, pe32.szExeFile) == 0)
				{
					memcpy(lpProcessEntry32, &pe32, pe32.dwSize);
					break;
				}
			}
			while (Process32Next(hProcessSnap, &pe32));
		}

		CloseHandle(hProcessSnap);
	}
}

BOOL InjectModule(DWORD processId, const char* filepath)
{
	size_t dll_size = strlen(filepath) + 1;

	// OpenProcess
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (hProc == NULL)
	{
		printf("Fail to open target process\n");
		return false;
	}
	printf("Opening Target Process...\n");

	// VirtualAllocEx
	LPVOID MyAlloc = VirtualAllocEx(hProc, 0, dll_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (MyAlloc == NULL)
	{
		printf("Fail to allocate memory in Target Process.\n");
		return false;
	}
	printf("Allocating memory in Target Process.\n");

	// VirtualProtectEx
	DWORD dwProtOut = 0;
	if (!VirtualProtectEx(hProc, MyAlloc, dll_size, PAGE_EXECUTE_READWRITE, &dwProtOut))
	{
		printf("Failed to set permissions on loader.\n");
		return false;
	}

	// WriteProcessMemory
	if (!WriteProcessMemory(hProc, MyAlloc, filepath, dll_size, 0))
	{
		printf("Fail to write in Target Process memory.\n");
		return false;
	}
	printf("Creating Remote Thread in Target Process.\n");

	// Reset VirtualProtectEx
	if (!VirtualProtectEx(hProc, MyAlloc, dll_size, dwProtOut, &dwProtOut))
	{
		printf("Failed to reset permissions on loader.\n");
		return false;
	}

	// GetProcAddress
	HMODULE hModuleKernel32 = LoadLibraryA("kernel32");
	if (hModuleKernel32 == 0)
	{
		printf("Failed to load module kernel32\n");
		return false;
	}
	LPTHREAD_START_ROUTINE addrLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(
		hModuleKernel32,
		"LoadLibraryA"
	);

	// CreateRemoteThread
	DWORD dWord;
	HANDLE ThreadReturn = CreateRemoteThread(hProc, NULL, 0, addrLoadLibrary, MyAlloc, 0, &dWord);
	if (ThreadReturn == NULL)
	{
		printf("Fail to create Remote Thread.\n");
		return false;
	}

	WaitForSingleObject(ThreadReturn, INFINITE);
	CloseHandle(ThreadReturn);
	return true;
}

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		printf("Usage : <Exe> <Dll>\n");
		return EXIT_FAILURE;
	}

	// Logger
	const char* loggerPath = GetAbsoluteFilename(argv[0], "Injector");
	printf("Logger path %s\n", loggerPath);

	// Get Process entry
	PROCESSENTRY32 pe32;
	GetProcessEntry32(argv[1], 0, &pe32);
	printf("PROCESS NAME: %s\n", pe32.szExeFile);
	printf("\t- Process ID        = 0x%08X\n", pe32.th32ProcessID); // this process
	printf("\t- Module ID         = 0x%08X\n", pe32.th32ModuleID);	// this module

	// Module
	const char* moduleFilename = GetAbsoluteFilename(argv[0], argv[2]);
	printf("Module file path <%s>\n", moduleFilename);

	// Inject
	if (!InjectModule(pe32.th32ProcessID, moduleFilename))
	{
		printf("Failed to inject <%s>.\n", moduleFilename);
		return EXIT_FAILURE;
	}

	printf("Successfully Injected module <%s> :).\n", moduleFilename);
	return EXIT_SUCCESS;
}
