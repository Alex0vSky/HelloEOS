// src\MainDeferred.h - code main class, for unittests code coverage
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
//#include "eos_auth.h"
//#include "eos_sdk.h"
//#include "eos_types.h"
//#include "Windows/eos_Windows.h"
//#include "eos_logging.h"
//#include "eos_p2p.h"
//#include "eos_friends.h"
//#include "eos_presence.h"

//#include <iostream>
//#include <atomic>
//#include <thread>
//#include <vector>
//#include <future>
//#include <coroutine>
//#include <chrono>

#include "Version.h"
#include "Log.h"
#include "HumanReadable.h"
#include "Credentials/Hardcode.h"
#include "InitializeEOS.h"
#include "Networking.h"
#include "Synchronously/Auth.h"
#include "Synchronously/Friend.h"
#include "Synchronously/AccountMapping.h"
#include "Synchronously/Presence.h"
// TODO(alex): removeme
#include "Synchronously/Send/BaseSend.h"
#include "Deferred/Ctx.h"
#include "Deferred/ConnectionRequestListener/AcceptEveryone.h"
#include "Deferred/QueueCommands.h"
#include "Deferred/Action.h"
#include "Deferred/Sender/Text.h"
#include "Deferred/Receiver/Text.h"
#include "Deferred/Sending.h"
#include "Deferred/Receiving.h"

namespace syscross::HelloEOS { struct MainDeferred {
	void run(int argc) {
		bool isServer = ( argc > 1 );
#pragma region prepare
		InitializeEOS init;
		EOS_HPlatform platformHandle = init.initialize( );
		if ( !platformHandle )
			return;

		Synchronously::Auth auth( platformHandle );
		std::string tokenDevAuthToolAuth;
		if ( isServer )
			tokenDevAuthToolAuth = "cred2";
		else
			tokenDevAuthToolAuth = "cred1";
		if ( !auth.connectAndLogin( tokenDevAuthToolAuth ) )
			return;
		LOG( "[~] auth.getLocalUserId( ) valid: %s", ( ::EOS_ProductUserId_IsValid( auth.getLocalUserId( ) ) ?"TRUE" :"FALSE" ) );

		Synchronously::Friend friends( platformHandle, auth.getAccount( ) );
		auto allFriends = friends.getAll( );
		if ( allFriends.empty( ) )
			return;

		Synchronously::AccountMapping mapping( platformHandle, auth.getLocalUserId( ), allFriends );
		if ( !mapping.getFirstFriendId( ) )
			return;
		LOG( "[~] mapping.getFriendLocalUserId( ) valid: %s", ( ::EOS_ProductUserId_IsValid( mapping.getFriendLocalUserId( ) ) ?"TRUE" :"FALSE" ) );

		Synchronously::Presence presence( platformHandle, auth.getAccount( ) );
		if ( !presence.setOnlineAndTitle( ) )
			return;

		char timeString[ 64 ];
		std::time_t t = std::time( nullptr );
		std::strftime( timeString, sizeof( timeString ), "%A %c", std::localtime( &t ) );
#pragma endregion // prepare

		Deferred::QueueCommands::init( platformHandle );
		Deferred::Ctx ctx{ "CHAT", platformHandle, auth.getLocalUserId( ), mapping.getFriendLocalUserId( ) };
		Deferred::ConnectionRequestListener::AcceptEveryone acceptEveryone( ctx );
		if ( isServer ) {
			LOG( "[~] server" );
			Deferred::Receiving receiving( ctx, acceptEveryone );
			auto len = strlen( "PING" );
			auto command = receiving.text( len );
			//command ->act( ); // OR
			receiving.text( len ); // OR

		} else {
			LOG( "[~] client" );
			
			Deferred::Sending sending( ctx );
			auto command = sending.text( "PING" );
			// Second
			command ->act( );
		}
		auto vector = Deferred::QueueCommands::instance( ).ticksAll( );
		LOG( "[~] press any key to exit" );
		getchar( );
		return;
	}
};
} // namespace syscross::HelloEOS
