#pragma once

#include <hooks/reloc.h>

namespace sm
{
	static smse::sReloc<void( __fastcall* )( char* )> _g_print{ 0x34d000 };

	void print( char* _str ) { _g_print.m_ptr( _str ); }
}