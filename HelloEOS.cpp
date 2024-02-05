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
	// for `boost::system::error_code::message` language
	setlocale( 0, "" );
	(void)argv;
	//syscross::HelloEOS::MainSynchronously main;
	//syscross::HelloEOS::MainAnchronously main;
	//syscross::HelloEOS::MainDeferred main;
	//syscross::HelloEOS::Main_gRpc main;
	syscross::HelloEOS::MainAsynchronously main;
	try {
		main.run( argc );
	} 
#ifdef A0S_BENCH_P2P
	// @insp https://stackoverflow.com/questions/20283059/boostexception-how-to-print-details
	// TODO(alex): #include <boost/stacktrace.hpp>
	catch (boost::exception &exc) {
		std::string info = boost::diagnostic_information( exc );
		LOG( "[E] boost::diagnostic_information = '%s'", info.c_str( ) );
		LOG( "[~] press [Enter] to exit" );
		getchar( );
	}
#endif // A0S_BENCH_P2P
	catch (std::exception &exc) {
		LOG( "[E] std::exception::what = '%s'", exc.what( ) );
		LOG( "[~] press [Enter] to exit" );
		getchar( );
	}
	int n = 1'000;
	return 0;
}
