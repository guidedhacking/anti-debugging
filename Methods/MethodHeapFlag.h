#pragma once
#include <Windows.h>
#include <iostream>

bool MethodHeapFlags() {
    auto processHeap = reinterpret_cast<ULONG>(GetProcessHeap());
    auto Flags = reinterpret_cast<ULONG*>(processHeap + 0x40);
    auto ForceFlags = reinterpret_cast<ULONG*>(processHeap + 0x44);

    return(*Flags != HEAP_GROWABLE || *ForceFlags != 0);
}