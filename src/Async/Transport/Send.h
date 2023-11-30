// src\Async\Transport\Send.h - only for single filling eos queue, without flush
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Transport {
class Send {
	multiplex_t m_multiplexer; // shared_ptr to avoid dangling
	const uint8_t m_channel;
	const EOS_HP2P m_p2PHandle;
	EOS_P2P_SendPacketOptions m_options{ EOS_P2P_SENDPACKET_API_LATEST };
	EOS_P2P_SocketId m_sendSocketId{ EOS_P2P_SOCKETID_API_LATEST };
	Deferred::ConnectionRequestListener::AcceptEveryone m_acceptor;

	template<typename T>
	void sendPacket_(const T &messageData) const {
		const size_t maxDataLengthBytes = Networking::c_MaxDataSizeBytes;
		auto it = messageData.begin( );
		while ( it != messageData.end( ) ) {
			const size_t distance = std::distance( it, messageData.end( ) );
			const size_t dataLengthBytes = std::min( maxDataLengthBytes, distance );
			// trace
			auto str = ( std::stringstream( )<< Hexdump( std::addressof( *it ), dataLengthBytes ) ).str( );
			LOG( "[sendPacket_] Hexdump of amout bytes: %zd\n%s", dataLengthBytes, str.c_str( ) );

			EOS_P2P_SendPacketOptions options = m_options;
			options.DataLengthBytes = static_cast< uint32_t >( dataLengthBytes );
			options.Data = std::addressof( *it );
			EOS_EResult result = ::EOS_P2P_SendPacket( m_p2PHandle, &options );
			if ( EOS_EResult::EOS_Success != result )
				throw std::runtime_error( "error EOS_P2P_SendPacket" );
			it += dataLengthBytes;
		}
	}

public:
	Send(Async::Context const& ctx, std::string const& socketName, multiplex_t const& multiplexer, uint8_t channel = 0) :
		m_multiplexer( multiplexer )
		, m_channel( channel )
		, m_p2PHandle( ::EOS_Platform_GetP2PInterface( ctx.m_platformHandle ) )
		// TODO(alex): make own Async::acceptor or proxy class
		, m_acceptor( Deferred::Ctx{ socketName, ctx.m_localUserId, ctx.m_platformHandle, ctx.m_friendLocalUserId } )
	{
		strcpy_s( m_sendSocketId.SocketName , socketName.c_str( ) );
		m_options.LocalUserId = ctx.m_localUserId;
		m_options.RemoteUserId = ctx.m_friendLocalUserId;
		m_options.SocketId = &m_sendSocketId;
		m_options.bAllowDelayedDelivery = EOS_TRUE;
		m_options.Channel = m_channel;
		m_options.Reliability = EOS_EPacketReliability::EOS_PR_ReliableOrdered;
		m_options.bDisableAutoAcceptConnection = EOS_FALSE;
	}
	[[nodiscard]] future_t text(std::string const& text) const {
		packaged_task_t task = 
			std::packaged_task( [this, text](arg0_packaged_task_t const&) ->Networking::messageData_t
			{
				sendPacket_( text );
				return { };
			} );
		auto future = task.get_future( );
		m_multiplexer ->outgoing( std::move( task ) );
		return future;
	}
};
} // namespace syscross::HelloEOS::Async::Transport
