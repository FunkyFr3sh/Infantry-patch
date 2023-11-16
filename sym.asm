%include "macros/setsym.inc"


; vars


; functions

setcglob 0x004C58BE, inf_start
setcglob 0x004017F8, WinMain

; imports

setcglob 0x00579958, _imp__LoadLibraryA_p
setcglob 0x00579A14, _imp__GetModuleHandleA_p
setcglob 0x00579A20, _imp__GetProcAddress_p
