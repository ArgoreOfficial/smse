#include "process.h"

#include <corecrt_startup.h>
#include <console/console.h>

#include <MinHook.h>

#include <hooks/lua_hooks.h>
#include <stdint.h>

decltype( &_get_wide_winmain_command_line ) fp_get_wide_winmain_command_line;
wchar_t* __CRTDECL processStartDetour()
{
	smse::log( "[SMSE] Process Started\n" );
	smse::_processOnLaunch();

	return fp_get_wide_winmain_command_line();
}

uintptr_t smse::getBaseAddr()
{
	static uintptr_t g_baseAddr = reinterpret_cast<uintptr_t>( GetModuleHandle( NULL ) );
	return g_baseAddr;
}

void smse::_processInit()
{
	MH_STATUS r = MH_CreateHookApiEx(
		L"api-ms-win-crt-runtime-l1-1-0",
		"_get_wide_winmain_command_line", &processStartDetour, (LPVOID*)&fp_get_wide_winmain_command_line, NULL );

}

void smse::_processOnLaunch()
{

}

