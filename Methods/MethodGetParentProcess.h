#pragma once

#include <windows.h>
#include <iostream>

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

bool MethodGetParentProcess()
{
    std::string debuggerNames[] = {  "ollydbg.exe", "ida.exe", "ida64.exe", "idag.exe", "idag64.exe", "idaw.exe", "idaw64.exe", "idaq.exe", "idaq64.exe", "idau.exe", "idau64.exe", "scylla.exe", "scylla_x64.exe", "scylla_x86.exe", "protection_id.exe", "x64dbg.exe", "x32dbg.exe", "windbg.exe", "reshacker.exe", "ImportREC.exe", "IMMUNITYDEBUGGER.EXE", "devenv.exe"}; 
  
    int pid = -1, len;
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe = { 0 };
    char name[100];
	pe.dwSize = sizeof(PROCESSENTRY32);
	pid = GetCurrentProcessId();
  
    if( Process32First(h, &pe)) {
    	do {
    		if (pe.th32ProcessID == pid) {
    			break;
    		}
    	} while( Process32Next(h, &pe));
    }
    CloseHandle(h);
	len = GetProcessName(pe.th32ParentProcessID, name, sizeof(name)-1);
	name[len]=0;
	std::string stringfied(name);
    for (int i = 0; i < (sizeof(debuggerNames) / sizeof(*debuggerNames)); i++) {
    	std::cout << debuggerNames[i] << "\n"; 
        if (debuggerNames[i] == stringfied)
        {
        	return true;
        }
        
    }

    return false;
}
