#pragma once

#include <windows.h>
#include <iostream>

PPEB getPeb(void);

bool MethodPEBBeingDebugged() {
    auto peb = (char*) __readfsdword(0x30);
    return *(peb+0x2);
}

PPEB getPeb() {
    PVOID tmp = NULL;
    __asm
    {
        mov eax, fs:[0x30]
        mov tmp, eax
    }
    return (PPEB)tmp;
}
