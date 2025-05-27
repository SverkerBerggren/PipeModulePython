// PipeModulePython.cpp : Defines the entry point for the application.
//

#include "..//Headers/PipeModulePython.h"
#include "..//..//MiddleManTerminal/Headers/MiddleManTerminal.h"
#include <boost/python.hpp>

char const* greet()
{
	return "abow sag vadds bror";
}

BOOST_PYTHON_MODULE(InteractiveTerminalPipe)
{
	using namespace boost::python;
	def("greet", greet);

	class_<MiddleManTerminal>("InteractiveTerminal").def("InjectInput", &MiddleManTerminal::InjectInput);
}