#pragma once

#include "string_tools.h"

/* IRP Functions. */
NTSTATUS Create_DeviceIo(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Close_DeviceIo(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Io_Unsupported(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Buffered_Write(PDEVICE_OBJECT DeviceObject, PIRP Irp);