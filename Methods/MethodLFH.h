#pragma once
#include <Windows.h>
#include <iostream>


bool MethodLFH() {
	auto processHeap = reinterpret_cast<ULONG64>(GetProcessHeap());
	int offset = 0xE4;

#ifdef _WIN64
	offset = 0x198;
#endif 

	auto LFHPtr = reinterpret_cast<VOID**>(processHeap + offset);

	if (*LFHPtr == NULL)
		return true;

	return false;
}