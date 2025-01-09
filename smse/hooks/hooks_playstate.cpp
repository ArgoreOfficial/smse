#include "hooks_playstate.h"

#include <sm_legacy/ContraptionCommon/LuaVM.h>

#include <hooks/detour.h>
#include <hooks/reloc.h>

#include <vector>

/// TODO: figure out params
void initLuaManager_detour( void* _this, void* _param_2, void* _param_3, sm::fp_luaopenlib_t** _loadFuncs );
static smse::sFuncDetour<initLuaManager_detour> _initLuaManager{ 0x6a6f00 };

void switchState_detour( void* _this, int _state );
static smse::sFuncDetour<switchState_detour> _switchState{ 0x2da450 };

extern "C" {

	static int print( lua_State* _luaState )
	{
		smse::log( "SMSE Print" );
		return 1;
	}

	static const struct luaL_Reg smseLib[] = {
		{ NULL, NULL }  /* sentinel */
	};

}

int openSMSELibrary( sm::LuaVM* _luaVM )
{
	smse::log( "Loading SMSE Functions" );
	/*
	int luaStack = lua_gettop( _luaVM->m_luaState );
	int luaStack2 = lua_gettop( _luaVM->m_luaState );
	luaL_register( _luaVM->m_luaState, "smse", smseLib );
	lua_settop( _luaVM->m_luaState, 0xfffffffe );
	int luaStack3 = lua_gettop( _luaVM->m_luaState );
	if ( luaStack2 != luaStack3 )
	{
		//FUN_140260760();
		//FUN_140549f90( luaState2 );
	}
	//FUN_14054e4d0( _luaVM, "sm.physics", &PTR_s_types_140f4c380 );
	luaStack2 = lua_gettop( _luaVM->m_luaState );
	if ( luaStack != luaStack2 )
	{
		//FUN_140260760();
		//FUN_140549f90( luaState );
	}
	*/

	return 1;
}

static int coolTestFunc( sm::LuaVM* _luaVM )
{
	smse::log( "This is cool and awesome" );
	return 1;
}

sm::fp_luaopenlib_t coolLib[] = {
	coolTestFunc
};

void initLuaManager_detour( void* _this, void* _param_2, void* _param_3, sm::fp_luaopenlib_t** _loadFuncs )
{
	std::vector<sm::fp_luaopenlib_t*> testLoadFuncs{};
	int n = 0;
	while ( _loadFuncs[ n ] != nullptr )
		testLoadFuncs.push_back( _loadFuncs[ n ] ), n++;
	
	/* push plugin libs */
	testLoadFuncs.push_back( coolLib );

	testLoadFuncs.push_back( nullptr );

	smse::log( "Loading %i Library Modules", testLoadFuncs.size() -  1 );
	_initLuaManager.original( _this, _param_2, _param_3, testLoadFuncs.data() );
}

void switchState_detour( void* _this, int _state )
{
	smse::log( "Switch State [%p][%p] to %i", _this, (uintptr_t)_this - smse::getBaseAddr(), _state);

	_switchState.original( _this, _state );
}

void smse::hooks::initPlaystate()
{
	_initLuaManager.createHook( initLuaManager_detour );
	_switchState.createHook( switchState_detour );
}
