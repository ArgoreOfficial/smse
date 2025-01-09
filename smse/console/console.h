#pragma once

#include <Windows.h>
#include <stdio.h>

namespace smse
{

class Console
{
public:

	void alloc();

	void open();
	void close();

	template<typename... _Args>
	void log( const char* _format, _Args... _args ) {
		printf( "[SMSE] " );
		printf( _format, _args... );
		printf( "\n" );
	}

private:
	FILE*  m_handle   = NULL;
	HANDLE m_hConsole = NULL;
};

static Console g_console;

template<typename... _Args>
void log( const char* _format, _Args... _args ) {
	g_console.log( _format, _args... );
}

}
