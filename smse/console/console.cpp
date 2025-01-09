#include "console.h"

#include <Windows.h>
#include <stdio.h>

#include <MinHook.h>

FILE* console_handle = NULL;
HANDLE hConsole;
decltype( &AllocConsole ) fpAllocConsole = NULL;

BOOL WINAPI DetourAllocConsole() {
	return TRUE;
}

void smse::_logAlloc()
{
	if ( !AllocConsole() )
		return;
	_logOpen();
	smse::log( "Allocated Console" );

	if ( MH_CreateHook( &AllocConsole, &DetourAllocConsole, (LPVOID*)&fpAllocConsole ) != MH_OK )
		return;

	if ( MH_EnableHook( &AllocConsole ) != MH_OK )
		return;
}

void smse::_logOpen()
{
	_logClose();
	freopen_s( &console_handle, "CONOUT$", "w", stdout );
}

void smse::_logClose()
{
	if ( !console_handle )
		return;

	fclose( console_handle );
	console_handle = NULL;
}
