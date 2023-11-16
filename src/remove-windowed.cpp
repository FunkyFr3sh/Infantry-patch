#include <windows.h>
#include "macros/patch.h"
#include <string>

// Remove the windowed setting from the resolution list when cnc-ddraw is being used

// Hook GetDeviceCaps and return a value that doesn't support windowed mode
SETWORD(0x0042F6E5, 0xBD90);
SETDWORD(0x0042F6E5 + 2, _nowindow_GetDeviceCaps);
EXTERN_C int WINAPI nowindow_GetDeviceCaps(HDC hdc, int index)
{
    if (index == BITSPIXEL && GetProcAddress(GetModuleHandleA("ddraw.dll"), "GameHandlesClose")) // detect cnc-ddraw
    {
        return 8; // only 16/32bit support windowed mode, so we just return a unsupported bit depth here
    }

    return GetDeviceCaps(hdc, index);
}
