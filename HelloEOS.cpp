// HelloEOS.cpp - entry point
// Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
#include "stdafx.h"
#include "Main.h"

int main(int argc, char *argv[]) {
#ifdef _DEBUG
    HeapSetInformation( NULL, HeapEnableTerminationOnCorruption, NULL, NULL );
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	syscross::HelloEOS::Main main;
	main.run( argc );
	return 0;
}
