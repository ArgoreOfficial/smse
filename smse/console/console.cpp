#include "console.h"

#include <MinHook.h>

decltype( &AllocConsole ) fpAllocConsole = NULL;
BOOL WINAPI DetourAllocConsole() { return TRUE; }

static smse::Console g_console{};

void smse::Console::alloc()
{
	if ( !AllocConsole() )
		return;

	open();
	smse::log( "Allocated Console" );

	if ( MH_CreateHook( &AllocConsole, &DetourAllocConsole, (LPVOID*)&fpAllocConsole ) != MH_OK )
		return;

	if ( MH_EnableHook( &AllocConsole ) != MH_OK )
		return;
}

void smse::Console::open()
{
	close();
	freopen_s( &m_handle, "CONOUT$", "w", stdout );
}

void smse::Console::close()
{
	if ( !m_handle )
		return;

	fclose( m_handle );
	m_handle = NULL;
}
