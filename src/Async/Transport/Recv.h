// src\Async\Transport\Recv.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Transport {
class Recv {
	Async::Context m_ctx;
	const std::string m_socketName;
	// shared_ptr to avoid dangling
	multiplex_t m_multiplexer;
	uint8_t m_channel;

	const EOS_HP2P m_p2PHandle;
	const uint8_t* m_requestedChannel = &m_channel;
	EOS_P2P_ReceivePacketOptions m_options{ EOS_P2P_RECEIVEPACKET_API_LATEST };

	Deferred::ConnectionRequestListener::AcceptEveryone m_acceptor;

	typedef Networking::messageData_t messageData_t;

	messageData_t receivPacket_( ) const {
		EOS_ProductUserId unused_;
		EOS_P2P_SocketId receiveSocketId;
		uint8_t outChannel = 0;
		messageData_t packet( m_options.MaxDataSizeBytes );
		uint32_t bytesWritten = 0;
		EOS_EResult result = ::EOS_P2P_ReceivePacket( m_p2PHandle, &m_options
			, &unused_, &receiveSocketId, &outChannel, packet.data( ), &bytesWritten );
		if ( EOS_EResult::EOS_NotFound == result ) 
			return { };
		if ( EOS_EResult::EOS_Success != result ) 
			throw std::runtime_error( "error EOS_P2P_ReceivePacket" );
		if ( m_channel != outChannel ) 
			throw std::runtime_error( "channel mismatch" );
		packet.resize( bytesWritten );

		// trace
		auto str = ( std::stringstream( )<< Hexdump( packet.data( ), packet.size( ) ) ).str( );
		LOG( "[Recv] '%s' Hexdump of amout bytes: %zd\n%s", receiveSocketId.SocketName, bytesWritten, str.c_str( ) );
		return packet;
	}

public:
	Recv(Async::Context const& ctx, std::string const& socketName, multiplex_t const& multiplexer, uint8_t channel = 0) :
		m_ctx( ctx )
		, m_socketName( socketName )
		, m_multiplexer( multiplexer )
		, m_channel( channel )
		, m_p2PHandle( ::EOS_Platform_GetP2PInterface( ctx.m_platformHandle ) )
		// TODO(alex): make own Async::acceptor or proxy class
		, m_acceptor( Deferred::Ctx{ socketName, m_ctx.m_localUserId, m_ctx.m_platformHandle, m_ctx.m_friendLocalUserId } )
	{
		m_options.LocalUserId = m_ctx.m_localUserId;
		m_options.MaxDataSizeBytes = Networking::c_MaxDataSizeBytes;
		m_options.RequestedChannel = m_requestedChannel;
	}
	[[nodiscard]] future_t text(size_t len) const {
		auto accumulator = std::make_shared< messageData_t >( );
		packaged_task_t task = 
			std::packaged_task( [this, len, accumulator](arg0_packaged_task_t const& tick) ->messageData_t
			{
				while ( true ) {
					messageData_t packet = receivPacket_( );
					// accumulate
					std::copy( packet.begin( ), packet.end( ), std::back_inserter( *accumulator ) );
					// until requirement size
					if ( accumulator ->size( ) == len )
						return *accumulator;
					if ( !tick( ) )
						return { };
				}
				throw std::runtime_error( "unreachable code" );
			} );
		auto future = task.get_future( );
		m_multiplexer ->incoming( std::move( task ) );
		return future;
	}
};
} // namespace syscross::HelloEOS::Async::Transport
