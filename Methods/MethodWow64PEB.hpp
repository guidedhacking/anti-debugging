#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <thread>
#include <mutex>

typedef struct _PROCESS_BASIC_INFORMATION_WOW64
{
    NTSTATUS ExitStatus;
    ULONG64  PebBaseAddress;
    ULONG64  AffinityMask;
    KPRIORITY BasePriority;
    ULONG64  UniqueProcessId;
    ULONG64  InheritedFromUniqueProcessId;

} PROCESS_BASIC_INFORMATION_WOW64, * PPROCESS_BASIC_INFORMATION_WOW64;

typedef NTSTATUS(NTAPI* tNtWow64QueryInformationProcess64)(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);

bool MethodWow64PEB()
{
    // only a wow64 process is going to have this function
    static tNtWow64QueryInformationProcess64 NtWow64QueryInformationProcess64 = (tNtWow64QueryInformationProcess64)GetProcAddress(LoadLibraryA("ntdll"), "NtWow64QueryInformationProcess64");

    if (NtWow64QueryInformationProcess64 == nullptr)
    {
        return false;
    }

    PROCESS_BASIC_INFORMATION_WOW64 wow64 {};
    ULONG outLength = 0;
    NTSTATUS status = NtWow64QueryInformationProcess64((HANDLE)-1, ProcessBasicInformation, &wow64, sizeof(PROCESS_BASIC_INFORMATION_WOW64), &outLength);

    if (wow64.PebBaseAddress)
    {
        return *(bool*)((char*)wow64.PebBaseAddress + 2);
    }

    return false;
}