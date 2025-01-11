#pragma once

#include <util/reloc.h>

namespace sm
{

static smse::ptr_reloc<void ( char* )> _g_print{ 0x34d000 };
static void print( char* _str ) { 
	_g_print( _str ); 
}

}