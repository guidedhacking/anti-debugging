#pragma once
#include <Windows.h>
#include <iostream>
#include "TimerDetection.h"

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

    bool detection_value = (((uiEnd.QuadPart - uiStart.QuadPart) * 100) / 1000000) > 100;
    static timer_detection local_time_detection(detection_value);
    local_time_detection.frame();
    local_time_detection.set_condition(!local_time_detection.get_detected());
    local_time_detection.update_detection(detection_value);
    return local_time_detection.get_detected();
}