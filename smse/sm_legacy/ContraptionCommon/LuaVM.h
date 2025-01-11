/**
 * @file LuaVM.h
 * @brief Auto Generated
 * 
 */

#pragma once

#include <lua.hpp>

namespace sm
{

struct LuaVM 
{ 
	lua_State* m_luaState;

	//static void installLibraries( sm::LuaVM* _luaVM, void* _param_2, int _isTerrain );

};

typedef int ( *fp_luaopenlib_t )( LuaVM* );

}