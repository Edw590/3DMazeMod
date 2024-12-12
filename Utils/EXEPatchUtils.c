#include <windows.h>
#include "EXEPatchUtils.h"

// Copied, adapted and updated from F1DP (Fallout 1 DOS Patcher).

// All these functions suppose `addr` is an address value seen on IDA as the value if the EXE would load in its supposed
// address, 0x10000 (for the code section), and something else for the data section. This is important because they
// automatically they care of seeing if the address is supposed to be from the Data section or the Code section and it
// is corrected internally. So do NOT correct it externally, or it WILL be wrong inside the functions.

// Also, the memory access functions have been updated to conform with the C standard part of strict aliasing. One can
// only cast a pointer to void* or char* - so here I do that (uint8_t*, which is the same), and all the operations are
// done always using a char*.
// Note: I made them supposedly to replace all memory writes and reads, but no way I'm gonna do it. New code can use
// them, but the old one will remains as is. I don't even think it's a problem here.

static void writeMem(uint32_t addr, uint64_t data, uint32_t n_bytes) {
	if (0 == addr) {
		return;
	}

	uint8_t *addr_cast = (uint8_t *) addr;

	DWORD old_protect;
	VirtualProtect((void *) addr, n_bytes, PAGE_EXECUTE_READWRITE, &old_protect);
	for (int i = 0; i < n_bytes; ++i) {
		*(addr_cast + i) = (uint8_t) (data >> i * 8u);
	}
	VirtualProtect((void *) addr, n_bytes, old_protect, &old_protect);
}
void writeMem64(uint32_t addr, uint64_t data) {
	writeMem(addr, data, sizeof(uint64_t));
}
void writeMem32(uint32_t addr, uint32_t data) {
	writeMem(addr, data, sizeof(uint32_t));
}
void writeMem16(uint32_t addr, uint16_t data) {
	writeMem(addr, data, sizeof(uint16_t));
}
void writeMem8(uint32_t addr, uint8_t data) {
	writeMem(addr, data, sizeof(uint8_t));
}

static uint64_t readMem(void *addr, uint32_t n_bytes) {
	uint32_t ret_var = 0;
	if (0 == addr) {
		return ret_var;
	}

	uint8_t *addr_cast = addr;

	for (int i = 0; i < n_bytes; ++i) {
		ret_var |= ((uint32_t) *addr_cast) << i*8;
	}

	return ret_var;
}
uint64_t readMem64(void *addr) {
	return readMem(addr, sizeof(uint64_t));
}
uint32_t readMem32(void *addr) {
	return readMem(addr, sizeof(uint32_t));
}
uint16_t readMem16(void *addr) {
	return readMem(addr, sizeof(uint16_t));
}
uint8_t readMem8(void *addr) {
	return readMem(addr, sizeof(uint8_t));
}


void makeCall(uint32_t addr, funcptr_t (func_ptr), bool jump, bool ds_addr) {
	if (ds_addr) {
		writeMem16(addr, jump ? 0x25FF : 0x15FF);
		writeMem32(addr + 2, (uint32_t) (func_ptr));
	} else {
		writeMem8(addr, jump ? 0xE9 : 0xE8);
		writeMem32(addr + 1, (uint32_t) (func_ptr) - ((uint32_t) (addr) + 5));
	}
}

void blockCall(uint32_t addr) {
	uint16_t two_bytes = readMem16((void *) addr);
	if (two_bytes == 0x15FF || two_bytes == 0x25FF) {
		writeMem(addr, 0x909090909090, 6);
	} else {
		writeMem(addr, 0x9090909090, 5);
	}
}
