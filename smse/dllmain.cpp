#pragma comment (lib, "user32.lib")
#include <Windows.h>
#include <corecrt_startup.h>
#include <console/console.h>

#include <util/detour.h>
#include <util/reloc.h>

#include <hooks/hooks_lua.h>
#include <hooks/hooks_playstate.h>

#include <MinHook.h>

bool onAttach()
{
	if ( MH_Initialize() != MH_OK )
		return false;

	smse::g_console.alloc();

	smse::log( "Process Started" );

	smse::hooks::initLua();
	smse::hooks::initPlaystate();

	if ( MH_EnableHook( MH_ALL_HOOKS ) != MH_OK )
	{
		MessageBoxA( NULL, "ERROR", "Failed to enable hooks", MB_OK );
		return false;
	}

	return true;
}

BOOL APIENTRY DllMain( HMODULE _hModule, DWORD _reason, LPVOID _reserved )
{
	switch ( _reason )
	{
	case DLL_PROCESS_ATTACH:
		return onAttach();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

