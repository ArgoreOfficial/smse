#pragma once

#include <plugin/plugin_api.h>
#include <util/singleton.h>

#include <vector>
#include <string>

#include <windows.h>

namespace smse
{

struct SMSEPlugin
{
	HMODULE hModule;
	PluginInfo info;
	PluginOnInstallFptr_t fpOnInstall;
	PluginOnLoadFptr_t fpOnLoad;
};

class PluginManager : public Singleton<PluginManager>
{
public:
	PluginManager() { }

	void init();

	const PluginInfo* getPluginInfo( const std::string& _name );
	
private:

	bool m_hasInstalledPlugins = false;
	std::vector<SMSEPlugin> m_plugins;
};

}