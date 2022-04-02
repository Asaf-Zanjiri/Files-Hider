#pragma once

#include "ssdt.h"
#include "string_tools.h"

#define FileVolumeNameInformation 58 // For FILE_INFORMATION_CLASS type that's reserved for system use by windows.

/* Function Declaration/Importation - Taken from MSDN */
NTSYSAPI NTSTATUS NTAPI ZwQueryDirectoryFile
(
    IN  HANDLE                  FileHandle,
    IN  HANDLE                  Event,
    IN  PIO_APC_ROUTINE         ApcRoutine,
    IN  PVOID                   ApcContext,
    OUT PIO_STATUS_BLOCK        IoStatusBlock,
    OUT PVOID                   FileInformation,
    IN  ULONG                   Length,
    IN  FILE_INFORMATION_CLASS  FileInformationClass,
    IN  BOOLEAN                 ReturnSingleEntry,
    IN  PUNICODE_STRING         FileName,
    IN  BOOLEAN                 RestartScan
);

NTSYSAPI NTSTATUS NTAPI ZwQueryDirectoryFileEx
(
    IN  HANDLE                 FileHandle,
    IN  HANDLE                 Event,
    IN  PIO_APC_ROUTINE        ApcRoutine,
    IN  PVOID                  ApcContext,
    OUT PIO_STATUS_BLOCK       IoStatusBlock,
    OUT PVOID                  FileInformation,
    IN  ULONG                  Length,
    IN  FILE_INFORMATION_CLASS FileInformationClass,
    IN  ULONG                  QueryFlags,
    IN  PUNICODE_STRING        FileName
);

typedef struct _FILE_BOTH_DIR_INFORMATION {
    ULONG         NextEntryOffset;
    ULONG         FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG         FileAttributes;
    ULONG         FileNameLength;
    ULONG         EaSize;
    CCHAR         ShortNameLength;
    WCHAR         ShortName[12];
    WCHAR         FileName[1];
} FILE_BOTH_DIR_INFORMATION, * PFILE_BOTH_DIR_INFORMATION;

typedef struct _FILE_ID_BOTH_DIR_INFORMATION {
    ULONG         NextEntryOffset;
    ULONG         FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG         FileAttributes;
    ULONG         FileNameLength;
    ULONG         EaSize;
    CCHAR         ShortNameLength;
    WCHAR         ShortName[12];
    LARGE_INTEGER FileId;
    WCHAR         FileName[1];
} FILE_ID_BOTH_DIR_INFORMATION, * PFILE_ID_BOTH_DIR_INFORMATION;

typedef struct _FILE_ID_FULL_DIR_INFORMATION {
    ULONG         NextEntryOffset;
    ULONG         FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG         FileAttributes;
    ULONG         FileNameLength;
    ULONG         EaSize;
    LARGE_INTEGER FileId;
    WCHAR         FileName[1];
} FILE_ID_FULL_DIR_INFORMATION, * PFILE_ID_FULL_DIR_INFORMATION;

typedef struct _FILE_DIRECTORY_INFORMATION {
    ULONG         NextEntryOffset;
    ULONG         FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG         FileAttributes;
    ULONG         FileNameLength;
    WCHAR         FileName[1];
} FILE_DIRECTORY_INFORMATION, * PFILE_DIRECTORY_INFORMATION;

typedef struct _FILE_FULL_DIR_INFORMATION {
    ULONG         NextEntryOffset;
    ULONG         FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG         FileAttributes;
    ULONG         FileNameLength;
    ULONG         EaSize;
    WCHAR         FileName[1];
} FILE_FULL_DIR_INFORMATION, * PFILE_FULL_DIR_INFORMATION;

typedef struct _FILE_NAMES_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NAMES_INFORMATION, * PFILE_NAMES_INFORMATION;

/// <summary>
/// A FILE_VOLUME_NAME_INFORMATION structure. This value is reserved for system use. This value is available starting with Windows 10.
/// </summary>
typedef struct _FILE_VOLUME_NAME_INFORMATION
{
    ULONG	DeviceNameLength;
    WCHAR	DeviceName[1];
} FILE_VOLUME_NAME_INFORMATION, * PFILE_VOLUME_NAME_INFORMATION;


/* Function Prototypes. */
typedef NTSTATUS(*ZwQueryDirectoryFilePtr)
(
    IN  HANDLE                  FileHandle,
    IN  HANDLE                  Event,
    IN  PIO_APC_ROUTINE         ApcRoutine,
    IN  PVOID                   ApcContext,
    OUT PIO_STATUS_BLOCK        IoStatusBlock,
    OUT PVOID                   FileInformation,
    IN  ULONG                   Length,
    IN  FILE_INFORMATION_CLASS  FileInformationClass,
    IN  BOOLEAN                 ReturnSingleEntry,
    IN  PUNICODE_STRING         FileName,
    IN  BOOLEAN                 RestartScan
    );

typedef NTSTATUS(*ZwQueryDirectoryFileExPtr)
(
    IN  HANDLE                 FileHandle,
    IN  HANDLE                 Event,
    IN  PIO_APC_ROUTINE        ApcRoutine,
    IN  PVOID                  ApcContext,
    OUT PIO_STATUS_BLOCK       IoStatusBlock,
    OUT PVOID                  FileInformation,
    IN  ULONG                  Length,
    IN  FILE_INFORMATION_CLASS FileInformationClass,
    IN  ULONG                  QueryFlags,
    IN  PUNICODE_STRING        FileName
    );

/// <summary>
/// Custom made type to allow quick access to the NextEntryOffset item from FileInformation's struct regardless of its type.
/// </summary>
typedef struct _GENERAL_INFORMATION
{
    ULONG NextEntryOffset;
} GENERAL_INFORMATION, * PGENERAL_INFORMATION;



/* Function hooks */
/// <summary>
/// NtQueryDirectoryFile is being used by most software in windows to query which files exist in a directory. This function manipulates it to strip files we want hidden.
/// </summary>
NTSTATUS HookZwQueryDirectoryFile
(
    IN  HANDLE                  FileHandle,
    IN  HANDLE                  Event,
    IN  PIO_APC_ROUTINE         ApcRoutine,
    IN  PVOID                   ApcContext,
    OUT PIO_STATUS_BLOCK        IoStatusBlock,
    OUT PVOID                   FileInformation,
    IN  ULONG                   Length,
    IN  FILE_INFORMATION_CLASS  FileInformationClass,
    IN  BOOLEAN                 ReturnSingleEntry,
    IN  PUNICODE_STRING         FileName,
    IN  BOOLEAN                 RestartScan
);

/// <summary>
/// Some applications (e.g. cmd.exe) use NtQueryDirectoryFileEx instead of NtQueryDirectoryFile. So it needs to be hooked as well to hide files from queries.
/// </summary>
NTSTATUS HookZwQueryDirectoryFileEx
(
    IN  HANDLE                 FileHandle,
    IN  HANDLE                 Event,
    IN  PIO_APC_ROUTINE        ApcRoutine,
    IN  PVOID                  ApcContext,
    OUT PIO_STATUS_BLOCK       IoStatusBlock,
    OUT PVOID                  FileInformation,
    IN  ULONG                  Length,
    IN  FILE_INFORMATION_CLASS FileInformationClass,
    IN  ULONG                  QueryFlags,
    IN  PUNICODE_STRING        FileName
);
