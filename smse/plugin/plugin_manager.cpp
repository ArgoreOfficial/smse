#include "plugin_manager.h"

void smse::PluginManager::init()
{
	/* install plugins */

	m_hasInstalledPlugins = true;

	/* load plugins */

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
