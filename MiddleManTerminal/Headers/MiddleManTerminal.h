#pragma once

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <processthreadsapi.h>
#include <consoleapi2.h>
#else
#include <pty.h>
#include <unistd.h>
#include <sys/wait.h>
#endif


