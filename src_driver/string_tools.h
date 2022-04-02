#pragma once

#include <wchar.h>
#include "hooks.h"

#define MAX_PATH 261

/* String tools. */
/// <summary>
/// Enforces a wide string to have a null byte at the end to make it a valid null terminated string.
/// </summary>
/// <param name="string">Pointer to wchar_t list (string) to enforce the null byte on. </param>
/// <param name="len">Length of the wchar_t list (string). </param>
void ForceNullTerminationWide(wchar_t* string, size_t len);

/* Dynamic array methods to manage user supplied paths. */
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
/// Driver suitable implementation of realloc.
/// </summary>
/// <param name="mem">Pointer to an existsing allocated buffer.</param>
/// <param name="allocSize">Number of bytes to allocated in memory.</param>
/// <returns>Returns the newly allocated memory.</returns>
PVOID realloc(PVOID mem, size_t allocSize);

/// <summary>
/// Initialize a new array to contain paths.
/// </summary>
/// <param name="arr">Pointer to an array object.</param>
/// <param name="initialSize">Initial array size.</param>
void InitArray(Array* arr, size_t initialSize);

/// <summary>
/// Free the allocated memory of a given array.
/// </summary>
/// <param name="arr"></param>
void FreeArray(Array* arr);

/// <summary>
/// Insert a new path to the given array. The new path would be placed in a sorted index according to it's length.
/// Note: The array would automatically resize if there isn't enough space for the new path.
/// </summary>
/// <param name="arr">Pointer to an array object.</param>
/// <param name="path">New path object to add to the array.</param>
void InsertToArray(Array* arr, path_t path);

/// <summary>
/// Looks up a path from an array using binary search and return it's index.
/// Note: If no index was found, the returned index would be -1.
/// </summary>
/// <param name="arr">Pointer to an array object.</param>
/// <param name="path">Path object to find within the given array.</param>
/// <returns></returns>
int FindInArray(Array* arr, path_t path);

/// <summary>
/// Remove a path from the given array.
/// </summary>
/// <param name="arr">Pointer to an array object.</param>
/// <param name="path">Path object to find and remove from the given array.</param>
void RemoveFromArray(Array* arr, path_t path);


/* Dynamic user input arrays. */
Array hidePathsArr;