// src\Async\Transport\Send.h - only for single filling eos queue, without flush
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Transport {
class Send {
	multiplex_t m_multiplexer; // shared_ptr to avoid dangling
	const EOS_HP2P m_p2PHandle;
	EOS_P2P_SendPacketOptions m_options{ EOS_P2P_SENDPACKET_API_LATEST };
	EOS_P2P_SocketId m_sendSocketId{ EOS_P2P_SOCKETID_API_LATEST };
	detail_::Acceptor m_acceptor;
	typedef Networking::messageData_t messageData_t;

	template<typename T>
	future_t send_(T const& container) const {
		task_t task = 
			std::packaged_task( [this, container](task_arg_t const&) ->messageData_t
			{
				const size_t maxDataLengthBytes = Networking::c_MaxDataSizeBytes;
				auto it = container.begin( );
				while ( it != container.end( ) ) {
					const size_t distance = std::distance( it, container.end( ) );
					const size_t dataLengthBytes = std::min( maxDataLengthBytes, distance );
					// trace
					auto str = ( std::stringstream( )<< Hexdump( std::addressof( *it ), dataLengthBytes ) ).str( ); LOG( "[sendPacket_] Hexdump of amout bytes: %zd\n%s", dataLengthBytes, str.c_str( ) );

					EOS_P2P_SendPacketOptions options = m_options;
					options.DataLengthBytes = static_cast< uint32_t >( dataLengthBytes );
					options.Data = std::addressof( *it );
					EOS_EResult result = ::EOS_P2P_SendPacket( m_p2PHandle, &options );
					if ( EOS_EResult::EOS_Success != result )
						throw std::runtime_error( "error EOS_P2P_SendPacket" );
					it += dataLengthBytes;
				}
				return { };
			} );
		auto future = task.get_future( );
		m_multiplexer ->outgoing( std::move( task ) );
		return future;
	}

public:
	Send(Environs const& ctx, std::string const& socketName, multiplex_t const& mux) :
		m_multiplexer( mux )
		, m_p2PHandle( ::EOS_Platform_GetP2PInterface( ctx.m_platformHandle ) )
		, m_acceptor( ctx, socketName )
	{
		strcpy_s( m_sendSocketId.SocketName , socketName.c_str( ) );
		m_options.LocalUserId = ctx.m_localUserId;
		m_options.RemoteUserId = ctx.m_friendLocalUserId;
		m_options.SocketId = &m_sendSocketId;
		m_options.bAllowDelayedDelivery = EOS_TRUE;
		m_options.Channel = 0; // zero for simplicity
		m_options.Reliability = EOS_EPacketReliability::EOS_PR_ReliableOrdered;
		m_options.bDisableAutoAcceptConnection = EOS_FALSE;
	}
	auto text(std::string const& text) const {
		return send_( text );
	}
	auto vector(messageData_t const& vector) const {
		return send_( vector );
	}
};
} // namespace syscross::HelloEOS::Async::Transport
