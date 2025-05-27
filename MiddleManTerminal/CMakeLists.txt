# CMakeList.txt : CMake project for PipeModulePython, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (MiddleManTerminal  
"Headers/MiddleManTerminal.h" 
"Source/MiddleManTerminal.cpp"  "Headers/GeneralPipe.h" "Source/GeneralPipe.cpp" "Headers/GeneralProcess.h"  "Source/GeneralProcess.cpp" "Source/MiddleManTerminalMain.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MiddleManTerminal PROPERTY CXX_STANDARD 20)
endif()

add_library (InteractiveTerminalPipe SHARED 
"Headers/MiddleManTerminal.h" 
"Source/MiddleManTerminal.cpp"  "Headers/GeneralPipe.h" "Source/GeneralPipe.cpp" "Headers/GeneralProcess.h"  "Source/GeneralProcess.cpp" "Source/MiddleManTerminalPython.cpp")

set_target_properties(InteractiveTerminalPipe PROPERTIES
    PREFIX ""
    SUFFIX ".pyd"
)
target_compile_definitions(InteractiveTerminalPipe PRIVATE
    BOOST_PYTHON_STATIC_LIB
)

set_target_properties(InteractiveTerminalPipe PROPERTIES
    PREFIX ""
    SUFFIX ".pyd"
)
target_compile_definitions(InteractiveTerminalPipe PRIVATE
    BOOST_PYTHON_STATIC_LIB
)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET InteractiveTerminalPipe PROPERTY CXX_STANDARD 20)
endif()
# TODO: Add tests and install targets if needed.
