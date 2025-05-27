#ifdef _WIN32
#include <windows.h>
#include <processthreadsapi.h>
#include <consoleapi2.h>
#endif
#include <string>
class GeneralProcess
{
private:
#ifdef _WIN32

	STARTUPINFOA StartupInfo = { sizeof(STARTUPINFOA) };
	PROCESS_INFORMATION ProcessInformation;
#endif 
#ifdef __linux__
	pid_t ChildPid;
#endif // __linux__


public:
	GeneralProcess(const GeneralProcess& other) = delete;
	GeneralProcess(GeneralProcess&& other) = delete;
	GeneralProcess& operator=(const GeneralProcess& other) = delete;
	GeneralProcess& operator=(GeneralProcess&& other) = delete;
	~GeneralProcess();
	GeneralProcess(const std::string& ProgramArguments);
	void Wait();
};