#ifdef _WIN32
#include <windows.h>
#include <processthreadsapi.h>
#include <consoleapi2.h>
#endif
#include <string>
class GeneralProcess
{
private:
	STARTUPINFOA StartupInfo = { sizeof(STARTUPINFOA) };
	PROCESS_INFORMATION ProcessInformation;
public:
	GeneralProcess(const GeneralProcess& other) = delete;
	GeneralProcess(GeneralProcess&& other) = delete;
	GeneralProcess& operator=(const GeneralProcess& other) = delete;
	GeneralProcess& operator=(GeneralProcess&& other) = delete;
	~GeneralProcess();
	GeneralProcess(const char* ProgramArguments);
	void Wait();
};