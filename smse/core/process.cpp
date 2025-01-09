#include "process.h"

#include <Windows.h>
#include <stdint.h>

uintptr_t smse::getBaseAddr()
{
	static uintptr_t g_baseAddr = reinterpret_cast<uintptr_t>( GetModuleHandle( NULL ) );
	return g_baseAddr;
}
