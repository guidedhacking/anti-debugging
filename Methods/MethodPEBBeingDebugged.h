#pragma once

#include <windows.h>
#include <iostream>

PPEB getPeb(void);

bool MethodPEBBeingDebugged() {
    // define our vars
    PPEB pPeb = NULL;
    bool dBug;

    pPeb = getPeb();
    std::cout << "ALALOELE PEB->>> " << pPeb << std::endl;

    dBug = pPeb->BeingDebugged;
    return dBug;
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
