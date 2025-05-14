#include "..//Headers/GeneralProcess.h"

GeneralProcess::~GeneralProcess()
{
	//Needed?
}

GeneralProcess::GeneralProcess(const char* ProgramArguments)
{
#ifdef _WIN32
	CreateProcessA(NULL, (LPSTR)ProgramArguments, NULL, NULL, TRUE, 0, NULL, NULL, &StartupInfo, &ProcessInformation);
#endif // _WIN#""

}

void GeneralProcess::Wait()
{
#ifdef _WIN32
	WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
#endif // _WIN#""
}
