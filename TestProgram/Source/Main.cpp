// PipeModulePython.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "..//Headers/TestHeader.h"
#include "..//..//PipeModulePython/Headers/PipeModulePython.h"
#define BOOST_ASIO_NO_WIN32_LEAN_AND_MEAN 
#define WIN32_LEAN_AND_MEAN  
#define _WIN32_WINNT 0x0A00
#include <Windows.h>
#include <boost/process.hpp>
#include <boost/process/v1/child.hpp>
#include <boost/process/v1/pipe.hpp>
#include <boost/process/v1/search_path.hpp>
#include <boost/process/v1/io.hpp>
#include <boost/python.hpp>
#include <boost/process/v1/handles.hpp>
#include <boost/detail/winapi/process.hpp>
#include "..//..//PipeModulePython/Headers/PseudoTerminalSession.h"

struct new_window : ::boost::process::v1::detail::handler_base
{
	// this function will be invoked at child process constructor before spawning process
	template <class WindowsExecutor>
	void on_setup(WindowsExecutor& e) const
	{
		e.creation_flags = ::boost::detail::winapi::CREATE_NEW_CONSOLE_;
	}
};

int main()
{	

	std::cout << "Hello CMake asdasdfddddasd." << std::endl;
	std::cout << "Testet" << greet()  << std::endl;
	boost::process::v1::opstream InputStream;
	boost::process::v1::ipstream out;
	
	// PseudoTerminalSession session(
	// 	"C:\\CTF\\radare2-5.9.8-w64\\radare2-5.9.8-w64\\bin\\r2.bat -d C:\\CTF\\TestScripts\\CTFTestandeNy.exe",
	// 	[](const std::string& output) {
	// 		std::cout << output;
	// 	}
	// );

	PseudoTerminalSession session(
		"C:\\Users\\svart\\source\\repos\\CTFTestande\\out\\build\\x64-debug\\CTFTestande\\CTFTestandeSenaste.exe",
		[](const std::string& output) {
			std::cout << output;
		}
	);

	std::this_thread::sleep_for(std::chrono::seconds(2));
	session.write("pd 5\r\n");

	int hej = 0;

	session.wait();

	// boost::process::v1::child ChildProcess(boost::process::v1::search_path("r2"), "-d", "C:\\CTF\\TestScripts\\CTFTestandeNy.exe", boost::process::v1::std_out > out, boost::process::v1::std_in < InputStream);
	// 	
	// std::string OutputString;
	// char buffer[100];
	// while (ChildProcess.running())
	// {	
	// 	out.read(buffer, 5);
	// 	int ReadAmount = out.gcount();
	// 	if (ReadAmount < 100 -1)
	// 	{
	// 		buffer[ReadAmount] = '\0';
	// 		InputStream.write("aaa\n", sizeof("aaa\n"));
	// 		InputStream.flush();
	// 	}
	// 	std::cout << buffer;
	// }

	return 0;
}
