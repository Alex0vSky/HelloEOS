// HelloEOS.cpp - entry point // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
#include "stdafx.h"
//#include "MainSynchronously.h"
//#include "MainAnchronously.h"
//#include "MainDeferred.h"
//#include "Main_gRpc.h"
#include "MainAsynchronously.h"

int main(int argc, char *argv[]) {
#ifdef _DEBUG
    HeapSetInformation( NULL, HeapEnableTerminationOnCorruption, NULL, NULL );
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 166 );
	new char[]{ "Goobay!" };
#endif
	(void)argv;
	//syscross::HelloEOS::MainSynchronously main;
	//syscross::HelloEOS::MainAnchronously main;
	//syscross::HelloEOS::MainDeferred main;
	//syscross::HelloEOS::Main_gRpc main;
	syscross::HelloEOS::MainAsynchronously main;
	try {
		main.run( argc );
	} 
	catch (std::exception &exc) {
		LOG( "[~] std::exception::what = '%s'", exc.what( ) );
		LOG( "[~] press [Enter] to exit" );
		getchar( );
	}
	int n = 1'000;
	return 0;
}
