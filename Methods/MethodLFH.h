#pragma once
#include <Windows.h>
#include <iostream>


bool MethodLFH() {
	auto processHeap = reinterpret_cast<ULONG64>(GetProcessHeap());

	auto LFHPtr = reinterpret_cast<VOID**>(processHeap + 0xe4);

	if (*LFHPtr == NULL)
		return true;

	return false;
}