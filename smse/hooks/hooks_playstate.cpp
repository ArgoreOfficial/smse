#include "hooks_playstate.h"

#include <arg/func_loader.hpp>

#include <plugin/plugin_api.h>

#include <sm_legacy/ContraptionCommon/LuaVM.h>

#include <sm_legacy/ContraptionCommon/Joint.h>
#include <sm_legacy/ContraptionCommon/RotationalJoint.h>

#include <util/detour.h>
#include <util/reloc.h>

#include <vector>
#include <memory>

/// TODO: figure out params
void luaVM_loadLibraries_detour( sm::LuaVM* _this, void* _param_2, void* _param_3, sm::fp_luaopenlib_t** _loadFuncs );
static smse::FuncDetour<luaVM_loadLibraries_detour> _luaVM_loadLibraries{ 0x6a6f00 };

void switchState_detour( void* _this, int _state );
static smse::FuncDetour<switchState_detour> _switchState{ 0x2da450 };

void openStandardLibraries_detour( sm::LuaVM* _luaVM, void* param_2, smse::LuaLibEnvironment _environment );
static smse::FuncDetour<openStandardLibraries_detour> _openStandardLibraries{ 0x54a7f0 };

namespace lua
{

static decltype( &luaL_register ) L_register;
static decltype( &lua_settop ) _settop;
static decltype( &luaL_loadstring ) L_loadstring;
static decltype( &lua_pcall ) _pcall;
}

// is this a wrap_Joint ?
typedef std::shared_ptr<sm::RotationalJoint> bearing_sptr;

bearing_sptr& popBearingFromLua_detour( bearing_sptr& _pOutPtr, lua_State* _luaState );
static smse::FuncDetour<popBearingFromLua_detour> _popBearingFromLua{ 0x827140 };

bearing_sptr& popBearingFromLua_detour( bearing_sptr& _pOutPtr, lua_State* _luaState )
{
	_popBearingFromLua.original( _pOutPtr, _luaState );
	smse::log( "Got Bearing %f", _pOutPtr->m_angleReadOnly );
	return _pOutPtr;
}

extern "C" {

	static int print( lua_State* _luaState )
	{
		smse::log( "SMSE Print" );
		return 1;
	}

	static const luaL_Reg smseLib[] = {
		{ "print", print },
		{ NULL, NULL }  /* sentinel */
	};

}

#define SMSE_ASSERT( _b, _msg ) if ( _b ) { \
	smse::logCol( smse::LogColor::RED, _msg ); \
	MessageBoxA( NULL, "Error", _msg, MB_OK ); \
	Sleep( 1000000 ); \
	return 0; \
}

int openSMSELibrary( sm::LuaVM* _luaVM )
{
	smse::log( "Loading SMSE Library" );
	
	int top1 = lua_gettop( _luaVM->m_luaState );
	int top2 = lua_gettop( _luaVM->m_luaState );
	smse::log( "%i %i", top1, top2 );
	
	lua::L_register( _luaVM->m_luaState, "smse", smseLib );
	lua::_settop( _luaVM->m_luaState, -2 );

	int top3 = lua_gettop( _luaVM->m_luaState );
	SMSE_ASSERT( top2 != top3, "top2 != top3")
	int top4 = lua_gettop( _luaVM->m_luaState );
	SMSE_ASSERT( top1 != top2, "top1 != top2" );
	
	smse::log( "Loaded SMSE Library" );
	
	return 1;
}

int openSMSEPluginLibraries( sm::LuaVM* _luaVM )
{
	smse::log( "Loading User Libraries" );
	
	smse::LuaInterface* luaInterface = smse::SMSEInterface::getInstance().getLuaInterface();
	const std::vector<smse::LuaLib>& pluginLibs = luaInterface->getPluginLibraries();

	for ( auto& lib : pluginLibs )
	{

		int top1 = lua_gettop( _luaVM->m_luaState );
		int top2 = lua_gettop( _luaVM->m_luaState );
		
		smse::log( "  - [%s]", lib.name.c_str() );
		lua::L_register( _luaVM->m_luaState, lib.name.c_str(), lib.lib );
		lua::_settop( _luaVM->m_luaState, -2 );

		int top3 = lua_gettop( _luaVM->m_luaState );
		SMSE_ASSERT( top2 != top3, "top2 != top3" );
		int top4 = lua_gettop( _luaVM->m_luaState );
		SMSE_ASSERT( top1 != top2, "top1 != top2" );

	}

	return 1;
}

static sm::fp_luaopenlib_t _smseLib[] = {
	openSMSELibrary,
	openSMSEPluginLibraries
};

void luaVM_loadLibraries_detour( sm::LuaVM* _luaVM, void* _param_2, void* _param_3, sm::fp_luaopenlib_t** _loadFuncs )
{
	smse::log( "LuaVM [%p]", _luaVM );

	std::vector<sm::fp_luaopenlib_t*> testLoadFuncs{};
	int n = 0;
	while ( _loadFuncs[ n ] != nullptr )
		testLoadFuncs.push_back( _loadFuncs[ n ] ), n++;

	testLoadFuncs.push_back( _smseLib );
	testLoadFuncs.push_back( nullptr );

	_luaVM_loadLibraries.original( _luaVM, _param_2, _param_3, testLoadFuncs.data() );
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
				smse::log( "Installed SMSE library" );
			else
				smse::logCol( smse::RED, "Failed to install SMSE library" );

		}
	}

	smse::LuaInterface* luaInterface = smse::SMSEInterface::getInstance().getLuaInterface();
	const std::vector<smse::LuaLib>& pluginLibs = luaInterface->getPluginLibraries();

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
				smse::log( "Installed library '%s'", lib.name.c_str() );
			else
				smse::logCol( smse::RED, "Failed to install library '%s'", lib.name.c_str() );
		}
	}

}

void smse::hooks::initPlaystate()
{
	_luaVM_loadLibraries.createHook( luaVM_loadLibraries_detour );
	_switchState.createHook( switchState_detour );
	_openStandardLibraries.createHook( openStandardLibraries_detour );

	arg::func_loader luaLoader{ L"lua51.dll" };
	lua::L_register = luaLoader.get<&luaL_register>( "luaL_register" );
	lua::_settop = luaLoader.get<&lua_settop>( "lua_settop" );
	lua::_settop = luaLoader.get<&lua_settop>( "lua_settop" );
	lua::L_loadstring = luaLoader.get<&luaL_loadstring>( "luaL_loadstring" );
	lua::_pcall = luaLoader.get<&lua_pcall>( "lua_pcall" );
	_popBearingFromLua.createHook( popBearingFromLua_detour );
}
