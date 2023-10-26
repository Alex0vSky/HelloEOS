// src\Main.h - code main class, for unittests code coverage
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
#include "eos_auth.h"
#include "eos_sdk.h"
#include "eos_types.h"
#include "Windows/eos_Windows.h"
#include "eos_logging.h"
#include "eos_p2p.h"
#include "eos_friends.h"
#include "eos_presence.h"

#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

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
#include "Synchronously\Receive\Chat.h"
#include "Synchronously\Receive\Bandwidth.h"

namespace syscross::HelloEOS {
struct Main {
	void run(int argc) {
		bool isServer = ( argc > 1 );

		InitializeEOS init;
		if ( !init.doit( ) )
			return;
		EOS_HPlatform platformHandle = init.getPlatformHandle( );
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

		if ( isServer ) {
			LOG( "[~] server" );

//			Synchronously::Receive::Chat chat( platformHandle, auth.getLocalUserId( ) );
//			std::string message = chat.getMessage( );
//			LOG( "[~] message: %s", message.c_str( ) );

			Synchronously::Receive::Bandwidth bandwith( platformHandle, auth.getLocalUserId( ) );
			if ( !bandwith.recvAndCheck( ) )
				return;

		} else {
			LOG( "[~] client" );

//			Synchronously::Send::Chat chat( platformHandle, auth.getLocalUserId( ), mapping.getFriendLocalUserId( ) );
//			char timeString[ 64 ];
//			std::time_t t = std::time( nullptr );
//			std::strftime( timeString, sizeof( timeString ), "%A %c", std::localtime( &t ) );
//			std::string message = timeString;
//			if ( !chat.message( message ) )
//				return;

			Synchronously::Send::Bandwidth bandwith( platformHandle, auth.getLocalUserId( ), mapping.getFriendLocalUserId( ) );
			size_t Bandwith;
			if ( !bandwith.measure( &Bandwith ) )
				return;

		}
		LOG( "[~] press any key to exit" );
		getchar( );
		return;
	}
};
} // namespace syscross::HelloEOS
