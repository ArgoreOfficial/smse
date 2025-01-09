#include "luavm.h"

extern "C" {

static const struct luaL_Reg PTR_s_raycast_140f4c3b0[] = {
	{ NULL, NULL }  /* sentinel */
};

}

int sm::openPhysicsLibraryTest( LuaVM* _luaVM )
{
	int luaStack = lua_gettop( _luaVM->m_luaState );
	int luaStack2 = lua_gettop( _luaVM->m_luaState );
	luaL_register( _luaVM->m_luaState, "sm.physics", PTR_s_raycast_140f4c3b0 );
	lua_settop( _luaVM->m_luaState, 0xfffffffe );
	int luaStack3 = lua_gettop( _luaVM->m_luaState );
	if ( luaStack2 != luaStack3 )
	{
		//FUN_140260760();
		//FUN_140549f90( luaState2 );
	}
	//FUN_14054e4d0( _luaVM, "sm.physics", &PTR_s_types_140f4c380 );
	luaStack2 = lua_gettop( luaState );
	if ( luaStack != luaStack2 )
	{
		//FUN_140260760();
		//FUN_140549f90( luaState );
	}

	return 1;
}
