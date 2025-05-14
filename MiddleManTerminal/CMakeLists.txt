# CMakeList.txt : CMake project for PipeModulePython, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (MiddleManTerminal  
"Headers/MiddleManTerminal.h" 
"Source/MiddleManTerminal.cpp"  "Headers/GeneralPipe.h" "Source/GeneralPipe.cpp" "Headers/GeneralProcess.h"  "Source/GeneralProcess.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MiddleManTerminal PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
