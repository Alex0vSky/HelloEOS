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
struct Context {
	const EOS_HPlatform m_platformHandle;
	const std::string m_socketName;
	EOS_ProductUserId const m_localUserId, m_friendLocalUserId;
};
} // namespace syscross::HelloEOS::Async
#include "Async/PrepareEos.h"
#include "Async/SuperClass.h"
#include "Async/IMultiplex.h"
#include "Async/Multiplexer.h"
namespace syscross::HelloEOS::Async { class Executor {
	multiplex_t m_multiplexer;
	const EOS_HPlatform m_platformHandle;
	const EOS_HP2P m_p2PHandle;
	static constexpr std::chrono::seconds c_commandTO{ 30 };
	static constexpr auto now = std::chrono::system_clock::now;
	uint64_t outgoingPacketQueueCurrentSizeBytes_() {
		EOS_P2P_PacketQueueInfo queueInfo = { };
		EOS_P2P_GetPacketQueueInfoOptions options = { EOS_P2P_GETPACKETQUEUEINFO_API_LATEST };
		auto r = ::EOS_P2P_GetPacketQueueInfo( m_p2PHandle, &options, &queueInfo ); // error checking has been ommited for "codereview"
		if ( EOS_EResult::EOS_Success != r )
			// TODO(alex): checkme in thread
			throw std::runtime_error( "error EOS_P2P_GetPacketQueueInfo" );
		return queueInfo.OutgoingPacketQueueCurrentSizeBytes;
	}
	EOS_NotificationId m_notificationIdConnectionClosed = EOS_INVALID_NOTIFICATIONID;
	bool m_bConnectionClosed = false;
	static void EOS_CALL ConnectionClosedHandler_(const EOS_P2P_OnRemoteConnectionClosedInfo* Data) {
		if ( !Data )
			return;
		Executor *self = reinterpret_cast<Executor *>( Data ->ClientData );
		self ->m_bConnectionClosed = true;
		LOG( "[ConnectionClosedHandler_] yeap" );
	}
	EOS_P2P_SocketId m_SocketId;

public:
	Executor(multiplex_t const& multiplexer, Async::Context const& ctx) :
		m_multiplexer( multiplexer )
		, m_platformHandle( ctx.m_platformHandle )
		, m_p2PHandle( ::EOS_Platform_GetP2PInterface( m_platformHandle ) )
		, m_SocketId{ EOS_P2P_SOCKETID_API_LATEST }
	{
		// TODO(alex): to separete class `ConnectionClosed`
		strcpy_s( m_SocketId.SocketName, ctx.m_socketName.c_str( ) );
		EOS_P2P_AddNotifyPeerConnectionClosedOptions options = { EOS_P2P_ADDNOTIFYPEERCONNECTIONCLOSED_API_LATEST };
		options.LocalUserId = ctx.m_localUserId;
		options.SocketId = &m_SocketId;
		m_notificationIdConnectionClosed = ::EOS_P2P_AddNotifyPeerConnectionClosed(
			m_p2PHandle, &options, this, ConnectionClosedHandler_ );
	}
	~Executor() {
		// TODO(alex): exception
		//::EOS_P2P_RemoveNotifyPeerConnectionClosed( m_p2PHandle, m_notificationIdConnectionClosed );
	}
	void processAll() {
		packaged_task_t task;
		Direction direction;
		if ( !m_multiplexer ->pop( &task, &direction ) ) 
			return;
		bool enough = false;
		if ( Direction::Outgoing == direction ) {
			// tmp
			task( &enough );
			task.reset( );
			task( &enough );
			do ::EOS_Platform_Tick( m_platformHandle ), std::this_thread::sleep_for( 15ms );
			while ( outgoingPacketQueueCurrentSizeBytes_( ) );
			if ( this ->m_bConnectionClosed ) {
				// TODO(alex): TO
			}
			return;
		}
		auto timeout = now( ) + c_commandTO;
		if ( Direction::Incoming == direction ) {
			Networking::messageData_t packet = { };
			do {
				// TODO(alex): Houston, we have a problem
				task( &enough );
				if ( enough )
					break;
				::EOS_Platform_Tick( m_platformHandle );
			} while ( now( ) < timeout );
		}
		::EOS_Platform_Tick( m_platformHandle );
	}
};
} // namespace syscross::HelloEOS::Async
#include "Async/Send.h"
#include "Async/Thread/Ticker.h"
#include "Async/Thread/JThread.h"
#include "Async/Thread/Factory.h"

namespace syscross::HelloEOS { struct MainAsynchronously {
	void run(int argc) {
		bool isServer = ( argc > 1 );

		auto ticker = Async::Thread::FactoryInfiniteWaiting::createTicker( isServer );
		if ( !ticker ) 
			return;
		Async::Send send = ticker ->createSender( "CHAT" );
		Async::Send::future_t commandSendText = send.text( "asd" ); // std::future
		auto zxc0 = commandSendText.get( ); // waiting...

		LOG( "[~] press [Enter] to exit" );
		getchar( );
		return;
	}
};
} // namespace syscross::HelloEOS
