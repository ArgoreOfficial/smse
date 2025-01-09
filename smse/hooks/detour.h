#pragma once

#include "core/process.h"
#include "../console/console.h"

#include <MinHook.h>

namespace smse {

template<auto _Fn, typename _Ty = decltype( _Fn ) >
struct sFuncDetour {
	sFuncDetour() 
	{ }

	sFuncDetour( uintptr_t _targetOffset ) : target{ reinterpret_cast<_Ty>( getBaseAddr() + _targetOffset ) }
	{ }

	sFuncDetour( LPCWSTR _pszModule, LPCSTR _pszProcName, _Ty _fpDetour ):
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
	
	void createHook( LPVOID pDetour ) {
		MH_CreateHook( target, pDetour, (LPVOID*)&original );
	}


	_Ty target = nullptr;
	_Ty original = nullptr;
		
	LPCWSTR pszModule = 0;
	LPCSTR pszProcName = 0;
	_Ty detour = nullptr;
};


template<typename _Ty> struct sLogDetour;
template<typename _Ty> struct sLogDetour<_Ty*> : sLogDetour<_Ty> { };
template<typename _Rty, typename... _Args>
struct sLogDetour<_Rty( _Args... )>
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

#define SMSE_LOG_LUA( _fn ) smse::sLogDetour<decltype( &_fn )> _##_fn; _##_fn.createHook( L"lua51", #_fn )