#include "..//Headers/GeneralPipe.h"
#include <iostream>
#include <string>
#include <fcntl.h>
#include <exception>
#include <string.h>

#ifdef _WIN32
bool GeneralPipe::CreatePipeWindows(const char* PipeName)
{
	PipeHandle = CreateFileA(PipeName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!(PipeHandle == INVALID_HANDLE_VALUE))
	{
		return = true;
	}
	return false;
}
#endif	
#ifdef __linux__
bool GeneralPipe::CreatePipeLinux(const char* PipeName)
{

	mode_t Mode;
	printf("Laste pipe: Fil %s\n", PipeName);

	FileDescriptor = open(PipeName, O_RDONLY , Mode);

	if (FileDescriptor < 0)
	{
		std::cout << "Invalid file descriptor yo " <<PipeName << "\n";
		printf("Error opening pipe %s \n", strerror(errno));
		return false;
	}

	return true; 
}
#endif	

GeneralPipe::GeneralPipe(const char* PipeName)
{
	#ifdef _WIN32
	SuccessfullyConstructed = CreatePipeWindows(PipeName);
	#endif	
	#ifdef __linux__
	SuccessfullyConstructed = CreatePipeLinux(PipeName);
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
#ifdef __linux__
	  ReadBytes = read(FileDescriptor, Buffer, BufferSize);
	  if (ReadBytes == -1)
	  {
		  printf("Error reading from pipe: %s \n", strerror(errno));
	  }
	  return  0 < (int)ReadBytes;
#endif

	return RetValue;
}

bool GeneralPipe::IsSuccessfullyConstructed()
{
	return SuccessfullyConstructed;
}
