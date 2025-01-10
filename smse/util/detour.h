#pragma once

#include "core/process.h"
#include "../console/console.h"

#include <MinHook.h>

namespace smse {

template<auto _Fn, typename _Ty = decltype( _Fn ) >
struct FuncDetour {
	FuncDetour() 
	{ }

	FuncDetour( uintptr_t _targetOffset ) : target{ reinterpret_cast<_Ty>( getBaseAddr() + _targetOffset ) }
	{ }

	FuncDetour( LPCWSTR _pszModule, LPCSTR _pszProcName, _Ty _fpDetour ):
		pszModule{ _pszModule },
		pszProcName{ _pszProcName },
		detour{ _fpDetour }
	{ }
	
	void createHook( LPCWSTR _pszModule, LPCSTR _pszProcName, _Ty _pDetour ) {
		MH_CreateHookApiEx( _pszModule, _pszProcName, (LPVOID)_pDetour, (LPVOID*)&original, NULL );
	}
		
	void createHook() {
		MH_CreateHookApiEx( pszModule, pszProcName, (LPVOID)detour, (LPVOID*)&original, NULL );
	}

	void createHook( LPVOID pTarget, LPVOID pDetour ) {
		MH_CreateHook( pTarget, pDetour, (LPVOID*)&original );
	}
	
	void createHook( _Ty pDetour ) {
		MH_CreateHook( target, reinterpret_cast<LPVOID>( pDetour ), (LPVOID*)&original );
	}


	_Ty target = nullptr;
	_Ty original = nullptr;
		
	LPCWSTR pszModule = 0;
	LPCSTR pszProcName = 0;
	_Ty detour = nullptr;
};


template<typename _Ty> struct LogDetour;
template<typename _Ty> struct LogDetour<_Ty*> : LogDetour<_Ty> { };
template<typename _Rty, typename... _Args>
struct LogDetour<_Rty( _Args... )>
{
	typedef _Rty( *fptr_t )( _Args... );

	inline static fptr_t detour;
	inline static fptr_t orig_fptr;

	inline static LPCSTR pszProcName;

	void createHook( LPCWSTR _pszModule, LPCSTR _pszProcName )
	{
		pszProcName = _pszProcName;
		detour = []( _Args... _args )
			{
				smse::log( "%s", pszProcName );
				return (orig_fptr)( _args... );
			};

		smse::log( "Hooked %s", _pszProcName );
		MH_CreateHookApiEx( _pszModule, _pszProcName, detour, (LPVOID*)&orig_fptr, NULL );
	}
};

}

#define SMSE_LOG_LUA( _fn ) smse::LogDetour<decltype( &_fn )> _##_fn; _##_fn.createHook( L"lua51", #_fn )