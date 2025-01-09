
#include <console/console.h>

extern "C" {

	const char* g_pluginName = "Template Plugin";

	void SMSEPluginLoad()
	{

		smse::log( "Loaded '%s'", g_pluginName );

	}

}