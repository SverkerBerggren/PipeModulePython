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

find_package(Boost 1.88.0 )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
link_directories( $ENV{BOOST_LIBRARYDIR} )
INCLUDE_DIRECTORIES("C:\\Users\\svart\\AppData\\Local\\Programs\\Python\\Python313\\include")

# Include sub-projects.
add_subdirectory ("PipeModulePython")
add_subdirectory ("TestProgram")
add_subdirectory ("MiddleManTerminal")
