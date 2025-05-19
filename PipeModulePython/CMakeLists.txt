# CMakeList.txt : CMake project for PipeModulePython, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_library (hello_ext SHARED 
"Headers/PipeModulePython.h" 
"Source/PipeModulePython.cpp" 
)

set_target_properties(hello_ext PROPERTIES
    PREFIX ""
    SUFFIX ".pyd"
)
target_compile_definitions(hello_ext PRIVATE
    BOOST_PYTHON_STATIC_LIB
)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET hello_ext PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
