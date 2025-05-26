#ifdef _WIN32
#include <windows.h>
#include <processthreadsapi.h>
#include <consoleapi2.h>
#endif
#ifdef __linux__
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#endif 
class GeneralPipe
{
private:
#ifdef _WIN32
	HANDLE PipeHandle;
	bool CreatePipeWindows(const char* PipeName);
#endif // _WINE#""
#ifdef __linux__
	int FileDescriptor = -1; 
	bool CreatePipeLinux(const char* PipeName);
#endif // __linux__

	bool SuccessfullyConstructed = false;
public:
	GeneralPipe(const char* PipeName);
	GeneralPipe(const GeneralPipe& other) = delete;
	GeneralPipe(GeneralPipe&& other) = delete;
	GeneralPipe& operator=(const GeneralPipe& other) = delete;
	GeneralPipe& operator=(GeneralPipe&& other) = delete;
	~GeneralPipe();
	
	bool Read(char* Buffer, const int BufferSize,unsigned long& ReadBytes);
	bool IsSuccessfullyConstructed();
};