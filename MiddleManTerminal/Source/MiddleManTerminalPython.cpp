#include "../Headers/MiddleManTerminal.h"
#include <boost/python.hpp>

char const* greet()
{
	return "ny sjuka target galna grejer";
}

BOOST_PYTHON_MODULE(InteractiveTerminalPipe)
{
	using namespace boost::python;
	def("greet", greet);
	class_<MiddleManTerminal>("InteractiveTerminal").def("InjectInput", &MiddleManTerminal::InjectInput).def("StartMiddleMan",&MiddleManTerminal::StartMiddleMan);
}