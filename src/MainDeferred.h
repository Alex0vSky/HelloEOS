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

		Synchronously::PresenceQueryable presence( platformHandle, auth.getAccount( ) );
		if ( !presence.setOnlineAndTitle( ) )
			return;

		char timeString[ 64 ];
		std::time_t t = std::time( nullptr );
		std::tm bt; localtime_s( &bt, &t );
		std::strftime( timeString, sizeof( timeString ), "%A %c", &bt );
#pragma endregion // prepare

		Deferred::QueueCommands::init( platformHandle );
		Deferred::Ctx ctx{ "CHAT", auth.getLocalUserId( ), platformHandle, mapping.getFriendLocalUserId( ) };
		Deferred::ConnectionRequestListener::AcceptEveryoneConnectionAware acceptEveryone( ctx );
		const uint8_t channelReceiving = 1;
		const uint8_t channelSending = 2;
		const uint8_t channelCommon = 3;
		const std::chrono::milliseconds sleep{ 100 };
		const size_t vectorSize = ( EOS_P2P_MAX_PACKET_SIZE + 1 );

		if ( isServer ) {
			LOG( "[~] server" );
////			Deferred::Receiving receiving( ctx, acceptEveryone );
////			auto len = strlen( "PING" );
////			auto command = receiving.text( len );
////			////command ->act( ); // OR
////			//receiving.text( len ); // OR
//
//			Deferred::Receiving receiving( ctx, channelReceiving, acceptEveryone );
//			Deferred::Sending sending( ctx, channelSending );
//			while ( true ) {
//				receiving.text( strlen( "PING" ) );
//				auto incomingData = Deferred::QueueCommands::instance( ).ticksAll( );
//				const auto &packet = incomingData[ 0 ];
//				std::string string( packet.begin( ), packet.end( ) );
//				LOG( "[>>] '%s'", string.c_str( ) );
//				if ( string.compare( "PING" ) ) 
//					throw std::runtime_error( "ping-pong mismatch" );
//				LOG( "[<<] 'PONG'" );
//				sending.text( "PONG" );
//				Deferred::QueueCommands::instance( ).ticksAll( );
//				LOG( "[~] sleep" );
//				std::this_thread::sleep_for( sleep );
//			}
//
//			Deferred::PingPonger pingPonger( ctx );
//			pingPonger.infinitePonger( );

			Deferred::Receiving receiving( ctx, channelCommon, acceptEveryone );
			receiving.vector( vectorSize );
			auto incomingData = Deferred::QueueCommands::instance( ).ticksAll( );
			const auto &packet = incomingData[ 0 ];
			LOG( "[>>%zd] vector", packet.size( ) );
			size_t i = 0;
			for ( ; i < packet.size( ); ++i ) {
				auto value = static_cast< Networking::messageData_t::value_type >( i );
				// Simple sequence checking
				if ( packet[ i ] != value ) {
					LOG( "[~] wrong, 'i' is %zd, but to be %zd", i, value );
					break;
				}
			}
			LOG( "[~] checking: %s", ( ( i >= packet.size( ) ) ?"true" :"false" ) );

		} else {
			LOG( "[~] client" );
////			Deferred::Sending sending( ctx );
////			auto command = sending.text( "PING" );
////			//// Second
////			//command ->act( );
//
//			std::swap( channelSending, channelReceiving );
//			Deferred::Sending sending( ctx, channelSending );
//			Deferred::Receiving receiving( ctx, channelReceiving, acceptEveryone );
//			while ( true ) {
//				LOG( "[<<] 'PING'" );
//				sending.text( "PING" );
//				receiving.text( strlen( "PONG" ) );
//				auto incomingData = Deferred::QueueCommands::instance( ).ticksAll( );
//				const auto &packet = incomingData[ 0 ];
//				std::string string( packet.begin( ), packet.end( ) );
//				LOG( "[>>] '%s'", string.c_str( ) );
//				if ( string.compare( "PONG" ) ) 
//					throw std::runtime_error( "ping-pong mismatch" );
//				LOG( "[~] sleep" );
//				std::this_thread::sleep_for( sleep );
//			}
//
//			std::chrono::seconds sleep{ 1 };
//			Deferred::PingPonger pingPonger( ctx );
//			LOG( "[~] press [Ctrl+C] to exit" );
//			while( true ) {
//				auto milli = pingPonger.pingerMeasure( );
//				LOG( "Reply from 'Epic-provided relay': bytes=4, time=%lldms", milli.count( ) );
//				std::this_thread::sleep_for( sleep );
//			}

			LOG( "[~] wait server..." );
			// +TODO(alex): presence
			//	[LogEOSPresence] Received presence update. LocalUserId=[34c...827] PresenceUserId=[fbd...fb1] RichText=[Using P2P NAT]
			const EOS_Presence_EStatus statusInitial = static_cast< EOS_Presence_EStatus >( -1 );
			EOS_Presence_EStatus statusPrev = statusInitial;
			EOS_Presence_EStatus statusCur = statusInitial;
			while ( true ) {
				statusPrev = statusCur;
				if ( !presence.query( allFriends[ 0 ].UserId, &statusCur ) )
					return;
				__nop( );
				if ( statusInitial != statusPrev && statusPrev != statusCur ) {
					LOG( "[~] server status changed" );
				}
				if ( statusCur == EOS_Presence_EStatus::EOS_PS_Online )  {
					bool isEstablished = acceptEveryone.isEstablished( ); // only after first data exchange
					LOG( "[~] server is online" );
					Deferred::Sending sending( ctx, channelCommon );
					Networking::messageData_t vector( vectorSize );
					for ( size_t i = 0; i < vector.size( ); ++i ) {
						auto value = static_cast< Networking::messageData_t::value_type >( i );
						vector[ i ] = value;
					}
					sending.vector( vector );
					LOG( "[<<%zd] vector", vector.size( ) );
					Deferred::QueueCommands::instance( ).ticksAll( );
					
					// TODO(alex): LOG( "[~] press [Enter] to break sleep right now" ); // breakableSleep
					LOG( "[~] sleep to other iteration..." );
					std::this_thread::sleep_for( std::chrono::seconds{ 60 } );
					continue;
				}
				//LOG( "[~] tick" );
				::EOS_Platform_Tick( platformHandle );
				std::this_thread::sleep_for( std::chrono::seconds{ 1 } );
			}
		}
		LOG( "[~] press [Enter] to exit" );
		getchar( );
		return;
	}
};
} // namespace syscross::HelloEOS
