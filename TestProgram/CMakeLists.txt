# CMakeList.txt : CMake project for PipeModulePython, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (TestProgram
"..//PipeModulePython/Headers/PipeModulePython.h" 
"..//PipeModulePython/Source/PipeModulePython.cpp"
"Source/Main.cpp"
 "Headers/TestHeader.h")

target_link_libraries( TestProgram "C:\\Users\\svart\\AppData\\Local\\Programs\\Python\\Python313\\libs\\python313.lib")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TestProgram PROPERTY CXX_STANDARD 20)
endif()


# TODO: Add tests and install targets if needed.
