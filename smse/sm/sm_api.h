#pragma once

#include <util/reloc.h>

namespace sm
{

static smse::Reloc<void( __fastcall* )( char* )> _g_print{ 0x34d000 };

void print( char* _str ) { _g_print.m_ptr( _str ); }

}