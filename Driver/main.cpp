#include "memory.hpp"

UNICODE_STRING DeviceName, SymbolicLink;

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS Status = { };
	ULONG BytesIO = { };
	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);

	ULONG ControlCode = Stack->Parameters.DeviceIoControl.IoControlCode;
	ULONG InputBufferLength = Stack->Parameters.DeviceIoControl.InputBufferLength;

	if (ControlCode == io_copy_memory)
	{
		if (InputBufferLength == sizeof(_copy_memory))
		{
			pcopy_memory req = (pcopy_memory)(Irp->AssociatedIrp.SystemBuffer);

			Status = CopyVirtualMemory(req);
			BytesIO = sizeof(_copy_memory);
		}
		else
		{
			Status = STATUS_INFO_LENGTH_MISMATCH;
			BytesIO = 0;
		}
	}
	else if (ControlCode == io_protect_memory)
	{
		if (InputBufferLength == sizeof(_protect_memory))
		{
			pprotect_memory req = (pprotect_memory)(Irp->AssociatedIrp.SystemBuffer);

			Status = ProtectVirtualMemory(req);
			BytesIO = sizeof(_protect_memory);
		}
		else
		{
			Status = STATUS_INFO_LENGTH_MISMATCH;
			BytesIO = 0;
		}
	}
	else if (ControlCode == io_allocate_memory)
	{
		if (InputBufferLength == sizeof(_allocate_memory))
		{
			pallocate_memory req = (pallocate_memory)(Irp->AssociatedIrp.SystemBuffer);

			Status = AllocateVirtualMemory(req);
			BytesIO = sizeof(_allocate_memory);
		}
		else
		{
			Status = STATUS_INFO_LENGTH_MISMATCH;
			BytesIO = 0;
		}
	}
	else if (ControlCode == io_free_memory)
	{
		if (InputBufferLength == sizeof(_free_memory))
		{
			pfree_memory req = (pfree_memory)(Irp->AssociatedIrp.SystemBuffer);

			Status = FreeVirtualMemory(req);
			BytesIO = sizeof(_free_memory);
		}
		else
		{
			Status = STATUS_INFO_LENGTH_MISMATCH;
			BytesIO = 0;
		}
	}
	else if (ControlCode == io_get_module_base_peb)
	{
		if (InputBufferLength == sizeof(_get_module_base_peb))
		{
			pget_module_base_peb req = (pget_module_base_peb)(Irp->AssociatedIrp.SystemBuffer);

			Status = GetModuleBasePeb(req);
			BytesIO = sizeof(_get_module_base_peb);
		}
		else
		{
			Status = STATUS_INFO_LENGTH_MISMATCH;
			BytesIO = 0;
		}
	}
	else if (ControlCode == io_get_module_size)
	{
		PULONG OutPut = (PULONG)Irp->AssociatedIrp.SystemBuffer;
		*OutPut = moduleSize;

		Status = STATUS_SUCCESS;
		BytesIO = sizeof(*OutPut);
	}

	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = BytesIO;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}

NTSTATUS UnsupportedDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Irp->IoStatus.Status;
}

NTSTATUS HandleDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);

	switch (Stack->MajorFunction)
	{
	case IRP_MJ_CREATE:
		DbgPrint("[ RootKit ] Handle created to the symbolic link: %wZ\n", SymbolicLink);
		break;
	case IRP_MJ_CLOSE:
		DbgPrint("[ RootKit ] Handle closed to the symbolic link: %wZ\n", SymbolicLink);
		break;
	default:
		break;
	}

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Irp->IoStatus.Status;
}

VOID Unload(PDRIVER_OBJECT DriverObject)
{
	NTSTATUS Status = { };

	Status = IoDeleteSymbolicLink(&SymbolicLink);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[ RootKit ] Unable to delete the symbolic link, status: %X\n", Status);
		return;
	}

	IoDeleteDevice(DriverObject->DeviceObject);

	DbgPrint("[ RootKit ] Driver unloaded\n");
}

NTSTATUS DriverInitialize(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	NTSTATUS Status = { };
	PDEVICE_OBJECT DeviceObject = { };

	RtlInitUnicodeString(&DeviceName, L"\\Device\\Xo1337GodPaster");
	RtlInitUnicodeString(&SymbolicLink, L"\\DosDevices\\Xo1337GodPaster");

	Status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[ RootKit ] Unable to create device, status: %X\n", Status);
		return Status;
	}

	Status = IoCreateSymbolicLink(&SymbolicLink, &DeviceName);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[ RootKit ] Unable to create symbolic link, status: %X\n", Status);
		return Status;
	}

	for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
		DriverObject->MajorFunction[i] = &UnsupportedDispatch;

	DeviceObject->Flags |= DO_BUFFERED_IO;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = &HandleDispatch;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = &HandleDispatch;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = &IoControl;
	DriverObject->DriverUnload = &Unload;
	
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return Status;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    return IoCreateDriver(NULL, &DriverInitialize);
}
