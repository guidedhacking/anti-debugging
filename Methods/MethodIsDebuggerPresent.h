#pragma once

#include <windows.h>
#include <iostream>

bool MethodIsDebuggerPresent() {
	std::cout << "Called MethodIsDebuggerPresent! \n";
	return IsDebuggerPresent() != 0;
}
