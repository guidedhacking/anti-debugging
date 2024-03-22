#pragma once
#include <windows.h>
#include <iostream>

inline bool MethodTrapFlag()
{
    __try
    {
        __asm
        {
            pushfd
            or word ptr[esp], 0x100
            popfd
            nop
        }
    }
    __except (1)
    {
        return false;
    }
    return true;
}