
#include <plugin/plugin_api.h>
#include <console/console.h>
#include <lua.hpp>

extern "C" {

	const char* g_pluginName = "Template Plugin";

	static int coolFunc( lua_State* _luaState )
	{
		smse::log( "Cool plugin function was called" );
		return 1;
	}

	static const struct luaL_Reg _pluginLib[] = {
		{ "coolFunc", coolFunc },
		{ NULL, NULL }  /* sentinel */
	};

	SMSEAPI bool SMSEPlugin_OnInstall( const smse::SMSEInterface* _pInterface, smse::PluginInfo* _pInfo )
	{
		_pInfo->name = g_pluginName;

		return true;
	}

	SMSEAPI bool SMSEPlugin_OnLoad( smse::SMSEInterface* _pInterface )
	{
		smse::LuaInterface* luaInterface = _pInterface->getLuaInterface();
		luaInterface->registerLib( "smseplugin", _pluginLib );

		smse::log(
			"Loaded template plugin\n"
			"/*\n"
			" * This is an example plugin\n"
			" * Lua Functions:\n"
			" *  - smseplugin.coolFunc()\n"
			" **/\n"
		);

		return true;
	}

}