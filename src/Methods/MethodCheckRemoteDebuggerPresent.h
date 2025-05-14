#pragma once

#include <windows.h>
#include <iostream>

bool MethodCheckRemoteDebuggerPresent() {
	std::cout << "Called CheckRemoteDebuggerPresent! \n";

	BOOL HasDebugPort = FALSE;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &HasDebugPort);
	return HasDebugPort;
}
