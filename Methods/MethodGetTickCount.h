#pragma once
#include <Windows.h>
#include <iostream>

bool MethodGetTickCount() {


    DWORD tickReference = GetTickCount64();

    Sleep(50); 

    DWORD currentTick = GetTickCount64();
    DWORD elapsedTime = currentTick - tickReference;

    if (elapsedTime > 100)
        return true;

    return false;
}