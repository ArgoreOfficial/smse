#include "plugin_manager.h"

#include <console/console.h>
#include <sm/sm_api.h>

#include <iterator>
#include <fstream>

void smse::PluginManager::init()
{
	#ifdef SMSE_DEBUG
	std::ifstream     file( "D:\\Dev\\smse\\bin\\Plugins\\_SMSELoadOrder.ini" );
	#else
	std::ifstream     file( "D:\\Dev\\smse\\bin\\Plugins\\_SMSELoadOrder.ini" );
	#endif

	if ( file.is_open() )
	{
		std::istream_iterator<std::string> fileItr( file );
		std::istream_iterator<std::string> endItr;

		while ( fileItr != endItr )
		{
			smse::log( "Installing '%s'", fileItr->c_str() );
			SMSEPlugin plugin = tryInstallPlugin( *fileItr );
			if ( plugin.hModule )
				m_plugins.push_back( plugin );

			++fileItr;
		}

		file.close();
	}
	else
	{
		smse::logCol( smse::RED, "!ERROR! Failed to locate Plugins/_SMSELoadOrder.ini" );
	}

	m_hasInstalledPlugins = true;

	for ( auto& plugin : m_plugins )
	{
		bool loaded = plugin.fpOnLoad( smse::SMSEInterface::getInstancePtr() );
		if ( loaded == false )
			smse::logCol( smse::RED, " !ERROR! %s::SMSEPlugin_OnLoad returned false`", plugin.info.name );
	}

}

const smse::PluginInfo* smse::PluginManager::getPluginInfo( const std::string& _name )
{
	if ( !m_hasInstalledPlugins )
		return nullptr;

	for ( size_t i = 0; i < m_plugins.size(); i++ )
	{
		if ( m_plugins[ i ].info.name == _name )
			return &m_plugins[ i ].info;
	}

	return nullptr;
}

smse::SMSEPlugin smse::PluginManager::tryInstallPlugin( const std::string& _name )
{
	SMSEPlugin plugin;

	std::string path = "D:/Dev/smse/bin/Plugins/" + _name + ".dll";
	plugin.hModule = LoadLibraryA( path.c_str() );

	if ( plugin.hModule == NULL )
	{
		smse::logCol( smse::RED, " ERROR! loading %s : Failed to load %s.dll\n(%s)", _name.c_str(), _name.c_str(), path.c_str() );
		return { 0 };
	}

	plugin.fpOnInstall = reinterpret_cast<PluginOnInstallFptr_t>( GetProcAddress( plugin.hModule, "SMSEPlugin_OnInstall" ) );
	plugin.fpOnLoad    = reinterpret_cast<PluginOnLoadFptr_t>   ( GetProcAddress( plugin.hModule, "SMSEPlugin_OnLoad" ) );

	if ( plugin.fpOnInstall == nullptr )
	{
		smse::logCol( smse::RED, " !ERROR! loading %s : Could not locate `bool SMSEPlugin_OnInstall( const smse::SMSEInterface*, smse::PluginInfo* )`", _name.c_str() );
		return { 0 };
	}

	if ( plugin.fpOnLoad == nullptr )
	{
		smse::logCol( smse::RED, " !ERROR! loading %s : Could not locate `bool SMSEPlugin_OnLoad( const smse::SMSEInterface* )`", _name.c_str() );
		return { 0 };
	}

	bool installed = plugin.fpOnInstall( smse::SMSEInterface::getInstancePtr(), &plugin.info );
	if ( !installed )
	{
		smse::logCol( smse::RED, " !ERROR! loading %s : %s::SMSEPlugin_OnInstall returned false`", _name.c_str(), plugin.info.name );
		return { 0 };
	}

	smse::logCol( LogColor::GREEN, "Installed '%s' (%s.dll)", plugin.info.name, _name.c_str() );

	return plugin;
	
}
