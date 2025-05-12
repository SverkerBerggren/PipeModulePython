# CMakeList.txt : CMake project for PipeModulePython, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_library (hello_ext SHARED 
"Headers/PipeModulePython.h" 
"Headers/PseudoTerminalSession.h"
"Source/PipeModulePython.cpp"
"Source/PseudoTerminalSession.cpp" )


# # Choose correct Boost.Python lib based on build type
# if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#     set(BOOST_PYTHON_LIB "$ENV{BOOST_LIBRARYDIR}/libboost_python313-vc143-mt-gd-x64-1_88.lib")
# else()
#     set(BOOST_PYTHON_LIB "$ENV{BOOST_LIBRARYDIR}/libboost_python313-vc143-mt-x64-1_88.lib")
# endif()
set_target_properties(hello_ext PROPERTIES
    PREFIX ""
    SUFFIX ".pyd"
)


target_compile_definitions(hello_ext PRIVATE
    BOOST_PYTHON_STATIC_LIB
)

target_link_libraries( hello_ext "C:\\Users\\svart\\AppData\\Local\\Programs\\Python\\Python313\\libs\\python313.lib")
#target_link_libraries( hello_ext ${BOOST_PYTHON_LIB})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET hello_ext PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
