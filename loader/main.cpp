

#include <Windows.h>

#include <string>
#include <stdio.h>


#define smseAssert( _v, ... ) if ( _v ) { \
	printf( __VA_ARGS__ ); \
	exit( 1 ); \
} \

struct sApplication
{
	HANDLE hProcess{0};
	HANDLE hThread{0};
};

sApplication launch( std::string _exePath, std::string _dirPath )
{
	PROCESS_INFORMATION	procInfo{ 0 };
	STARTUPINFO startupInfo{ 0 };
	startupInfo.cb = sizeof( startupInfo );

	std::string cmd = "\"" + _exePath + "\" -dev";

	if ( !CreateProcessA(
		_exePath.c_str(),
		(LPSTR)cmd.c_str(),
		NULL, 
		NULL, 
		FALSE,
		CREATE_SUSPENDED,
		NULL, 
		_dirPath.c_str(),
		&startupInfo, &procInfo ) )
	{
		return {};
	}

	sApplication app;
	app.hProcess = procInfo.hProcess;
	app.hThread  = procInfo.hThread;
	return app;
}

HANDLE injectDll( sApplication _app, const char* _dllpath )
{
	printf( "Attempting to inject %s", _dllpath );

	LPVOID p = VirtualAllocEx( _app.hProcess, nullptr, 1 << 12, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	smseAssert( !p, "Failed to call VirtualAllocEx\n" );
	printf( "Allocated memory at [%llu]\n", (uint64_t)p );

	WriteProcessMemory( _app.hProcess, p, _dllpath, strlen( _dllpath ) + 1, nullptr );
	printf( "Wrote dll path to process\n" );

	HMODULE hKernel32dll = GetModuleHandleW( L"kernel32.dll" );
	smseAssert( !hKernel32dll, "failed to get kernel32.dll handle\n" );
	printf( "Loaded kernel32.dll\n" );

	LPTHREAD_START_ROUTINE loadProcAddr = (LPTHREAD_START_ROUTINE)GetProcAddress( hKernel32dll, "LoadLibraryA" );
	printf( "Loaded LoadLibraryA [%llu]\n", (uint64_t)loadProcAddr );

	HANDLE hDllThread = CreateRemoteThread( _app.hProcess, nullptr, 0, loadProcAddr, p, 0, nullptr );
	smseAssert( !hDllThread, "Failed to create remote thread\n" );
	printf( "Created remote thread [%llu]\n", (uint64_t)hDllThread );

	return hDllThread;
}

int main()
{
	int pid = 18752; // process ID

	const char* exePath = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Scrap Mechanic\\Release\\ScrapMechanic.exe";
	const char* dirPath = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Scrap Mechanic\\Release\\";

	sApplication app = launch( exePath, dirPath );
	
	if ( !app.hProcess )
		return 1;
	
	HANDLE hSMSEThread = injectDll( app, "D:\\Dev\\smse\\bin\\smse_dbg.dll" );
	
	Sleep( 500 );
	ResumeThread( app.hThread );
	
	CloseHandle( hSMSEThread );

	return 0;
}