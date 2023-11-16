#include <windows.h>
#include <tlhelp32.h>
#include "macros/patch.h"
#include <string>
#include <iostream>
#include <set>

// Fixes a crash that happens when entering a zone (Caused by too many background processes)

std::set<std::string> procfix_names;

// Make sure each process name is only returned once
BOOL WINAPI procfix_Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
{
    BOOL result;

    while ((result = Process32Next(hSnapshot, lppe)) && !procfix_names.insert(lppe->szExeFile).second);

    return result;
}

// Hook GetProcAddress so we can redirect the real Process32Next to ours
SETWORD(0x0042A2C6, 0xBD90);
SETDWORD(0x0042A2C6 + 2, _procfix_GetProcAddress);
EXTERN_C FARPROC WINAPI procfix_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    if (strcmp(lpProcName, "Process32Next") == 0)
    {
        procfix_names.clear();
        return (FARPROC)procfix_Process32Next;
    }

    return GetProcAddress(hModule, lpProcName);
}
