#include "hooks_playstate.h"

#include <sm_legacy/ContraptionCommon/LuaVM.h>
#include <plugin/plugin_api.h>

#include <util/detour.h>
#include <util/reloc.h>

#include <vector>

/// TODO: figure out params
void luaVM_loadLibraries_detour( sm::LuaVM* _this, void* _param_2, void* _param_3, sm::fp_luaopenlib_t** _loadFuncs );
static smse::FuncDetour<luaVM_loadLibraries_detour> _luaVM_loadLibraries{ 0x6a6f00 };

void switchState_detour( void* _this, int _state );
static smse::FuncDetour<switchState_detour> _switchState{ 0x2da450 };

void openStandardLibraries_detour( sm::LuaVM* _luaVM, void* param_2, smse::LuaLibEnvironment _environment );
static smse::FuncDetour<openStandardLibraries_detour> _openStandardLibraries{ 0x54a7f0 };


extern "C" {

	static int print( lua_State* _luaState )
	{
		smse::log( "SMSE Print" );
		return 1;
	}

	static const struct luaL_Reg smseLib[] = {
		{ "print", print },
		{ NULL, NULL }  /* sentinel */
	};

}

namespace lua
{

static decltype( &lua_gettop ) _gettop;
static decltype( &luaL_register ) _L_register;
static decltype( &lua_settop ) _settop;
static decltype( &luaL_loadstring ) L_loadstring;
static decltype( &lua_pcall ) _pcall;

}

int openSMSELibrary( sm::LuaVM* _luaVM )
{
	smse::log( "Loading SMSE Library" );

	lua::_L_register( _luaVM->m_luaState, "smse", smseLib );
	lua::_settop( _luaVM->m_luaState, -2 );

	/// TODO: stack error check (see core load funcs)

	return 1;
}

int openSMSEPluginLibraries( sm::LuaVM* _luaVM )
{
	smse::log( "Loading User Libraries" );


	smse::LuaInterface& luaInterface = smse::SMSEInterface::getInstance().luaInterface;
	const std::vector<smse::LuaLib>& pluginLibs = luaInterface.getPluginLibraries();

	for ( auto& lib : pluginLibs )
	{
		lua::_L_register( _luaVM->m_luaState, lib.name.c_str(), lib.lib );
		lua::_settop( _luaVM->m_luaState, -2 );
	}


	return 1;
}

static sm::fp_luaopenlib_t _smseLib[] = {
	openSMSELibrary,
	openSMSEPluginLibraries
};

void luaVM_loadLibraries_detour( sm::LuaVM* _this, void* _param_2, void* _param_3, sm::fp_luaopenlib_t** _loadFuncs )
{
	std::vector<sm::fp_luaopenlib_t*> testLoadFuncs{};
	int n = 0;
	while ( _loadFuncs[ n ] != nullptr )
		testLoadFuncs.push_back( _loadFuncs[ n ] ), n++;

	testLoadFuncs.push_back( _smseLib );
	testLoadFuncs.push_back( nullptr );

	_luaVM_loadLibraries.original( _this, _param_2, _param_3, testLoadFuncs.data() );
}

void switchState_detour( void* _this, int _state )
{
	smse::log( "Switch State [%p][%p] to %i", _this, (uintptr_t)_this - smse::getBaseAddr(), _state );

	_switchState.original( _this, _state );
}

static std::string constructLibraryEnvInstall( const std::string& libName, const luaL_Reg* lib )
{
	std::string str = "unsafe_env." + libName + " = {\n";

	int n = 0;
	while ( lib[ n ].name != nullptr )
	{
		if ( n != 0 )
			str += ",\n";

		std::string symbol = libName + "." + lib[ n ].name;
		str += "    " + std::string{ lib[ n ].name } + " = " + symbol;
		n++;
	}

	str += "\n}\n";

	return str;
}

void openStandardLibraries_detour( sm::LuaVM* _luaVM, void* param_2, smse::LuaLibEnvironment _environment )
{
	_openStandardLibraries.original( _luaVM, param_2, _environment );

	/// install smse lib
	/// TODO: move to pluginLib ?
	if ( _environment == smse::LUA_LIB_ENV_GAME )
	{
		std::string str = constructLibraryEnvInstall( "smse", smseLib );

		if ( lua::L_loadstring( _luaVM->m_luaState, str.c_str() ) == 0 )
		{
			int res = lua::_pcall( _luaVM->m_luaState, 0, 0xffffffff, 0 );

			if ( res == 0 )
				smse::log( "Installed SMSE Lua Functions" );
			else
				smse::log( "Failed to SMSE Functions" );

		}
	}

	smse::LuaInterface& luaInterface = smse::SMSEInterface::getInstance().luaInterface;
	const std::vector<smse::LuaLib>& pluginLibs = luaInterface.getPluginLibraries();

	// install plugin libs
	for ( auto& lib : pluginLibs )
	{
		if ( _environment != lib.env )
			continue;

		std::string str = constructLibraryEnvInstall( lib.name, lib.lib );
		if ( lua::L_loadstring( _luaVM->m_luaState, str.c_str() ) == 0 )
		{
			int res = lua::_pcall( _luaVM->m_luaState, 0, 0xffffffff, 0 );

			if ( res == 0 )
				smse::log( "Installed plugin '%s'", lib.name.c_str() );
			else
				smse::log( "Failed to install plugin '%s'", lib.name.c_str() );
		}
	}

}

extern "C" {

	static int pluginFunction( lua_State* _luaState )
	{
		smse::log( "Cool Plugin Function" );
		return 1;
	}

	static const struct luaL_Reg _pluginLib[] = {
		{ "pluginFunction", pluginFunction },
		{ NULL, NULL }  /* sentinel */
	};

}

void smse::hooks::initPlaystate()
{
	_luaVM_loadLibraries.createHook( luaVM_loadLibraries_detour );
	_switchState.createHook( switchState_detour );
	_openStandardLibraries.createHook( openStandardLibraries_detour );

	smse::SMSEInterface* _pInterface = &smse::SMSEInterface::getInstance();

	_pInterface->luaInterface.registerLib( "pluginLib", _pluginLib );


	FuncLoader luaLoader{ L"lua51.dll" };
	lua::_gettop = luaLoader.load<&lua_gettop>( "lua_gettop" );
	lua::_L_register = luaLoader.load<&luaL_register>( "luaL_register" );
	lua::_settop = luaLoader.load<&lua_settop>( "lua_settop" );
	lua::L_loadstring = luaLoader.load<&luaL_loadstring>( "luaL_loadstring" );
	lua::_pcall = luaLoader.load<&lua_pcall>( "lua_pcall" );

}
