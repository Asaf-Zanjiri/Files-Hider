#pragma once

#include "ssdt.h" // For the hooking itself
#include "file_hide_hooks.h" // For the original function prototype typedef (e.g ZwQueryDirectoryFilePtr)

/* Hooks Initialization Methods. */
/// <summary>
/// Registers hooks to various windows functions that are pre-defined in this function.
/// </summary>
void InitializeHooks();
/// <summary>
/// Un-registers hooks to various windows functions that are pre-defined in this function.
/// </summary>
void DeinitializeHooks();