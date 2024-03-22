#pragma once
#include <Windows.h>
#include <iostream>

bool MethodQPC()
{
    LARGE_INTEGER start, end, frequency;
    QueryPerformanceCounter(&start);
    QueryPerformanceFrequency(&frequency);

    Sleep(50);

    QueryPerformanceCounter(&end);
    return (end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart > 100;
}
