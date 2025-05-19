#include "..//Headers/GeneralPipe.h"
#include <iostream>
#include <string>

GeneralPipe::GeneralPipe(const char* PipeName)
{
	#ifdef _WIN32
	PipeHandle = CreateFileA(PipeName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!(PipeHandle == INVALID_HANDLE_VALUE))
	{
		SuccessfullyConstructed = true;
	}
	#endif	
}

GeneralPipe::~GeneralPipe()
{
	#ifdef _WIN32
	CloseHandle(PipeHandle);
	#endif
}

bool GeneralPipe::Read(char* Buffer,const int BufferSize , unsigned long& ReadBytes)
{
	bool RetValue = false;
#ifdef _WIN32
	BOOL Ok = ReadFile(PipeHandle, Buffer, BufferSize, &ReadBytes, NULL);
	RetValue = Ok;
#endif // _WIN32

	return RetValue;
}

bool GeneralPipe::IsSuccessfullyConstructed()
{
	return SuccessfullyConstructed;
}
