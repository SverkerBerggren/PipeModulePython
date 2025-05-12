#include "..//Headers/PseudoTerminalSession.h"

#ifdef _WIN32

#include <vector>
#include <sstream>

PseudoTerminalSession::PseudoTerminalSession(const std::string& command, OutputCallback cb)
    : outputCallback(std::move(cb))
{
    // Create pipes
    HANDLE hInRead, hOutWrite;
    SECURITY_ATTRIBUTES SecurityAttributes{ sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    CreatePipe(&hInRead, &hInWrite, &SecurityAttributes, 0);
    CreatePipe(&hOutRead, &hOutWrite, &SecurityAttributes, 0);

    // Create ConPTY
    COORD size = { 80, 25 };
    HRESULT hr = CreatePseudoConsole(size, hInRead, hOutWrite, 0, &hPC);
    if (FAILED(hr)) throw std::runtime_error("Failed to create ConPTY");

    // Setup process startup info
    STARTUPINFOEXW si = {};
    si.StartupInfo.cb = sizeof(STARTUPINFOEXW);
    si.StartupInfo.hStdInput = hInRead;
    si.StartupInfo.hStdOutput = hOutWrite;
    si.StartupInfo.dwFlags = STARTF_USESTDHANDLES;

    SIZE_T attrSize = 0;
    InitializeProcThreadAttributeList(NULL, 1, 0, &attrSize);
    std::vector<char> attrBuf(attrSize);
    si.lpAttributeList = reinterpret_cast<PPROC_THREAD_ATTRIBUTE_LIST>(attrBuf.data());

    InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &attrSize);
    UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, hPC, sizeof(hPC), NULL, NULL);

    // Convert command to wide
    std::wstring wcmd(command.begin(), command.end());

    printf("Vilket varde har handlen: %p \n", hInWrite);
    printf("Vilket varde har handlen: %p \n", hPC);
    BOOL success = CreateProcessW(NULL, &wcmd[0], NULL, NULL, TRUE,
        EXTENDED_STARTUPINFO_PRESENT | CREATE_NEW_CONSOLE, NULL, NULL,
        &si.StartupInfo, &pi);
    DWORD err = GetLastError();
    std::cerr << "CreateProcessW failed. Error code: " << err << std::endl;
    if (!success) throw std::runtime_error("Failed to launch process");

    startReader();
}

void PseudoTerminalSession::write(const std::string& input)
{
    DWORD written;
    WriteFile(hInWrite, input.c_str(), input.size(), &written, NULL);
}

void PseudoTerminalSession::startReader()
{
    readerThread = std::thread([this]() {
        char buffer[256];
        DWORD bytesRead;
        while (running && ReadFile(hOutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            buffer[bytesRead] = '\0';
            if (outputCallback) outputCallback(std::string(buffer));
        }
        });
}

void PseudoTerminalSession::wait()
{
    WaitForSingleObject(pi.hProcess, INFINITE);
    running = false;
    if (readerThread.joinable()) readerThread.join();
}

PseudoTerminalSession::~PseudoTerminalSession()
{
    running = false;
    if (readerThread.joinable()) readerThread.join();
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hInWrite);
    CloseHandle(hOutRead);
    ClosePseudoConsole(hPC);
}

#else // UNIX-like systems

#include <cstring>
#include <sys/types.h>

PseudoTerminalSession::PseudoTerminalSession(const std::string& command, OutputCallback cb)
    : outputCallback(std::move(cb))
{
    child_pid = forkpty(&master_fd, NULL, NULL, NULL);
    if (child_pid == -1) throw std::runtime_error("forkpty failed");

    if (child_pid == 0) {
        execl("/bin/sh", "sh", "-c", command.c_str(), (char*)NULL);
        _exit(1);
    }
    else {
        startReader();
    }
}

void PseudoTerminalSession::write(const std::string& input)
{
    ::write(master_fd, input.c_str(), input.size());
}

void PseudoTerminalSession::startReader()
{
    readerThread = std::thread([this]() {
        char buffer[256];
        while (running) {
            ssize_t n = read(master_fd, buffer, sizeof(buffer) - 1);
            if (n <= 0) break;
            buffer[n] = '\0';
            if (outputCallback) outputCallback(std::string(buffer));
        }
        });
}

void PseudoTerminalSession::wait()
{
    int status = 0;
    waitpid(child_pid, &status, 0);
    running = false;
    if (readerThread.joinable()) readerThread.join();
}

PseudoTerminalSession::~PseudoTerminalSession()
{
    running = false;
    if (readerThread.joinable()) readerThread.join();
    close(master_fd);
}

#endif