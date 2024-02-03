// src\MainAsynchronously.h - code main class, for unittests code coverage
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
#if __has_include( "Credentials/Private.h" )
#	include "Credentials/Private.h"
#else
#	pragma message( "Please insert in 'Hardcode.h' content from \
your own 'dev.epicgames.com/portal', or there will be '5 unresolved externals'" )
#	include "Credentials/Hardcode.h"
#endif
#include "InitializeEOS.h"
#include "Networking.h"
#include "Synchronously/Auth.h"
#include "Synchronously/Friend.h"
#include "Synchronously/AccountMapping.h"
#include "Synchronously/Presence.h"
#include "Synchronously/PresenceQueryable.h"
#ifdef _DEBUG
#	define A0S_SetThreadName( name ) ::SetThreadDescription( ::GetCurrentThread( ), L##name );
#else
#	define A0S_SetThreadName( name ) do while( false );
#endif
#include "Async/EosContext.h"
#include "Async/PrepareEos.h"
#include "Async/Selector/IMultiplex.h"
#include "Async/Selector/Multiplexer.h"
#include "Async/GradualExecutor.h"
#include "Async/Acceptor.h"
#ifdef A0S_BENCH_P2P
#include "Async/_Bench/Transport.h"
#else // A0S_BENCH_P2P
#include "Async/Transport/Sender.h"
#include "Async/Transport/Recv.h"
#endif // A0S_BENCH_P2P
#include "Async/TickerCore.h"
#include "Async/Thread/GameThread.h"
#include "Async/Thread/JThread.h"
#ifdef A0S_BENCH_P2P
#include <src/aliases.h>
#include <src/Command.h>
#include "Async/_Bench/Thread/Factory.h"
#else // A0S_BENCH_P2P
#include "Async/Thread/Factory.h"
#endif // A0S_BENCH_P2P

namespace syscross::HelloEOS { struct MainAsynchronously {
	void run(int argc) {
		bool isServer = ( argc > 1 );

		auto oes = Async::Thread::Factory::create( isServer );
		if ( !oes ) 
			return;

		//while ( true ) {
		//	std::this_thread::sleep_for( std::chrono::milliseconds{ 300 } );
		//}

		Async::command_t command;

		auto socketNameChat = "CHAT";
		auto socketNameData = "DATA";
		std::string text0 = "Hello ";
		std::string text1 = "World!";

		const size_t vectorSize = ( EOS_P2P_MAX_PACKET_SIZE + 1 );

		if ( isServer ) {
			LOG( "[~] server" );
			// Here and in clients back order, or "[LogEOSP2P] Received connection invitation request for unknown socket. LocalUserId=[000...7e5] RemoteUserId=[000...6e2] SocketId=[DATA]"
			Async::Transport::Recv recvData = oes ->createReceiver( socketNameData );
			Async::Transport::Recv recvChat = oes ->createReceiver( socketNameChat );
			Networking::messageData_t incoming;
			std::string text;

			command = recvChat.byLength( text0.length( ) );
			incoming = command.get( );
			text.assign( incoming.begin( ), incoming.end( ) );
			LOG( "[>>%zd] text: '%s'", incoming.size( ), text.c_str( ) );

			command = recvChat.byLength( text1.length( ) );
			incoming = command.get( );
			text.assign( incoming.begin( ), incoming.end( ) );
			LOG( "[>>%zd] text: '%s'", incoming.size( ), text.c_str( ) );

			command = recvData.byLength( vectorSize );
			incoming = command.get( );
			LOG( "[>>%zd] vector", incoming.size( ) );
			size_t i = 0;
			for ( ; i < incoming.size( ); ++i ) {
				auto value = static_cast< Networking::messageData_t::value_type >( i );
				if ( incoming[ i ] != value ) {
					LOG( "[~] wrong, 'i' is %zd, but to be %d", i, value );
					break;
				}
			}
			LOG( "[~] checking: %s", ( i && ( i >= incoming.size( ) ) ?"true" :"false" ) );
		} else {
			LOG( "[~] client" );
			Async::Transport::Sender sendChat = oes ->createSender( socketNameChat );

			command = sendChat.sendText( text0 );
			command.wait( );
			LOG( "[<<%zd] text: '%s'", text0.size( ), text0.c_str( ) );
			command = sendChat.sendText( text1 );
			command.wait( );
			LOG( "[<<%zd] text: '%s'", text1.size( ), text1.c_str( ) );

			Async::Transport::Sender sendData = oes ->createSender( socketNameData );
			Networking::messageData_t vector( vectorSize );
			for ( size_t i = 0; i < vector.size( ); ++i ) {
				auto value = static_cast< Networking::messageData_t::value_type >( i );
				vector[ i ] = value;
			}
			command = sendData.sendVector( vector );
			command.wait( );
			LOG( "[<<%zd] vector", vector.size( ) );
		}

		LOG( "[~] press [Enter] to exit" );
		getchar( );
		return;
	}
};
} // namespace syscross::HelloEOS
