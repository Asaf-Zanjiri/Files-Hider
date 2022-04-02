#pragma once

#include "file_hide_hooks.h"

/* Hooks Initialization Methods. */
/// <summary>
/// Registers hooks to various windows functions that are pre-defined in this function.
/// </summary>
void InitializeHooks();
/// <summary>
/// Un-registers hooks to various windows functions that are pre-defined in this function.
/// </summary>
void DeinitializeHooks();

/* Original system functions. */
ZwQueryDirectoryFilePtr		oldZwQueryDirectoryFile;
ZwQueryDirectoryFileExPtr	oldZwQueryDirectoryFileEx;
