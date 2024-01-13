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
#include "Credentials/Hardcode.h"
#include "InitializeEOS.h"
#include "Networking.h"
#include "Synchronously/Auth.h"
#include "Synchronously/Friend.h"
#include "Synchronously/AccountMapping.h"
#include "Synchronously/Presence.h"
#include "Synchronously/PresenceQueryable.h"
#include "Deferred/Ctx.h"
#include "Deferred/ConnectionRequestListener/AcceptEveryone.h"
#include "Deferred/ConnectionRequestListener/AcceptEveryoneConnectionAware.h"
#include "Deferred/QueueCommands.h"
#include "Deferred/Action.h"
#include "Deferred/Sender/SendText.h"
#include "Deferred/Receiver/RecvText.h"
#include "Deferred/Sending.h"
#include "Deferred/Receiving.h"
#include "Deferred/PingPonger.h"
#ifdef _DEBUG
#	define A0S_SetThreadName( name ) ::SetThreadDescription( ::GetCurrentThread( ), L##name );
#else
#	define A0S_SetThreadName( name ) do while( false );
#endif
#include "Async/Environs.h"
#include "Async/PrepareEos.h"
#include "Async/Selector/IMultiplex.h"
#include "Async/Selector/Multiplexer.h"
#include "Async/GradualExecutor.h"
#include "Async/Acceptor.h"
#include "Async/Transport/Send.h"
#include "Async/Transport/Recv.h"
#include "Async/TickerCore.h"
#include "Async/Thread/GameThread.h"
#include "Async/Thread/JThread.h"
#include "Async/Thread/Factory.h"

namespace syscross::HelloEOS { struct MainAsynchronously {
	void run(int argc) {
		bool isServer = ( argc > 1 );

		auto oes = Async::Thread::FactoryInfiniteWait::gameThread( isServer );
		if ( !oes ) 
			return;
		Async::future_t command;

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
			Async::Transport::Send sendChat = oes ->createSender( socketNameChat );

			command = sendChat.text( text0 );
			command.wait( );
			LOG( "[<<%zd] text: '%s'", text0.size( ), text0.c_str( ) );
			command = sendChat.text( text1 );
			command.wait( );
			LOG( "[<<%zd] text: '%s'", text1.size( ), text1.c_str( ) );

			Async::Transport::Send sendData = oes ->createSender( socketNameData );
			Networking::messageData_t vector( vectorSize );
			for ( size_t i = 0; i < vector.size( ); ++i ) {
				auto value = static_cast< Networking::messageData_t::value_type >( i );
				vector[ i ] = value;
			}
			command = sendData.vector( vector );
			command.wait( );
			LOG( "[<<%zd] vector", vector.size( ) );
		}

		LOG( "[~] press [Enter] to exit" );
		getchar( );
		return;
	}
};
} // namespace syscross::HelloEOS
