#pragma once

#include <lua/lua.hpp>

namespace sm
{
	struct LuaVM
	{
		lua_State* m_luaState;
	};

	int openPhysicsLibraryTest( LuaVM* _luaVM );
}