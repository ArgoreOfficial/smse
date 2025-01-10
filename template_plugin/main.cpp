
#include <plugin/plugin_api.h>
#include <console/console.h>

extern "C" {

	const char* g_pluginName = "Template Plugin";

	bool SMSEPluginInstall( const smse::SMSEInterface* _pInterface, smse::PluginInfo* _pInfo )
	{
		_pInfo->name = g_pluginName;

		return true;
	}

	bool SMSEPluginLoad( const smse::SMSEInterface* _pInterface )
	{
		smse::log( "Loaded '%s'", g_pluginName );

		return true;
	}

}