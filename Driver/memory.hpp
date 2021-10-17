#pragma once
#include "ioctl.hpp"

NTSTATUS CopyVirtualMemory(pcopy_memory req)
{
	NTSTATUS Status = { };
	SIZE_T Bytes = { };
	PEPROCESS TargetProcess = { };

	Status = PsLookupProcessByProcessId((HANDLE)req->pid, &TargetProcess);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[ RootKit ] Unable to find process id: %X\n", Status);
		return Status;
	}

	BOOLEAN bWrite = req->write;

	if (bWrite)
	{
		Status = MmCopyVirtualMemory(IoGetCurrentProcess(), (PVOID)req->buffer, TargetProcess, (PVOID)req->address, req->size, UserMode, &Bytes);
	}
	else
	{
		Status = MmCopyVirtualMemory(TargetProcess, (PVOID)req->address, IoGetCurrentProcess(), (PVOID)req->buffer, req->size, UserMode, &Bytes);
	}

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[ RootKit ] Unable to copy virtual memory: %X\n", Status);
		return Status;
	}

	ObfDereferenceObject(TargetProcess);
	return Status;
}

NTSTATUS ProtectVirtualMemory(pprotect_memory req)
{
	NTSTATUS Status = { };
	PEPROCESS TargetProcess = { };
	KAPC_STATE ApcState = { };

	Status = PsLookupProcessByProcessId((HANDLE)req->pid, &TargetProcess);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[ RootKit ] Unable to find process id: %X\n", Status);
		return Status;
	}

	KeStackAttachProcess(TargetProcess, &ApcState);

	PVOID BaseAddress = (PVOID)req->address;
	SIZE_T RegionSize = req->size;

	Status = ZwProtectVirtualMemory(ZwCurrentProcess(), &BaseAddress, &RegionSize, PAGE_EXECUTE_READWRITE, NULL); // if you want to pass protection via user mode, replace the fourth argument with req->new_protect

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[ RootKit ] Unable to change page protection: %X\n", Status);
		return Status;
	}

	KeUnstackDetachProcess(&ApcState);
	ObfDereferenceObject(TargetProcess);

	return Status;
}

NTSTATUS AllocateVirtualMemory(pallocate_memory req)
{
	NTSTATUS Status = { };
	PEPROCESS TargetProcess = { };

	Status = PsLookupProcessByProcessId((HANDLE)req->pid, &TargetProcess);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[ RootKit ] Unable to find process id: %X\n", Status);
		return Status;
	}

	KeAttachProcess(TargetProcess);

	PVOID BaseAddress = 0;
	SIZE_T RegionSize = req->size;

	Status = ZwAllocateVirtualMemory((HANDLE)-1, &BaseAddress, 0, &RegionSize, 0x3000, req->protect);
	MmSecureVirtualMemory(BaseAddress, RegionSize, 4);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[ RootKit ] Unable to allocate memory: %X\n", Status);
		return Status;
	}

	req->address = (ULONGLONG)BaseAddress;

	DbgPrint("[ RootKit ] %llx\n", (uintptr_t)req->address);

	KeDetachProcess();
	ObfDereferenceObject(TargetProcess);

	return Status;
}

NTSTATUS FreeVirtualMemory(pfree_memory req)
{
	NTSTATUS Status = { };
	PEPROCESS TargetProcess = { };

	Status = PsLookupProcessByProcessId((HANDLE)req->pid, &TargetProcess);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[ RootKit ] Unable to find process id: %X\n", Status);
		return Status;
	}

	PVOID BaseAddress = (PVOID)req->address;
	SIZE_T RegionSize = 0;

	Status = ZwFreeVirtualMemory(ZwCurrentProcess(), &BaseAddress, &RegionSize, MEM_RELEASE);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[ RootKit ] Unable to free the memory page: %X\n", Status);
		return Status;
	}

	ObfDereferenceObject(TargetProcess);

	return Status;
}

NTSTATUS GetModuleBase(pmodule_memory req)
{
	NTSTATUS Status = { };
	PEPROCESS TargetProcess = { };

	Status = PsLookupProcessByProcessId((HANDLE)req->pid, &TargetProcess);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[ RootKit ] Unable to find process id: %X\n", Status);
		return Status;
	}

	req->address = (ULONGLONG)PsGetProcessSectionBaseAddress(TargetProcess);

	return Status;
}