#include "ssdt.h"

/* Support functions. */
/// <summary>
/// Disable Memory Write Protection
/// CR0 (Control Register 0) contains a few flags, one of them checks if there's write protection. This function will set it to false.
/// </summary>
void deprotect_mem()
{
	__asm
	{
		push eax // Save EAX
		mov eax, CR0 // Put CR0 contents into EAX
		and eax, 0FFFEFFFFh // Turn off write protection
		mov CR0, eax // Put back the value after the modifications
		pop eax // Load former EAX value
	}
}

/// <summary>
/// Enables Memory Write Protection
/// CR0 (Control Register 0) contains a few flags, one of them checks if there's write protection. This function will set it to true.
/// </summary>
void protect_mem()
{
	__asm
	{
		push eax // Save EAX
		mov eax, CR0 // Put CR0 contents into EAX
		or eax, NOT 0FFFEFFFFh // Turn on write protection
		mov CR0, eax // Put back the value after the modifications
		pop eax // Load former EAX value
	}
}

long ssdtHook(long sysServiceFunction, long hookAddr)
{
	deprotect_mem(); // Disable memory write protections.
	long originalSysServiceFunction = InterlockedExchange((PLONG)&SYSTEMSERVICE(sysServiceFunction), hookAddr); // Switches the left value with the right one, returns the original left side value.
	protect_mem(); // Re-enable memory write protections.

	return originalSysServiceFunction;
}
