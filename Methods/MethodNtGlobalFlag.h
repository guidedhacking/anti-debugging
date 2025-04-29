#pragma once
#include "../Helpers.h"

// https://anti-reversing.com/Downloads/Anti-Reversing/The_Ultimate_Anti-Reversing_Reference.pdf page 5
bool MethodNtGlobalFlag() {
    auto peb = helpers::get_peb();

    int flag_offset = 0x68;
#ifdef _WIN64
    flag_offset = 0xBC;
#endif

    auto ntGlobalFlag = *(peb + flag_offset);
    return ((ntGlobalFlag & 0x70) == 0x70);
}