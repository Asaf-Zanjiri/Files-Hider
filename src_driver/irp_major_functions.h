#pragma once

#include "hooks.h"

#define ADD_CMD_PREFIX L"add." // Add path to list command operation prefix
#define REMOVE_CMD_PREFIX L"del." // Remove path from list command operation prefix

/* IRP Functions. */
NTSTATUS Create_DeviceIo(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Close_DeviceIo(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Io_Unsupported(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Buffered_Write(PDEVICE_OBJECT DeviceObject, PIRP Irp);
