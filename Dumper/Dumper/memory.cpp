#include <Windows.h>
#include "memory.h"

HANDLE hProcess;
uint64 Base;

bool Read(void *address, void *buffer, uint64 size) {
    mem->read_virtual_memory((void*)address, (void*)buffer, (uint64)size);
    return true;
}

bool ReaderInit(uint32 pid) {
  hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  return hProcess != nullptr;
}
