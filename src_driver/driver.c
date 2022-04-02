#include "driver.h"

PDEVICE_OBJECT g_pDeviceObject = NULL;

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	NTSTATUS status;
	UNREFERENCED_PARAMETER(pRegistryPath); // Avoid unreferenced parameter errors.

	pDriverObject->DriverUnload = DriverUnload;
	InitializeIrpMajorFunctions(pDriverObject);
	status = SetupIoDevice(pDriverObject);
	InitializeHooks();
	DbgPrintEx(0, 0, "Driver has been loaded successfully!");

	return status;
}

void InitializeIrpMajorFunctions(PDRIVER_OBJECT pDriverObject)
{
	// Register all functions to Io_Unsupported.
	for (unsigned int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
		pDriverObject->MajorFunction[i] = Io_Unsupported;

	// Register Spesific Supported Functions.
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = Create_DeviceIo;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = Close_DeviceIo;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = Buffered_Write;
}

NTSTATUS SetupIoDevice(PDRIVER_OBJECT pDriverObject)
{
	NTSTATUS status;

	// Create an IO device.
	status = IoCreateDevice(pDriverObject, 0, &unicodeDeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &g_pDeviceObject);
	if (!NT_SUCCESS(status))
	{
		DbgPrintEx(0, 0, "Error Creating IO device.");
		return status;
	}
	g_pDeviceObject->Flags |= DO_BUFFERED_IO; // Tells the IO device we're going to be using a buffered IO method.
	g_pDeviceObject->Flags &= (~DO_DEVICE_INITIALIZING); // Unsets the DEVICE_INTERNALIZING flag.

	// Create a symbolic link for the device.
	status = IoCreateSymbolicLink(&unicodeSymLink, &unicodeDeviceName);
	if (!NT_SUCCESS(status))
	{
		DbgPrintEx(0, 0, "Error Creating symbolic link.");
		return status;
	}

	DbgPrintEx(0, 0, "Successfully setup IO device. SymLink: %ws, Name: %ws", unicodeDeviceName.Buffer, unicodeSymLink.Buffer);
	return status;
}

void DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject); // Avoid unreferenced parameter errors.
	if (g_pDeviceObject)
	{
		IoDeleteDevice(g_pDeviceObject);
		IoDeleteSymbolicLink(&unicodeSymLink);
		DbgPrintEx(0, 0, "Deleted IO Device and symbolic link.");
	}
	DeinitializeHooks();
	DbgPrintEx(0, 0, "Driver has been unloaded successfully!");
}