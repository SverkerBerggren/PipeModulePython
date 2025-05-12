#pragma once

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <processthreadsapi.h>
#include <consoleapi2.h>
#else
#include <pty.h>
#include <unistd.h>
#include <sys/wait.h>
#endif

class PseudoTerminalSession {
public:
    using OutputCallback = std::function<void(const std::string&)>;

    PseudoTerminalSession(const std::string& command, OutputCallback cb);
    ~PseudoTerminalSession();

    void write(const std::string& input);
    void wait();

private:
#ifdef _WIN32
    HPCON hPC;
    HANDLE hInWrite;
    HANDLE hOutRead;
    PROCESS_INFORMATION pi;
#else
    int master_fd;
    pid_t child_pid;
#endif

    std::thread readerThread;
    std::atomic<bool> running{ true };
    OutputCallback outputCallback;

    void startReader();
};