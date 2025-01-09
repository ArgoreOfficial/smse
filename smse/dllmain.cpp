#pragma comment (lib, "user32.lib")
#include <windows.h>

#include <stdio.h>
#include <inttypes.h>

#include <MinHook.h>

#include "core/process.h"
#include "console/console.h"
#include "hooks/lua_hooks.h"

bool onAttach()
{
	if ( MH_Initialize() != MH_OK )
		return false;

	smse::_logAlloc();
	smse::_processInit();

	smse::_hookLua();

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

