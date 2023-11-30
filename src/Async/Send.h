// src\Async\Send.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async {
class Send {
	Async::Context m_ctx;
	multiplex_t m_multiplexer;
	uint8_t m_channel;
	typedef Networking::messageData_t messageData_t;

public:
	Send(Async::Context const& ctx, multiplex_t const& multiplexer, uint8_t channel = 0) :
		m_ctx( ctx )
		, m_multiplexer( multiplexer )
		, m_channel( channel )
	{}
	typedef std::future<messageData_t> future_t;
	[[nodiscard]] future_t text(std::string const& text) const {
		auto accumulator = std::make_shared< Networking::messageData_t >( );
		packaged_task_t task = 
			std::packaged_task( [this, text, accumulator](bool *enough) ->messageData_t
			{
				accumulator ->push_back( rand( ) );
				*enough = true;
				const EOS_HP2P m_p2PHandle = ::EOS_Platform_GetP2PInterface( m_ctx.m_platformHandle );
				EOS_P2P_SendPacketOptions m_options{ EOS_P2P_SENDPACKET_API_LATEST };
				EOS_P2P_SocketId m_sendSocketId{ EOS_P2P_SOCKETID_API_LATEST };
				strcpy_s( m_sendSocketId.SocketName , m_ctx.m_socketName.c_str( ) );
				m_options.LocalUserId = m_ctx.m_localUserId;
				m_options.RemoteUserId = m_ctx.m_friendLocalUserId;
				m_options.SocketId = &m_sendSocketId;
				m_options.bAllowDelayedDelivery = EOS_TRUE;
				m_options.Channel = m_channel;
				m_options.Reliability = EOS_EPacketReliability::EOS_PR_ReliableOrdered;
				m_options.bDisableAutoAcceptConnection = EOS_FALSE;

				auto &messageData = text;
				const size_t maxDataLengthBytes = Networking::c_MaxDataSizeBytes;
				auto it = messageData.begin( );
				while ( it != messageData.end( ) ) {
					const size_t distance = std::distance( it, messageData.end( ) );
					const size_t dataLengthBytes = std::min( maxDataLengthBytes, distance );
					// trace
					auto str = ( std::stringstream( )<< Hexdump( std::addressof( *it ), dataLengthBytes ) ).str( );
					LOG( "[sendPacket_] Hexdump of amout bytes: %zd\n%s", dataLengthBytes, str.c_str( ) );

					m_options.DataLengthBytes = static_cast< uint32_t >( dataLengthBytes );
					m_options.Data = std::addressof( *it );
					EOS_EResult result = ::EOS_P2P_SendPacket( m_p2PHandle, &m_options );
					if ( EOS_EResult::EOS_Success != result )
						throw std::runtime_error( "error EOS_P2P_SendPacket" );
					it += dataLengthBytes;
				}
				return { 'h', 'e', 'l', 'l', 'o' };
			} );
		auto future = task.get_future( );
		m_multiplexer ->outgoing( std::move( task ) );
		return future;
	}
};
} // namespace syscross::HelloEOS::Async
