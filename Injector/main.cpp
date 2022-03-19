#include "injector.h"

int main(int argc, char* argv[])
{
	// Args required
	if (argc < 3)
	{
		printf("[!]Usage : <ExeWindowName> <DLLFilepath>\n");
		return EXIT_FAILURE;
	}

	// Get Process entry
	PROCESSENTRY32 pe32;
	GetProcessEntry32ByName(argv[1], 0, &pe32);
	printf("[+]PROCESS NAME: %s\n", pe32.szExeFile);
	printf("\t[+]Process ID        = 0x%08X\n", pe32.th32ProcessID);		// this process
	printf("\t[+]Module ID         = 0x%08X\n\n", pe32.th32ModuleID);	// this module

	// Inject module
	char dllFilepath[_MAX_PATH]; // getting the full path of the dll file
	GetFullPathName(argv[2], _MAX_PATH, dllFilepath, NULL);
	if (!InjectModule(pe32.th32ProcessID, dllFilepath))
	{
		printf("[!]Failed to inject <%s>.\n", dllFilepath);
		return EXIT_FAILURE;
	}

	// Successfully
	printf("[+]Successfully Injected module <%s> :).\n\n", dllFilepath);
	system("pause");

	return EXIT_SUCCESS;
}
