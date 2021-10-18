#pragma once
#include "ioctl.hpp"

class driver
{
	HANDLE DriverHandle = { };
	DWORD ProcessId = { };
public:
	driver(DWORD pid) : ProcessId(pid) { };

	void init();

	void read_virtual_memory(PVOID Base, PVOID Buffer, DWORD Size);
	void write_virtual_memory(PVOID Base, PVOID Buffer, DWORD Size);
	void protect_virtual_memory(PVOID Base, DWORD Size, DWORD Protection);

	PVOID allocate_virtual_memory(DWORD Size, DWORD Protection);

	void free_virtual_memory(PVOID Address);

	PVOID get_module_base_peb();
	DWORD get_module_size();
};