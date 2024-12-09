#ifndef MAZESCRMOD_EXEPATCHUTILS_H
#define MAZESCRMOD_EXEPATCHUTILS_H



#include <stdbool.h>
#include <stdint.h>
#include "General.h"

void writeMem64(uint32_t addr, uint64_t data);
void writeMem32(uint32_t addr, uint32_t data);
void writeMem16(uint32_t addr, uint16_t data);
void writeMem8(uint32_t addr, uint8_t data);
uint64_t readMem64(void *addr);
uint32_t readMem32(void *addr);
uint16_t readMem16(void *addr);
uint8_t readMem8(void *addr);

void makeCall(uint32_t addr, funcptr_t (func_ptr), bool jump, bool ds_addr);

void blockCall(uint32_t addr);



#endif //MAZESCRMOD_EXEPATCHUTILS_H
