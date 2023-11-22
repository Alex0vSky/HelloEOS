// src\Deferred\Sender\SendText.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred::Sender {
class SendText {
	const Ctx m_ctx;
	const EOS_HP2P m_p2PHandle;
	EOS_P2P_SocketId m_sendSocketId;
	EOS_P2P_SendPacketOptions m_options;

public: // tmp

	// TODO(alex): base and multiple sender classes, or single SendText=SendOrdinary
	template<typename T>
	void sendPacket_(const T &messageData) {
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
	}

public:
	SendText(const Ctx &ctx, uint8_t channel) :
		m_ctx( ctx )
		, m_p2PHandle( ::EOS_Platform_GetP2PInterface( ctx.m_PlatformHandle ) )
		, m_sendSocketId{ EOS_P2P_SOCKETID_API_LATEST }
		, m_options{ EOS_P2P_SENDPACKET_API_LATEST }
	{
		strcpy_s( m_sendSocketId.SocketName , m_ctx.m_SocketName.c_str( ) );
		m_options.LocalUserId = m_ctx.m_LocalUserId;
		m_options.RemoteUserId = m_ctx.m_FriendLocalUserId;
		m_options.SocketId = &m_sendSocketId;
		m_options.bAllowDelayedDelivery = EOS_TRUE;
		m_options.Channel = channel;
		m_options.Reliability = EOS_EPacketReliability::EOS_PR_ReliableOrdered;
		m_options.bDisableAutoAcceptConnection = EOS_FALSE;
	}
	void sendTextPacket_(const std::string &value) {
		sendPacket_( value );
	}
};
} // namespace syscross::HelloEOS::Deferred::Sender
