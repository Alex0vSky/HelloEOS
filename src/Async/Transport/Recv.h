// src\Async\Transport\Recv.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Transport {
class Recv {
	multiplex_t m_multiplexer; // shared_ptr to avoid dangling
	const uint8_t m_channel;
	const EOS_HP2P m_p2PHandle;
	const uint8_t* m_requestedChannel = &m_channel;
	EOS_P2P_ReceivePacketOptions m_options{ EOS_P2P_RECEIVEPACKET_API_LATEST };
	detail_::Acceptor m_acceptor;
	typedef Networking::messageData_t messageData_t;

public:
	Recv(Environs const& ctx, std::string const& socketName, multiplex_t const& mux) :
		m_multiplexer( mux )
		, m_channel( 0 ) // zero for simplicity
		, m_p2PHandle( ::EOS_Platform_GetP2PInterface( ctx.m_platformHandle ) )
		, m_acceptor( ctx, socketName )
	{
		m_options.LocalUserId = ctx.m_localUserId;
		m_options.MaxDataSizeBytes = Networking::c_MaxDataSizeBytes;
		m_options.RequestedChannel = m_requestedChannel;
	}
	[[nodiscard]] auto byLength(size_t len) const {
		task_t task = 
			std::packaged_task( [this, len](task_arg_t const& tick) ->messageData_t
			{
				EOS_ProductUserId unused_;
				EOS_P2P_SocketId socketId;
				uint8_t channel = 0;
				messageData_t container( len );
				uint32_t bytesWritten = 0;
				auto it = container.begin( );
				while ( it != container.end( ) ) {
					EOS_EResult result = ::EOS_P2P_ReceivePacket( m_p2PHandle, &m_options
						, &unused_, &socketId, &channel, std::addressof( *it ), &bytesWritten );
					if ( EOS_EResult::EOS_Success == result ) {
						// trace
						auto str = ( std::stringstream( )<< Hexdump( std::addressof( *it ), bytesWritten ) ).str( ); LOG( "[Recv] '%s' Hexdump of amout bytes: %zd\n%s", socketId.SocketName, bytesWritten, str.c_str( ) );

						it += bytesWritten;
						continue;
					}
					if ( EOS_EResult::EOS_NotFound != result ) 
						throw std::runtime_error( "error EOS_P2P_ReceivePacket" );
					if ( !tick( ) )
						return { };
				}
				return container;
			} );
		auto future = task.get_future( );
		m_multiplexer ->incoming( std::move( task ) );
		return future;
	}
};
} // namespace syscross::HelloEOS::Async::Transport
