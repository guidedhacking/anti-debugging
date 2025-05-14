#pragma once
#include <Windows.h>
#include <iostream>
#include <chrono>
#include "TimerDetection.h"

bool MethodQPC()
{
    LARGE_INTEGER start, end, frequency;
    QueryPerformanceCounter(&start);
    QueryPerformanceFrequency(&frequency);

    Sleep(50);

    QueryPerformanceCounter(&end);
    
    bool detection_value = (end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart > 100;

    static timer_detection qpc_detection(detection_value);
    qpc_detection.frame();
    qpc_detection.set_condition(!qpc_detection.get_detected());
    qpc_detection.update_detection(detection_value);
    return qpc_detection.get_detected();
}
 