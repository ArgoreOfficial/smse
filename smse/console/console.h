#pragma once

#include <stdio.h>


namespace smse
{
	void _logAlloc();

	void _logOpen();
	void _logClose();

	template<typename... _Args>
	void log( const char* _format, _Args... _args );

	template<typename ..._Args>
	inline void log( const char* _format, _Args ..._args )
	{
		printf( "[SMSE] " );
		printf( _format, _args... );
		printf( "\n" );
	}
}
