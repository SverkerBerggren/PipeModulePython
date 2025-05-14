#include "..//Headers/MiddleManTerminal.h"

#include <vector>
#include <sstream>
#include <string> // for string and to_string()
#include <chrono>
#include <thread>
#include <iostream>
#include "..//Headers/GeneralPipe.h"
#include "..//Headers/GeneralProcess.h"
#ifdef _WIN32
HANDLE TerminalHandle = GetStdHandle(STD_INPUT_HANDLE);
void InjectInputWindows(const std::string& text)
{
    for (char Character : text) {
        INPUT_RECORD ir[2] = {};

        ir[0].EventType = KEY_EVENT;
        ir[0].Event.KeyEvent.bKeyDown = TRUE;
        ir[0].Event.KeyEvent.wRepeatCount = 1;
        ir[0].Event.KeyEvent.wVirtualKeyCode = Character;
        ir[0].Event.KeyEvent.uChar.AsciiChar = Character;

        ir[1] = ir[0];
        ir[1].Event.KeyEvent.bKeyDown = FALSE;

        DWORD written;
        WriteConsoleInputA(TerminalHandle, ir, 2, &written);
    }
}
#endif

#ifdef __linux__	
    InjectInputLinux(const std::string& text)
#endif


void InjectInput(const std::string& text) 
{
        std::cout << "argument kommer hit" << "\n";

#ifdef _WIN32
    InjectInputWindows(text);
 // WIN_32
#endif
#ifdef __linux__	

#endif
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Invalid amount of arguments. A program name and pipe name is needed\n";
        for (int i = 0; i < argc; i++)
        {
            std::cout << argv[i] << "\n";
        }
        // return 1;
    }
    if (argc > 3) {
        std::cout << "To many arguments" << argc << "\n"; 
        for (int i = 0; i < argc;i++)
        {
            std::cout << argv[i] <<"\n";
        }
       // return 1;
    }

    const char* ProgramArguments = argv[1];
    const char* PipeName = argv[2];
    GeneralPipe Pipe(PipeName);
    HANDLE PipeHandle = CreateFileA(PipeName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (!Pipe.IsSuccessfullyConstructed())
    {
        DWORD err = 0;
        err = GetLastError();
        std::cout << "knas med pipen " << PipeName <<" " << err << "\n";
        // return 1;
    }
    
    GeneralProcess Process(ProgramArguments);
    
    std::thread pipeReader([&]() {
        char Buffer[128];
        unsigned long ReadBytes = 0;
        bool Continue = true;
        while (Continue)
        {
            bool Ok = Pipe.Read(Buffer, sizeof(Buffer), ReadBytes);
            if (Ok)
            {
                std::string str(Buffer, (int)ReadBytes);
                InjectInput(str);
            }
        }
        });

    Process.Wait();

    return 0;
}