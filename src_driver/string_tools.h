#pragma once

#include <wchar.h>
#include "ntddk.h"

/* Global defines */
#define MAX_PATH 291 // I increased the MAX_PATH size due to the `\Device\HarddiskVolume2` format taking more space.
#define ARRAY_SIZE 20
#define HIDE_PREFIX L"@h@"
#define ENABLE_HIDE_PREFIX L"enable.hide" // IRP command message to enable hide prefix
#define DISABLE_HIDE_PREFIX L"disable.hide" // IRP command message to disable hide prefix
#define ADD_CMD_PREFIX L"add." // Add path to list command operation prefix for IRP
#define REMOVE_CMD_PREFIX L"del." // Remove path from list command operation prefix for IRP

/* String tools. */
/// <summary>
/// Enforces a wide string to have a null byte at the end to make it a valid null terminated string.
/// </summary>
/// <param name="string">Pointer to wchar_t list (string) to enforce the null byte on. </param>
/// <param name="len">Length of the wchar_t list (string). </param>
void ForceNullTerminationWide(wchar_t* string, size_t len);

/* Array methods to manage user supplied paths. */
typedef struct {
    WCHAR path[MAX_PATH];
    ULONG length;
} path_t;

typedef struct {
    path_t* paths;
    size_t used;
    size_t size;
} Array;

/// <summary>
/// Driver suitable implementaion of calloc.
/// </summary>
/// <param name="allocSize">Number of bytes to allocated in memory.</param>
/// <returns>Returns the newly allocated memory.</returns>
PVOID calloc(size_t nitems, size_t size);

/// <summary>
/// Initialize a new array to contain paths.
/// </summary>
/// <param name="arr">Pointer to an array object.</param>
/// <param name="size">Max array size.</param>
void InitArray(Array* arr, size_t size);

/// <summary>
/// Free the allocated memory of a given array.
/// </summary>
/// <param name="arr"></param>
void FreeArray(Array* arr);

/// <summary>
/// Insert a new path to the given array. The new path would be placed in a sorted index according to it's length.
/// </summary>
/// <param name="arr">Pointer to an array object.</param>
/// <param name="path">New path object to add to the array.</param>
/// <returns>Returns true if the operation was completed successfully.</returns>
BOOLEAN InsertToArray(Array* arr, path_t path);

/// <summary>
/// Remove a path from the given array.
/// </summary>
/// <param name="arr">Pointer to an array object.</param>
/// <param name="path">Path object to find and remove from the given array.</param>
/// <returns>Returns true if the operation was completed successfully.</returns>
BOOLEAN RemoveFromArray(Array* arr, path_t path);

/// <summary>
/// Looks up a path from an array using binary search and return it's index.
/// Note: If no index was found, the returned index would be -1.
/// </summary>
/// <param name="arr">Pointer to an array object.</param>
/// <param name="path">Path object to find within the given array.</param>
/// <returns></returns>
int FindInArray(Array* arr, path_t path);

/* Global variables. */
Array hidePathsArr;
BOOLEAN enableHidePrefix;