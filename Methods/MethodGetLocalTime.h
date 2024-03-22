#pragma once
#include <Windows.h>
#include <iostream>

bool MethodGetLocalTime()
{
    SYSTEMTIME sysStart, sysend;
    FILETIME fStart, fEnd;
    ULARGE_INTEGER uiStart, uiEnd;

    GetLocalTime(&sysStart);
    Sleep(50); 
    GetLocalTime(&sysend);

    if (!SystemTimeToFileTime(&sysend, &fEnd))
        return false;
    if (!SystemTimeToFileTime(&sysStart, &fStart))
        return false;

    uiStart.LowPart = fStart.dwLowDateTime;
    uiStart.HighPart = fStart.dwHighDateTime;
    uiEnd.LowPart = fEnd.dwLowDateTime;
    uiEnd.HighPart = fEnd.dwHighDateTime;

    return (((uiEnd.QuadPart - uiStart.QuadPart) * 100) / 1000000) > 100; 
}