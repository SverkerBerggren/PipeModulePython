// PipeModulePython.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <string>
#ifdef _WIN32
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN 
#define WIN32_LEAN_AND_MEAN  
#define _WIN32_WINNT 0x0A00
#include <Windows.h>
#endif

#ifdef __linux__
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <asm/termbits.h> 
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#endif 

#ifdef _WIN32
int WindowsMain()
{
    // Create to the pipe
    HANDLE hPipe = CreateNamedPipeA(
        pipeName,
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_BYTE,
        4,
        1000,
        1000,
        0,
        NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create to pipe\n";
        return 1;
    }

    // Start middleman with pipe name
    std::string cmd = std::string("C:\\VisualStudioProjekt\\PipeModulePython\\out\\build\\x64-debug\\MiddleManTerminal\\MiddleManTerminal.exe ") + "\"C:\\CTF\\radare2-5.9.8-w64\\radare2-5.9.8-w64\\bin\\r2.bat -d C:\\CTF\\CrackMes\\IOLI-crackme\\bin-win32\\crackme0x00.exe\" " + pipeName;

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    LPSTR hej = (LPSTR)cmd.c_str();

    BOOL ok = CreateProcessA(
        NULL, (LPSTR)cmd.c_str(),
        NULL, NULL, TRUE,
        CREATE_NEW_CONSOLE,
        NULL, NULL,
        &si, &pi
    );

    if (!ok) {
        DWORD err = GetLastError();
        std::cerr << "CreateProcessW failed. Error code: " << err << std::endl;
        std::cerr << "Failed to launch middleman\n";
        return 1;
    }

    // Wait a bit for middleman to connect (or use ConnectNamedPipe on middleman side)
    ConnectNamedPipe(hPipe, NULL);

    // Send a command   
    const char* msg = "echo Hello from parent\r\n";
    DWORD written;
    WriteFile(hPipe, msg, strlen(msg), &written, NULL);

    WriteFile(hPipe, "aaa\n", strlen("aaa\n"), &written, NULL);
    WriteFile(hPipe, "seek @main\n", strlen("seek @main\n"), &written, NULL);
    WriteFile(hPipe, "v\n", strlen("v\n"), &written, NULL);


    bool quit = false;

    while (!quit)
    {
        std::string Command;
        std::getline(std::cin, Command);
        Command += "\r\n";
        DWORD written;
        WriteFile(hPipe, Command.data(), Command.size(), &written, NULL);

        if (Command == "q")
        {
            quit = true;
        }
    }

    CloseHandle(hPipe);
    return 0;
}
#endif
#ifdef __linux__    
int LinuxMain()
{
    const char* pipeName = "/tmp/MyMiddlemanPipe.fifo";

    if (mkfifo(pipeName, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0)
    {
        if (errno != EEXIST)
        {
            printf("Pipe failed %s \n", strerror(errno));
            exit(1);
        }
    }

    pid_t pid = fork();

    if (pid == -1) {
        printf("fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child process: launch new terminal running another program
       // execlp("xterm", "xterm", "-e", "./laskeblask", (char*)NULL);
        execlp("xterm", "xterm", "-hold", "-e", "../MiddleManTerminal/MiddleManTerminal", "/home/sverker/.vs/PipeModulePython/out/build/linux-debug/TestProgram/laskeblask", pipeName, (char*)NULL);
        perror("execlp failed"); // Only reached if execlp fails
        return 1;
    }
    else {
        // Parent process: wait for the child (xterm) to finish
        int status;        
        bool quit = false;
        mode_t Mode;
        int PipeFileDescriptor = open(pipeName, O_WRONLY, Mode);
        printf("kommer förbi open\n");
        std::string Command;
        while (!quit)
        {
            std::getline(std::cin, Command);
            Command += "\n";
            write(PipeFileDescriptor, Command.data(), Command.size());

            if (Command == "q")
            {
                quit = true;
            }
        }

        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            std::cout << "Terminal exited with code " << WEXITSTATUS(status) << std::endl;
        }
        else {
            std::cout << "Terminal process terminated abnormally\n";
        }
    }

    // pid_t MiddlemanPid = 0;
    // std::string Path(WorkingDirectory);
    // Path += "/laskeblask";
    // 
    // const char*  Environment[1] = { "abow" };
    // 
    // 
    // char FirstArgument[] = "argument 1";
    // char SecondArgument[] = "argument 2";
    // char*  Arguments[2] = {FirstArgument,SecondArgument };
    // 
    // char FirstEnvoirementVariable[] = "argument 1";
    // // char* EnvoirementVariables[1] = { FirstEnvoirementVariable };
    // char* EnvoirementVariables[2] ;
    // 
    // posix_spawnattr_t SpawnAttributes;
    // posix_spawn_file_actions_t SpawnActions;
    // int Result = posix_spawnattr_init(&SpawnAttributes);
    // SpawnAttributes.__flags = SpawnAttributes.__flags | POSIX_SPAWN_SETSID;
    // 
    // int status = posix_spawn( &MiddlemanPid, Path.data(), &SpawnActions, &SpawnAttributes, Arguments, EnvoirementVariables);

   // printf("%d statusen\n", status);
    return 0;
}

#endif

int main()
{	
#ifdef __linux__    
    return LinuxMain();
#endif
#ifdef _WIN32
    return WindowsMain();
#endif // Win
}
