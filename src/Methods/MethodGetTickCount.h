#pragma once
#include <Windows.h>
#include <iostream>
#include "TimerDetection.h"


bool MethodGetTickCount() {
    auto tickReference = GetTickCount64();

    Sleep(50); 

    auto currentTick = GetTickCount64();
    auto elapsedTime = currentTick - tickReference;

    bool detection_value = elapsedTime > 100;
    static timer_detection tick_detection(detection_value);
    tick_detection.frame();
    tick_detection.set_condition(!tick_detection.get_detected());
    tick_detection.update_detection(detection_value);
    return tick_detection.get_detected();
}