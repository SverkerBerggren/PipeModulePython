# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("PipeModulePython")

Set(PythonCopyOutput "${CMAKE_SOURCE_DIR}/Config/BuildCopy.py")
Set(PythonWorkingDirectory "${CMAKE_SOURCE_DIR}/Config")

if(WIN32)
find_package(Boost 1.88.0 )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( "Dependencies/Windows/Python/include" )
link_directories( $ENV{BOOST_LIBRARYDIR} )
link_directories( "Dependencies/Windows/Python/libs" )
set(CurrentOperativeSystem "Windows")
endif(WIN32)

if (UNIX)
INCLUDE_DIRECTORIES( "Dependencies/linux" )
INCLUDE_DIRECTORIES( "Dependencies/linux/python3.11" )
INCLUDE_DIRECTORIES( "Dependencies/linux/x86_64-linux-gnu" )
set(CurrentOperativeSystem "Linux")
endif (UNIX)

# Include sub-projects.
add_subdirectory ("PipeModulePython")
add_subdirectory ("MiddleManTerminal")
add_subdirectory ("TestProgram")
