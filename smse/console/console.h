#pragma once

#include <Windows.h>
#include <stdio.h>

namespace smse
{

enum LogColor
{
	DARK_BLUE = 1,
	DARK_GREEN,
	DARK_CYAN,
	DARK_RED,
	PURPLE,
	GOLD,
	LIGHT_GRAY,
	DARK_GRAY,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	YELLOW,
	WHITE
};


class Console
{
public:

	void alloc();

	void open();
	void close();

	void setFgColor( int _color ) {
		if ( !m_hConsole )
			m_hConsole = GetStdHandle( STD_OUTPUT_HANDLE );

		if ( _color < 0 )
			_color = 0; // default color

		SetConsoleTextAttribute( m_hConsole, _color );
	}

	template<typename... _Args>
	void log( const char* _format, _Args... _args ) {
		#ifdef SMSE_CORE
		printf( "[SMSE] " );
		#else
		printf( "[SMSEPlugin] " );
		#endif
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

template<typename... Args>
inline void logCol( int _color, const char* _format, Args... _args )
{
	g_console.setFgColor( _color );
	g_console.log( _format, _args... );
	g_console.setFgColor( LogColor::LIGHT_GRAY );
}

}
