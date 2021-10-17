#pragma once
#include "includes.hpp"

#include <SubAuth.h>

#define io_copy_memory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define io_protect_memory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define io_allocate_memory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define io_free_memory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define io_module_memory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x5, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _MODULEENTRY
{
	ULONGLONG Address;
	ULONG Size;
} MODULEENTRY, * PMODULEENTRY;

typedef struct _copy_memory
{
	INT32 pid;
	ULONGLONG address;
	ULONGLONG buffer;
	ULONGLONG size;
	BOOLEAN write;
} copy_memory, * pcopy_memory;

typedef struct _protect_memory
{
	INT32 pid;
	ULONGLONG address;
	ULONGLONG size;
	DWORD32 new_protect;
} protect_memory, * pprotect_memory;

typedef struct _allocate_memory
{
	INT32 pid;
	ULONGLONG address;
	ULONGLONG size;
	DWORD32 protect;
} allocate_memory, * pallocate_memory;

typedef struct _free_memory
{
	INT32 pid;
	ULONGLONG address;
} free_memory, * pfree_memory;

typedef struct _module_memory
{
	INT32 pid;
	ULONGLONG address;
} module_memory, * pmodule_memory;