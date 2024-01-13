// src\Deferred\Receiver\RecvText.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred::Receiver {
class RecvText {
	const Ctx m_ctx;
	const EOS_HP2P m_p2PHandle;
	EOS_P2P_SocketId m_receiveSocketId;

	EOS_P2P_ReceivePacketOptions m_options;
	const uint8_t m_channel = 0;
	// If NULL, we're retrieving the size of the next packet on any channel
	const uint8_t* m_requestedChannel = &m_channel;

public:
	RecvText(const Ctx &ctx, uint8_t channel) :
		m_ctx( ctx )
		, m_p2PHandle( ::EOS_Platform_GetP2PInterface( ctx.m_PlatformHandle ) )
		, m_receiveSocketId{ EOS_P2P_SOCKETID_API_LATEST }
		, m_options{ EOS_P2P_RECEIVEPACKET_API_LATEST }
		, m_channel( channel )
	{
		strcpy_s( m_receiveSocketId.SocketName , m_ctx.m_SocketName.c_str( ) );
		m_options.LocalUserId = m_ctx.m_LocalUserId;
		m_options.MaxDataSizeBytes = Networking::c_MaxDataSizeBytes;
		m_options.RequestedChannel = m_requestedChannel;
	}

	// TODO(alex): base and multiple sender classes, or single SendText=SendOrdinary
	Networking::messageData_t receive_() {
		EOS_ProductUserId unused_;
		Networking::messageData_t messageData( m_options.MaxDataSizeBytes );
		uint32_t bytesWritten = 0;
		uint8_t outChannel = 0;
		EOS_EResult result = ::EOS_P2P_ReceivePacket( m_p2PHandle, &m_options
			, &unused_, &m_receiveSocketId, &outChannel, messageData.data( ), &bytesWritten );
		if ( EOS_EResult::EOS_NotFound == result ) 
			return { };
		if ( EOS_EResult::EOS_Success != result ) 
			throw std::runtime_error( "error EOS_P2P_ReceivePacket" );
		if ( m_channel != outChannel ) 
			throw std::runtime_error( "channel mismatch" );

		//LOG( "[receive_] bytesWritten: %d", bytesWritten );
		messageData.resize( bytesWritten );

		// trace
		auto str = ( std::stringstream( )<< Hexdump( messageData.data( ), messageData.size( ) ) ).str( );
		LOG( "[receive_] Hexdump of amout bytes: %u\n%s", bytesWritten, str.c_str( ) );

		return messageData;
	}
};
} // namespace syscross::HelloEOS::Deferred::Receiver
