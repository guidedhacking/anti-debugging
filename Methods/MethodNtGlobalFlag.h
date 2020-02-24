#pragma once


// only works on 32bit applications. Will add 64bit support later. See https://anti-reversing.com/Downloads/Anti-Reversing/The_Ultimate_Anti-Reversing_Reference.pdf page 5
bool MethodNtGlobalFlag() {
    int tmp = NULL;
    __asm
    {
        mov eax, fs:[0x30]
        mov eax, [eax+0x68]
        and al, 0x70
        mov tmp, eax
    }

    return tmp == 0x70;
}