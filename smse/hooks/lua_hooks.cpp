#include "lua_hooks.h"

#include "detour.h"

#include <lua/lua.hpp>
#include <sm/sm_api.h>

static smse::sFuncDetour<lua_setfield> _lua_setfield;
void lua_setfieldDetour( lua_State* L, int idx, const char* k )
{
	//smse::log( "global lua_setfield: %s", k );
	_lua_setfield.fptr( L, idx, k );
}

static smse::sFuncDetour<lua_pushcclosure> _lua_pushcclosure;
void lua_pushcclosureDetour( lua_State* L, lua_CFunction fn, int n )
{
	//smse::log( "lua_pushcclosure: [%i] %p", n, (void*)fn );
	_lua_pushcclosure.fptr( L, fn, n );
}

extern "C" {

static int testLuaPrint( lua_State* L )
{
	smse::log( "Logged from lua" );

	return 1;
}

}

decltype( &luaL_loadstring ) fpluaL_loadstring;
decltype( &lua_pcall ) fplua_pcall;

void luaL_dostring2( lua_State* L, const char* _str )
{
	fpluaL_loadstring( L, "SMSELog" );
	fplua_pcall( L, 0, ( -1 ), 0 );
}


static smse::sFuncDetour<luaL_register> _luaL_register;
void luaL_registerDetour( lua_State* L, const char* libname, const luaL_Reg* l )
{
	if ( l != nullptr )
	{
		int n = 0;

		if ( libname )
			smse::log( "%s", libname );
		else
			smse::log( "_G:", l[ n ].func, l[ n ].name );

		do
		{
			if ( l[ n ].func )
			{
				uintptr_t offset = (uintptr_t)l[ n ].func - smse::getBaseAddr();
				smse::log( "    [%p] .%s", offset, l[ n ].name );
			}
			else
				smse::log( "    [%p] .%s", 0, l[ n ].name );
			n++;
		} while ( l[ n ].func || l[ n ].name );
	}

	_luaL_register.fptr( L, libname, l );
}

static const struct luaL_Reg mylib[] = {
	{ "SMSELog", testLuaPrint },
	{ NULL, NULL }  /* sentinel */
};

static smse::sFuncDetour<lua_newstate> _lua_newstate;
lua_State* lua_newstateDetour( lua_Alloc f, void* ud )
{
	lua_State* state = _lua_newstate.fptr( f, ud );
	

	//_luaL_register.fptr( state, NULL, mylib );

	sm::print( "sm::print called from SMSE" );
	
	return state;
}

struct sFuncLoader
{
	sFuncLoader( LPCWSTR _module ) {
		hModule = GetModuleHandleW( _module );
	}

	template<auto _Fn, typename _Ty = decltype( _Fn )>
	_Ty load( const char* _fn )
	{
		return (_Ty)GetProcAddress( hModule, _fn );
	}

	HMODULE hModule;
};

void smse::_hookLua()
{
	sFuncLoader luaLoader{ L"lua51.dll" };
	fpluaL_loadstring = luaLoader.load<&luaL_loadstring>( "luaL_loadstring" );
	fplua_pcall       = luaLoader.load<&lua_pcall>      ( "lua_pcall" );

	// SMSE_LOG_LUA( lua_newstate );
	_lua_newstate.createHook( L"lua51", "lua_newstate", &lua_newstateDetour );
	// SMSE_LOG_LUA( lua_close );
	// SMSE_LOG_LUA( lua_setfield ); 
	_lua_setfield.createHook( L"lua51", "lua_setfield", &lua_setfieldDetour );
	// SMSE_LOG_LUA( luaL_setfuncs );
	// SMSE_LOG_LUA( luaL_loadbufferx );


	// SMSE_LOG_LUA( lua_pushnil );
	// SMSE_LOG_LUA( lua_pushnumber );
	// SMSE_LOG_LUA( lua_pushinteger );
	// SMSE_LOG_LUA( lua_pushlstring );
	// SMSE_LOG_LUA( lua_pushstring );
	// SMSE_LOG_LUA( lua_pushvfstring );
	// SMSE_LOG_LUA( lua_pushcclosure );
	_lua_pushcclosure.createHook( L"lua51", "lua_pushcclosure", &lua_pushcclosureDetour );
	// SMSE_LOG_LUA( lua_pushboolean );
	// SMSE_LOG_LUA( lua_pushlightuserdata );
	// SMSE_LOG_LUA( lua_pushthread );

	// SMSE_LOG_LUA( lua_call );
	// SMSE_LOG_LUA( lua_pcall );
	// SMSE_LOG_LUA( lua_cpcall );
	// SMSE_LOG_LUA( lua_load );

	// SMSE_LOG_LUA( luaL_register );
	_luaL_register.createHook( L"lua51", "luaL_register", &luaL_registerDetour );
}
