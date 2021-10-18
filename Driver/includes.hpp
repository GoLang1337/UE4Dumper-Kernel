#pragma once
#include <ntifs.h>

ULONG moduleSize;

extern "C" NTSTATUS NTAPI MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);
extern "C" NTSTATUS NTAPI ZwProtectVirtualMemory(HANDLE ProcessHandle, PVOID * BaseAddress, SIZE_T * NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection);
extern "C" NTSTATUS NTAPI IoCreateDriver(PUNICODE_STRING DriverName, PDRIVER_INITIALIZE InitializationFunction);
extern "C" PVOID NTAPI PsGetProcessSectionBaseAddress(__in PEPROCESS Process);
extern "C" PPEB NTAPI PsGetProcessPeb(IN PEPROCESS Process);
extern "C" PVOID PsGetProcessWow64Process(_In_ PEPROCESS Process);
extern "C" NTSTATUS NTAPI ZwQueryInformationThread(IN HANDLE ThreadHandle, IN THREADINFOCLASS ThreadInformationClass, OUT PVOID ThreadInformation, IN ULONG ThreadInformationLength, OUT PULONG ReturnLength OPTIONAL);