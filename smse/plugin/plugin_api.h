#pragma once

#include <lua/lua.hpp>
#include <string>
#include <vector>

#include <util/singleton.h>

namespace smse
{

enum LuaLibEnvironment : uint32_t
{
	LUA_LIB_ENV_GAME = 0,
	LUA_LIB_ENV_TERRAIN
};

struct LuaLib
{
	std::string name;
	const luaL_Reg* lib;
	LuaLibEnvironment env;
};

class LuaInterface
{
public:
	void registerLib( const std::string& _libName, const luaL_Reg* _lib, LuaLibEnvironment _environment = LUA_LIB_ENV_GAME ) {
		m_userLibs.push_back( { _libName, _lib, _environment } );
	}

	const std::vector<LuaLib>& getPluginLibraries() const { 
		return m_userLibs; 
	}

private:

	std::vector<LuaLib> m_userLibs;
};


class SMSEInterface : public Singleton<SMSEInterface>
{
public:
	LuaInterface luaInterface;
};

struct PluginInfo
{
	std::string name;
};

typedef bool ( *PluginOnInstallFptr_t )( const SMSEInterface* _pInterface, PluginInfo* _pInfo );
typedef bool ( *PluginOnLoadFptr_t )( const SMSEInterface* _pInterface );

}