// src\Async\GradualExecutor.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async {
class GradualExecutor {
	demultiplex_t m_demultiplexer;
	const EOS_HPlatform m_platformHandle;
	const EOS_HP2P m_p2PHandle;
	static constexpr auto c_commandTO{ 30s };
	static constexpr auto now = std::chrono::system_clock::now;
	// TODO(alex): useless if socketName not supported 
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
		GradualExecutor *self = reinterpret_cast<GradualExecutor *>( Data ->ClientData );
		self ->m_bConnectionClosed = true;
		LOG( "[ConnectionClosedHandler_] SocketName: '%s'", Data ->SocketId ->SocketName );
	}

public:
	GradualExecutor(demultiplex_t const& demultiplexer, Async::Context const& ctx) :
		m_demultiplexer( demultiplexer )
		, m_platformHandle( ctx.m_platformHandle )
		, m_p2PHandle( ::EOS_Platform_GetP2PInterface( m_platformHandle ) )
	{
		// TODO(alex): to separete class `ConnectionClosed`
		EOS_P2P_AddNotifyPeerConnectionClosedOptions options = { EOS_P2P_ADDNOTIFYPEERCONNECTIONCLOSED_API_LATEST };
		options.LocalUserId = ctx.m_localUserId;
		// `SocketId` is NULL and will be called for all closed connections
		m_notificationIdConnectionClosed = ::EOS_P2P_AddNotifyPeerConnectionClosed(
			m_p2PHandle, &options, this, ConnectionClosedHandler_ );
	}
	~GradualExecutor() {
		// TODO(alex): exception
		//::EOS_P2P_RemoveNotifyPeerConnectionClosed( m_p2PHandle, m_notificationIdConnectionClosed );
	}
	void all() {
		packaged_task_t task;
		Direction direction;
		while ( m_demultiplexer ->pop( &task, &direction ) ) {
			const auto timeout = now( ) + c_commandTO;
			if ( Direction::Outgoing == direction ) {
				task( { } );
				// TODO(alex): hung on inactive peer. save time and push_back, wait TO too. ?and save outgoing length?
				do {
					::EOS_Platform_Tick( m_platformHandle );
					std::this_thread::sleep_for( 15ms );
					if ( this ->m_bConnectionClosed )
						break;
				} while ( now( ) < timeout && outgoingPacketQueueCurrentSizeBytes_( ) );
			}
			if ( Direction::Incoming == direction ) {
				arg0_packaged_task_t function( [this, &timeout] { 
						::EOS_Platform_Tick( m_platformHandle );
						std::this_thread::sleep_for( 300ms );
						return ( now( ) < timeout );
					} );
				task( function );
			}
		}
	}
};
} // namespace syscross::HelloEOS::Async
