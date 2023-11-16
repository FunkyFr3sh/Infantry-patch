#include <windows.h>
#include "macros/patch.h"
#include "app.h"
#include "imports.h"

// entry point
void start()
{
    app_start();
}

// Required for c++ - You must hook WinMain here and make sure you also update the address for the real WinMain in sym.asm
CALL(0x004C5850, _fake_WinMain);

int WinMainCRTStartup(void);
int APIENTRY fake_WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    if (!imports_init())
        return 0;
    
    return WinMainCRTStartup();
}
