#include "..//Headers/GeneralProcess.h"

#include <iostream>
#ifdef __linux__
#include <wait.h>
#include <unistd.h>
#endif // __linux__
GeneralProcess::~GeneralProcess()
{
	//Needed?
}

GeneralProcess::GeneralProcess(const std::string& ProgramArguments)
{
#ifdef _WIN32
	CreateProcessA(NULL, (LPSTR)ProgramArguments.data(), NULL, NULL, TRUE, 0, NULL, NULL, &StartupInfo, &ProcessInformation);
#endif // _WIN#""


#ifdef __linux__
    pid_t pid = fork();
    if (pid == -1) {
        printf("fork failed");
    }
    if (pid == 0) {
        // Child process: launch new terminal running another program
       // execlp("xterm", "xterm", "-e", "./laskeblask", (char*)NULL);
        std::cout << ProgramArguments << "\n";
        execlp(ProgramArguments,"MiddleManProgram", (char*)NULL);
        perror("execlp failed"); // Only reached if execlp fails
    }
    ChildPid = pid;
#endif // __linux__
}

void GeneralProcess::Wait()
{
#ifdef _WIN32
	WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
#endif // _WIN#""
#ifdef __linux__
    int Status;
    waitpid(ChildPid, &Status, 0);
#endif // __linux__

}
