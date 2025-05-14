#ifdef _WIN32
#include <windows.h>
#include <processthreadsapi.h>
#include <consoleapi2.h>
#endif

class GeneralPipe
{
private:
#ifdef _WIN32
	HANDLE PipeHandle;
#endif // _WINE#""
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