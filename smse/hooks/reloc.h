#pragma once

#include <stdint.h>
#include <core/process.h>

namespace smse
{

template<typename _Ty>
struct sReloc
{
	sReloc( uintptr_t _offset ) :
		m_ptr{ reinterpret_cast<_Ty>( smse::getBaseAddr() + _offset ) }
	{
			
	}

	_Ty m_ptr;
};

struct sFuncLoader
{
	sFuncLoader( LPCWSTR _module )
	{
		hModule = GetModuleHandleW( _module );
	}

	template<auto _Fn, typename _Ty = decltype( _Fn )>
	_Ty load( const char* _fn )
	{
		return (_Ty)GetProcAddress( hModule, _fn );
	}

	HMODULE hModule;
};

}