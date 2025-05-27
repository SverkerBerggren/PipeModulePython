#include "../Headers/MiddleManTerminal.h"


int main(int argc, char* argv[]) {
    MiddleManTerminal MiddleMan;
    if (argc < 3) {
        std::cout << "Invalid amount of arguments. A program name and pipe name is needed \n";
        // return 1;
    }
    if (argc > 3) {
        std::cout << "To many arguments" << argc << "\n";
        // return 1;
    }
    const char* ProgramArguments = argv[1];
    const char* PipeName = argv[2];

    return MiddleMan.StartMiddleMan(ProgramArguments, PipeName);
}