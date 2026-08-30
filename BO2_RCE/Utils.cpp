#include "main.h"

void _sys_ppu_thread_exit(uint64_t val)
{
	system_call_1(41, val);
}


int sys_dbg_read_process_memory(uint64_t address, void* data, size_t size) {
	system_call_4(904, (uint64_t)sys_process_getpid(), address, size, (uint64_t)data);
	return_to_user_prog(int);
}

int sys_dbg_write_process_memory(uint64_t address, void* data, size_t size) {
	system_call_4(905, (uint64_t)sys_process_getpid(), address, size, (uint64_t)data);
	__dcbst((void*)address);
	__sync();
	__isync();
	return_to_user_prog(int32_t);
}

void DetourFunction(int address, void(*hookFunc), void(*stubFunc)) {
	int StubData[8], FuncData[4];

	int hook_address = hookFunc != NULL ? *(int*)hookFunc : 0;
	int stub_address = stubFunc != NULL ? *(int*)stubFunc : 0;

	if (stub_address) {
		int branchAddr = address + 0x10;
		StubData[0] = 0x3D600000 + ((branchAddr >> 16) & 0xFFFF) + (branchAddr & 0x8000 ? 1 : 0);
		StubData[1] = 0x396B0000 + (branchAddr & 0xFFFF);
		StubData[2] = 0x7D6903A6;
		StubData[7] = 0x4E800420;
		sys_dbg_read_process_memory(address, &StubData[3], 0x10);
		sys_dbg_write_process_memory(stub_address, StubData, 0x20);
	}

	if (hook_address) {
		FuncData[0] = 0x3D600000 + ((hook_address >> 16) & 0xFFFF) + (hook_address & 0x8000 ? 1 : 0);
		FuncData[1] = 0x396B0000 + (hook_address & 0xFFFF);
		FuncData[2] = 0x7D6903A6;
		FuncData[3] = 0x4E800420;
		sys_dbg_write_process_memory(address, FuncData, 0x10);
	}
}
