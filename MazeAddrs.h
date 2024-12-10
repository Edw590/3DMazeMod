#ifndef INC_3DMAZEMOD_MAZEADDRS_H
#define INC_3DMAZEMOD_MAZEADDRS_H



#include <stdint.h>

#define wglMakeCurrent_EXE 0x10012A8
#define wglCreateContext_EXE 0x10012A4
#define wglShareLists_EXE 0x10800D0
#define PeekMessageW_EXE 0x100130C
#define TranslateMessage_EXE 0x1001384
#define DispatchMessageW_EXE 0x1001318

uint32_t CreateSSWindow = 0x100967D;
uint32_t SetupInitialWindows = 0x1008CB6;

uint32_t word_101D410 = 0x101D410;



#endif //INC_3DMAZEMOD_MAZEADDRS_H
