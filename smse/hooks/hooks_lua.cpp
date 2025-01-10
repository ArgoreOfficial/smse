#include "hooks_lua.h"

#include <sm/sm_api.h>

static smse::FuncDetour<lua_setfield>     _lua_setfield;
static smse::FuncDetour<lua_pushcclosure> _lua_pushcclosure;
static smse::FuncDetour<luaL_register>    _luaL_register;
static smse::FuncDetour<lua_newstate>     _lua_newstate;

static decltype( &luaL_loadstring ) _luaL_loadstring;
static decltype( &lua_pcall )       _lua_pcall;

static void lua_setfield_hook( lua_State* L, int idx, const char* k )
{
	_lua_setfield.original( L, idx, k );
}

static void lua_pushcclosure_hook( lua_State* L, lua_CFunction fn, int n )
{
	_lua_pushcclosure.original( L, fn, n );
}

static void luaL_register_hook( lua_State* L, const char* libname, const luaL_Reg* l )
{
	
	if ( false && l != nullptr )
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

	_luaL_register.original( L, libname, l );

	if ( libname )
		smse::log( "Loaded library '%s'", libname );

}


static lua_State* lua_newstate_hook( lua_Alloc f, void* ud )
{
	lua_State* state = _lua_newstate.original( f, ud );

	return state;
}

void smse::hooks::initLua()
{
	FuncLoader luaLoader{ L"lua51.dll" };
	_luaL_loadstring = luaLoader.load<&luaL_loadstring>( "luaL_loadstring" );
	_lua_pcall       = luaLoader.load<&lua_pcall>      ( "lua_pcall" );

	_lua_setfield    .createHook( L"lua51", "lua_setfield",     &lua_setfield_hook );
	_lua_pushcclosure.createHook( L"lua51", "lua_pushcclosure", &lua_pushcclosure_hook );
	_luaL_register   .createHook( L"lua51", "luaL_register",    &luaL_register_hook );
	_lua_newstate    .createHook( L"lua51", "lua_newstate",     &lua_newstate_hook );
}
