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
#endif // __linux__

int main()
{	
    #ifdef __linux__
    const char* pipeName = "\\\\.\\pipe\\MyMiddlemanPipe";


    std::cout << "hej fran linux \n";
    char inbuf[100];
    int Pipe[2];

    for (int i = 0; i < 2; i++)
    {
        Pipe[i] = i;
    }

    if (pipe(Pipe) < 0)
    {
        exit(1);
    }

    int status = posix_spawn()

    #endif

    #ifdef _WIN32
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
    std::string cmd = std::string("C:\\VisualStudioProjekt\\PipeModulePython\\out\\build\\x64-debug\\MiddleManTerminal\\MiddleManTerminal.exe ") +"\"C:\\CTF\\radare2-5.9.8-w64\\radare2-5.9.8-w64\\bin\\r2.bat -d C:\\CTF\\CrackMes\\IOLI-crackme\\bin-win32\\crackme0x00.exe\" " + pipeName;

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
    ConnectNamedPipe(hPipe,NULL);

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

#endif
    return 0;
}
