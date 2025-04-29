#pragma once
#include <Windows.h>
#include <iostream>

bool MethodHeapFlags() {
    
#ifdef _WIN64
    ULONGLONG processHeap = reinterpret_cast<ULONGLONG>(GetProcessHeap());
#else
    ULONG processHeap = reinterpret_cast<ULONG>(GetProcessHeap());
#endif

    int flag_offset = 0x40;
    int force_flag_offset = 0x44;

#ifdef _WIN64
    flag_offset = 0x70;
    force_flag_offset = 0x74;
#endif

    ULONG* Flags = reinterpret_cast<ULONG*>(processHeap + flag_offset);
    ULONG* ForceFlags = reinterpret_cast<ULONG*>(processHeap + force_flag_offset);

    return(*Flags != HEAP_GROWABLE || *ForceFlags != 0);
}