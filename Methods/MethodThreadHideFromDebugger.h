#pragma once

#include <windows.h>
#include <iostream>

THREADINFOCLASS ThreadHideFromDebugger = (THREADINFOCLASS)0x11;
bool hasNtSetInformationThreadRun = false;

typedef NTSTATUS(WINAPI* NtSetInformationThread_t)(HANDLE, THREADINFOCLASS, PVOID, ULONG);
typedef NTSTATUS (WINAPI *NtQueryInformationThread_t)(HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG);

NtSetInformationThread_t fnNtSetInformationThread = NULL;
NtQueryInformationThread_t fnNtQueryInformationThread = NULL;


bool MethodThreadHideFromDebugger() {

    HANDLE hThread = GetCurrentThread();
    fnNtSetInformationThread = (NtSetInformationThread_t)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtSetInformationThread");
    fnNtQueryInformationThread = (NtQueryInformationThread_t)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtQueryInformationThread");

	if (hasNtSetInformationThreadRun == false)
	{
        NTSTATUS errorCode = fnNtSetInformationThread(hThread, ThreadHideFromDebugger, NULL, NULL);
        hasNtSetInformationThreadRun = true;        
	}

    unsigned char lHideThreadQuery = false;
    ULONG lRet = 0;

    NTSTATUS errorCode = fnNtQueryInformationThread(hThread, ThreadHideFromDebugger, &lHideThreadQuery, sizeof(lHideThreadQuery), &lRet);
    CloseHandle(hThread);

	return false; //it will crash if its detected anyway 
}
