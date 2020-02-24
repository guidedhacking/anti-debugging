#pragma once


// only works on 32bit applications. Will add 64bit support later. See https://anti-reversing.com/Downloads/Anti-Reversing/The_Ultimate_Anti-Reversing_Reference.pdf page 5
bool MethodNtGlobalFlag() {
    auto peb = (char*) __readfsdword(0x30);
    auto ntGlobalFlag = *(peb + 0x68);
    return ( (ntGlobalFlag & 0x70) == 0x70);
}