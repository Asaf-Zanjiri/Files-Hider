#pragma once

#include <ntddk.h>
#include "hooks.h"
#include "irp_major_functions.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);
void DriverUnload(PDRIVER_OBJECT pDriverObject);

/// <summary>
/// Registers to all IRP major functions.
/// Note: Unless spesified otherwise in the function, all major functions would point to Io_Unsupported.
/// </summary>
/// <param name="DriverObject">Pointer to the driver object that would register for the major functions.</param>
void InitializeIrpMajorFunctions(PDRIVER_OBJECT pDriverObject);

/// <summary>
/// Creates an IO device, as well as a symbolic link for it.
/// </summary>
/// <param name="pDriverObject">Pointer to the driver's object to create the device for.</param>
/// <returns>A NTSTATUS code.</returns>
NTSTATUS SetupIoDevice(PDRIVER_OBJECT pDriverObject);