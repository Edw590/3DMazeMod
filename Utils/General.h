#ifndef MAZESCRMOD_GENERAL_H
#define MAZESCRMOD_GENERAL_H



#include <tchar.h>
#include <windows.h>

// The Microsoft-chosen newline characters: CR LF
#define NL "\r\n"

// Generic function pointer type, like void* for data pointers (universal, including for casting (use no parameters in that case))
#define funcptr_t(sym_name) void (*sym_name)()

int CDECL MessageBoxPrintf(TCHAR * szCaption, TCHAR * szFormat, ...);



#endif //MAZESCRMOD_GENERAL_H
