/**
 * @file LuaVM.h
 * @brief Auto Generated
 * 
 */

#include <lua/lua.hpp>

namespace sm
{

struct LuaVM 
{ 
	lua_State* m_luaState;
};

typedef int ( *fp_luaopenlib_t )( LuaVM* );

}