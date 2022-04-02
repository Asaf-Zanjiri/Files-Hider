#pragma once

#include "ntddk.h"

#define DllImport   __declspec(dllimport)
#define SYSTEMSERVICE(_Function) KeServiceDescriptorTable.ServiceTableBase[*(PULONG)((PUCHAR)_Function + 1)] // Finds the Nt* SSDT entry from Zw* functions. (Works based on how their asm is written - mov eax, 00h)


/// <summary>
/// Defined SSDT structure (32bit).
/// Note: The kernel exports a structure called the KeServiceDescriptorTable. That structure isn't documented nor defined by Microsoft itself since there is really no reason for the average program to be modifying this table. 
/// </summary>
typedef struct SystemServiceDescriptorTable {
    unsigned int* ServiceTableBase; // Base address of the system service table.
    unsigned int* ServiceCounterTableBase;
    unsigned int NumberOfServices; // Number of services in the table.
    unsigned char* ParamTableBase; // Pointer to a table called the param table.
} SSDT;

DllImport SSDT KeServiceDescriptorTable;

/// <summary>
/// Creates a hook to replace a Nt* function with your own function.
/// Note: This function obtains the Nt* functions addresses from Zw* methods that supports them. So methods that aren't supported by Zw* functions won't work.
/// </summary>
/// <param name="sysServiceFunction">Zw* system function memory address.</param>
/// <param name="hookAddr">Memory address of the function to replace the Nt* function with.</param>
/// <returns>Memory address of the original Nt* function that was hooked.</returns>
long ssdtHook(long sysServiceFunction, long hookAddr);
