// HelloEOS.cpp - entry point
// Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
#include "stdafx.h"
//#include "MainSynchronously.h"
//#include "MainAnchronously.h"
#include "MainDeferred.h"

int main(int argc, char *argv[]) {
#ifdef _DEBUG
    HeapSetInformation( NULL, HeapEnableTerminationOnCorruption, NULL, NULL );
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	//syscross::HelloEOS::MainSynchronously main;
	//syscross::HelloEOS::MainAnchronously main;
	syscross::HelloEOS::MainDeferred main;
	main.run( argc );
	return 0;
}
