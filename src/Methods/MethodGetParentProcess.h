#pragma once

#include <windows.h>
#include <iostream>

namespace nt
{
	typedef struct _PROCESS_BASIC_INFORMATION
	{
		NTSTATUS ExitStatus;
		PPEB PebBaseAddress;
		KAFFINITY AffinityMask;
		LONG BasePriority;
		HANDLE UniqueProcessId;
		HANDLE InheritedFromUniqueProcessId;
	} PROCESS_BASIC_INFORMATION, * PPROCESS_BASIC_INFORMATION;
}

std::wstring GetProcessNameById(DWORD pid)
	{
	    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	    if (hProcessSnap == INVALID_HANDLE_VALUE)
	    {
	        return 0;
	    }
	    PROCESSENTRY32 pe32;
	    pe32.dwSize = sizeof(PROCESSENTRY32);
	    std::wstring processName = L"";
	    if (!Process32First(hProcessSnap, &pe32))
	    {
	        CloseHandle(hProcessSnap);
	        return processName;
	    }
	    do
	    {
	        if (pe32.th32ProcessID == pid)
	        {
	            processName = pe32.szExeFile;
	            break;
	        }
	    } while (Process32Next(hProcessSnap, &pe32));
	    
	    CloseHandle(hProcessSnap);
	    return processName;
	}

int GetProcessName(DWORD PID, char *buff, int size)
{
    int len = 0;
    HANDLE hProc = NULL;

    if ( (hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID)) != NULL)
    {
        len = GetModuleBaseNameA(hProc, NULL, (LPSTR)buff, size);
        CloseHandle (hProc);
    }

    return len;
}

std::string debuggerNames[] = { "ollydbg.exe", "ida.exe", "ida64.exe", "idag.exe", "idag64.exe", "idaw.exe", "idaw64.exe", "idaq.exe", "idaq64.exe", "idau.exe", "idau64.exe", "scylla.exe", "scylla_x64.exe", "scylla_x86.exe", "protection_id.exe", "x64dbg.exe", "x32dbg.exe", "windbg.exe", "reshacker.exe", "ImportREC.exe", "IMMUNITYDEBUGGER.EXE", "devenv.exe", "msvsmon.exe" };
bool IsStringBad(const std::string& string)
{
	for (auto& str : debuggerNames)
	{
		if (str == string)
		{
			return true;
		}
	}

	return false;
}

bool GetParentToolHelpCheck()
{
	int pid = -1, len;
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe = { 0 };
	char name[MAX_PATH];
	pe.dwSize = sizeof(PROCESSENTRY32);
	pid = GetCurrentProcessId();

	if (Process32First(h, &pe))
	{
		do
		{
			if (pe.th32ProcessID == pid)
			{
				break;
			}
		} while (Process32Next(h, &pe));
	}
	CloseHandle(h);
	len = GetProcessName(pe.th32ParentProcessID, name, sizeof(name) - 1);
	name[len] = 0;
	std::string stringfied(name);
	return IsStringBad(stringfied);
}


bool NtQueryParentProcessCheck()
{
	nt::PROCESS_BASIC_INFORMATION process_info;
	static decltype(&NtQueryInformationProcess) QueryInformationProcess = (decltype(&NtQueryInformationProcess))(GetProcAddress(LoadLibraryA("ntdll"), "NtQueryInformationProcess"));
	QueryInformationProcess((HANDLE)-1, ProcessBasicInformation, &process_info, sizeof(process_info), nullptr);
	
	char name[MAX_PATH];
	auto len = GetProcessName((DWORD)process_info.InheritedFromUniqueProcessId, name, sizeof(name) - 1);
	name[len] = 0;

	std::string stringfied(name);
	return IsStringBad(stringfied);
}

bool MethodGetParentProcess()
{
	if (GetParentToolHelpCheck() || NtQueryParentProcessCheck())
	{
		return true;
	}

	return false;
}
