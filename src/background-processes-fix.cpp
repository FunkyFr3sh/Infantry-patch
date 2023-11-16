#include <windows.h>
#include <tlhelp32.h>
#include "macros/patch.h"
#include <string>
#include <iostream>
#include <set>

// Fixes a crash that happens when entering a zone (Caused by too many background processes)

std::set<std::string> ProcessList;

// Make sure each process name is only returned once
BOOL WINAPI fake_Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
{
    BOOL result;

    while ((result = Process32Next(hSnapshot, lppe)) && !ProcessList.insert(lppe->szExeFile).second);

    return result;
}

// Hook GetProcAddress so we can redirect the real Process32Next to ours
SETBYTE(0x0042A2C6, 0xBD);
SETDWORD(0x0042A2C6 + 1, _fake_GetProcAddress);
SETBYTE(0x0042A2C6 + 5, 0x90);
EXTERN_C FARPROC WINAPI fake_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    if (strcmp(lpProcName, "Process32Next") == 0)
    {
        ProcessList.clear();
        return (FARPROC)fake_Process32Next;
    }

    return GetProcAddress(hModule, lpProcName);
}
