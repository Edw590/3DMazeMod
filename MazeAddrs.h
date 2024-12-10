#ifndef INC_3DMAZEMOD_MAZEADDRS_H
#define INC_3DMAZEMOD_MAZEADDRS_H



#include <stdint.h>

#define wglMakeCurrent_EXE 0x10012A8
#define wglCreateContext_EXE 0x10012A4
#define wglShareLists_EXE 0x10800D0

uint32_t CreateSSWindow_EXE = 0x100967D;
uint32_t SetupInitialWindows_EXE = 0x1008CB6;
uint32_t UpdateWindow_EXE = 0x1009FC1;



#endif //INC_3DMAZEMOD_MAZEADDRS_H
