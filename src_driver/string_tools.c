#include "string_tools.h"

#define TAG 'Allo' // Random tag name for ExAllocatePool2.

void ForceNullTerminationWide(wchar_t* string, size_t len)
{
    if (string[len] != L'\0')
        string[len] = L'\0';
}


/* Dynamic array methods to manage user supplied paths */

PVOID calloc(size_t nitems, size_t size)
{
    PVOID mem = ExAllocatePool2(POOL_FLAG_NON_PAGED, nitems * size, TAG); // Allocate memory.
    if (mem != NULL)
        RtlZeroMemory(mem, nitems * size); // Zero initialization.
    return mem;
}

PVOID realloc(PVOID mem, size_t allocSize)
{
    PVOID newMem = calloc(allocSize, 1); // Allocate new memory.
    RtlCopyMemory(newMem, mem, allocSize); // Copy new memory into the new memory.
    ExFreePoolWithTag(mem, TAG); // Free the old allocated memory.
    return newMem; // Return the new memory
}

void InitArray(Array* arr, size_t initialSize)
{
    arr->paths = (path_t*)calloc(initialSize, sizeof(path_t));
    arr->used = 0;
    arr->size = initialSize;
}

void FreeArray(Array* arr)
{
    ExFreePoolWithTag(arr->paths, TAG); // Free the allocated memory.
    arr->used = arr->size = 0;
}

void InsertToArray(Array* arr, path_t path)
{
    // Initializing variables.
    int i;
    int move_index = -1;

    // Checking if there's space left to add a new path. If not, then add space.
    if (arr->used == arr->size)
    {
        arr->size++;
        arr->paths = realloc(arr->paths, arr->size * sizeof(path_t));
    }

    // Insert sorted by length.
    for (i = 0; (size_t)i < arr->used; i++)
    {
        if (arr->paths[i].length > path.length)
        {
            move_index = i;
            break;
        }
    }

    if (move_index != -1)
    {
        for (i = (int)arr->used++; i > move_index; i--)
            arr->paths[i] = arr->paths[i - 1];
        arr->paths[move_index] = path;
    }
    else
        arr->paths[arr->used++] = path;
}

void RemoveFromArray(Array* arr, path_t path)
{
    // Initialize Variables.
    int i;
    int index = FindInArray(arr, path);

    // If the path doesn't exist within the given array, exit function.
    if (index == -1)
        return;

    // Remove path from list.
    for (i = index; i < (int)arr->used - 1; i++)
        arr->paths[i] = arr->paths[i + 1];

    // Resize array to a smaller size - To save memory.
    arr->used--;
    arr->size--;
    arr->paths = realloc(arr->paths, arr->size * sizeof(path_t));
}

int FindInArray(Array* arr, path_t path)
{
    // If the array is empty, there's no need to check if the path exists inside of it.
    if (arr->used == 0)
        return -1;

    // Initializing Variables
    size_t low = 0;
    size_t high = arr->used - 1;
    size_t mid;

    // Check if the length is higher than the max length / smaller than the min length. If it is, there's no need to iterate the array since there's no way it exists inside it.
    if (arr->paths[high].length < path.length || arr->paths[low].length > path.length)
        return -1;

    // Actual Binary Search
    while (low <= high) {
        mid = low + (high - low) / 2;

        // When the same length is found, check sideways if the string contents are the same. (It's more efficient to compare numbers than strings. Hence, the length checks before string contents check.)
        if (arr->paths[mid].length == path.length)
        {
            size_t temp_mid;

            // Checking if mid's path is the same as the wanted path. If it's not check the next one as long as it's within the same length.
            temp_mid = mid;
            while (arr->paths[temp_mid].length == path.length)
            {
                if (!wcscmp(arr->paths[temp_mid].path, path.path))
                    return (int)temp_mid;
                temp_mid++;
            }

            // Checking if mid's path is the same as the wanted path. If it's not check the previous one as long as it's within the same length.
            temp_mid = mid - 1;
            while (arr->paths[temp_mid].length == path.length)
            {
                if (!wcscmp(arr->paths[temp_mid].path, path.path))
                    return (int)temp_mid;
                temp_mid--;
            }

            return -1;
        }
        if (arr->paths[mid].length < path.length)
            low = mid + 1;
        else
            high = mid - 1;
    }

    return -1;
}
