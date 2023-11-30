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
namespace syscross::HelloEOS::Async {
using namespace std::literals::chrono_literals;
typedef std::future< Networking::messageData_t > future_t;
// POD/Aggregation
struct Context {
	const EOS_HPlatform m_platformHandle;
	EOS_ProductUserId const m_localUserId, m_friendLocalUserId;
};
} // namespace syscross::HelloEOS::Async
#include "Async/PrepareEos.h"
#include "Async/SuperClass.h"
#include "Async/IMultiplex.h"
#include "Async/Multiplexer.h"
#include "Async/GradualExecutor.h"
#include "Async/Transport/Send.h"
#include "Async/Transport/Recv.h"
#include "Async/SuperclassTicker.h"
#include "Async/Thread/Ticker.h"
#include "Async/Thread/JThread.h"
#include "Async/Thread/Factory.h"

namespace syscross::HelloEOS { struct MainAsynchronously {
	void run(int argc) {
		bool isServer = ( argc > 1 );

		auto ticker = Async::Thread::FactoryInfiniteWaiting::createTicker( isServer );
		if ( !ticker ) 
			return;
		auto socketName = "CHAT";
		std::string text0 = "Hello!";

		if ( isServer ) {
			LOG( "[~] server" );
			Async::Transport::Recv recv = ticker ->createReceiver( socketName );
			Async::future_t commandRecvText = recv.text( text0.length( ) );
			auto incoming = commandRecvText.get( );
			std::string text( incoming.begin( ), incoming.end( ) );
			LOG( "[>>%zd] text: '%s'", incoming.size( ), text.c_str( ) );
		} else {
			LOG( "[~] client" );
			Async::Transport::Send send = ticker ->createSender( socketName );
			Async::future_t commandSendText = send.text( text0 );
			commandSendText.wait( );
			LOG( "[<<%zd] text: '%s'", text0.size( ), text0.c_str( ) );
		}

		LOG( "[~] press [Enter] to exit" );
		getchar( );
		return;
	}
};
} // namespace syscross::HelloEOS
