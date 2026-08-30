#pragma once

void _sys_ppu_thread_exit(uint64_t val);
void DetourFunction(int address, void(*hookFunc), void(*stubFunc));
int sys_dbg_write_process_memory(uint64_t address, void* data, size_t size);
int sys_dbg_read_process_memory(uint64_t address, void* data, size_t size);