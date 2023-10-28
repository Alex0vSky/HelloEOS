// src\Main.h - code main class, for unittests code coverage
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
#include "Credentials\Hardcode.h"
#include "InitializeEOS.h"
#include "Networking.h"
#include "Synchronously\Auth.h"
#include "Synchronously\Friend.h"
#include "Synchronously\AccountMapping.h"
#include "Synchronously\Presence.h"
#include "Synchronously\Receive\BaseReceive.h"
#include "Synchronously\Send\BaseSend.h"
#include "Synchronously\Send\Chat.h"
#include "Synchronously\Send\Bandwidth.h"
#include "Synchronously\Send\PingPong.h"
#include "Synchronously\Receive\Chat.h"
#include "Synchronously\Receive\Bandwidth.h"
#include "Synchronously\Receive\PingPong.h"
#include "Anchronously\Send\Chat.h"
#include "Anchronously\Receive\Chat.h"
#include "Anchronously\Ping.h"

namespace syscross::HelloEOS { struct Main {
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

		if ( isServer ) {
			LOG( "[~] server" );

			Synchronously::Receive::Chat chat( platformHandle, auth.getLocalUserId( ) );
			std::string message = chat.getMessage( );
			LOG( "[~] message: '%s'", message.c_str( ) );

//			Synchronously::Receive::Bandwidth bandwith( platformHandle, auth.getLocalUserId( ) );
//			if ( !bandwith.recvAndCheck( ) )
//				return;

//			Synchronously::Receive::PingPong pingPong( platformHandle, auth.getLocalUserId( ), mapping.getFriendLocalUserId( ) );
//			if ( !pingPong.recvPingAndAnswerPong( ) )
//				return;

//			Anchronously::Receive::Chat chat( platformHandle, auth.getLocalUserId( ) );
//			Networking::recv_t future = chat.getMessage( );
//			do { 
//				::EOS_Platform_Tick( platformHandle );
//				std::future_status fStat = future.wait_for( std::chrono::milliseconds( 1 ) );
//				if ( std::future_status::timeout != fStat )
//					break;
//				std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } );
//			} while( true );
//			auto messageData = future.get( );
//			if ( messageData.empty( ) )
//				return;
//			std::string message( messageData.begin( ), messageData.end( ) );
//			LOG( "[~] message: '%s'", message.c_str( ) );

//			using namespace std::chrono_literals;
//			co_await 10s;

		} else {
			LOG( "[~] client" );

//			Synchronously::Send::Chat chat( platformHandle, auth.getLocalUserId( ), mapping.getFriendLocalUserId( ) );
//			std::string message = timeString;
//			if ( !chat.message( message ) )
//				return;

//			Synchronously::Send::Bandwidth bandwith( platformHandle, auth.getLocalUserId( ), mapping.getFriendLocalUserId( ) );
//			size_t Bandwith;
//			if ( !bandwith.measure( &Bandwith ) )
//				return;

//			Synchronously::Send::PingPong pingPong( platformHandle, auth.getLocalUserId( ), mapping.getFriendLocalUserId( ) );
//			if ( !pingPong.sendPingWaitPong( ) )
//				return;

			//std::vector< int > vec;
			std::deque< int > fifo;
			fifo.push_back( 1 );
			fifo.push_back( 2 );
			int n = fifo.front( );
			fifo.pop_front( );

			Anchronously::Send::Chat chat( platformHandle, auth.getLocalUserId( ), mapping.getFriendLocalUserId( ) );

			auto x = chat.message2( timeString );

			Networking::send_t future = chat.message( timeString );
			bool b = future.get( );
			if ( !b )
				return;
			LOG( "[~] press [Ctrl+C] to exit" );
			do { 
				::EOS_Platform_Tick( platformHandle );
				std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } );
			} while( true );

//			Anchronously::Ping ping( platformHandle, auth.getLocalUserId( ), mapping.getFriendLocalUserId( ) );
//			std::chrono::milliseconds duration;
//			Networking::ping_t future1 = ping.measure( &duration );
//			Networking::ping_t future2;
//			Networking::ping_t *pfuture = &future1;
//			do { 
//				::EOS_Platform_Tick( platformHandle );
//				std::future_status fStat = pfuture ->wait_for( std::chrono::milliseconds( 1 ) );
//				if ( std::future_status::timeout != fStat ) {
//					if ( pfuture == &future2 ) 
//						break;
//					// Second after warmup 
//					future2 = ping.measure( &duration );
//				}
//				std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } );
//			} while( true );
//			if ( !future2.get( ) )
//				return;
//			LOG( "[~] ping: %lld ms", duration.count( ) );

		}
		LOG( "[~] press any key to exit" );
		getchar( );
		return;
	}
};
} // namespace syscross::HelloEOS
