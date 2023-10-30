// src\Synchronously\Receive\BaseReceive.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously::Receive {
class BaseReceive {
	// TODO(alex): dont repeat youself
	static void OnIncomingConnectionRequest_(const EOS_P2P_OnIncomingConnectionRequestInfo* Data) {
		if ( !Data )
			return;
		BaseReceive *self = reinterpret_cast<BaseReceive *>( Data ->ClientData );
		std::string SocketName = Data ->SocketId ->SocketName;
		if ( SocketName != self ->m_SocketName ) {
			LOG( "[OnIncomingConnectionRequest] bad socket id.");
			return;
		}
		EOS_P2P_AcceptConnectionOptions Options = {};
		Options.ApiVersion = EOS_P2P_ACCEPTCONNECTION_API_LATEST;
		Options.LocalUserId = self ->m_LocalUserId;
		Options.RemoteUserId = Data->RemoteUserId;

		Options.SocketId = &self ->m_SocketId;

		EOS_EResult result = ::EOS_P2P_AcceptConnection( self ->m_P2PHandle, &Options );
		if ( EOS_EResult::EOS_Success != result ) 
			LOG( "[OnIncomingConnectionRequest] error while accepting connection, code: %s."
				, EOS_EResult_ToString(result) );
		LOG( "[OnIncomingConnectionRequest] accepted" );
	}

	const std::string m_SocketName;
	EOS_P2P_SocketId m_SocketId;
	uint8_t m_channel = 0;
	// retrieving the size of the next packet on any channel
	const uint8_t* m_requestedChannel = nullptr;
	EOS_NotificationId m_notificationId;
	EOS_P2P_ReceivePacketOptions m_ReceivePacketOptions;

protected:
	const EOS_HPlatform m_PlatformHandle;
	const EOS_ProductUserId m_LocalUserId;
	const EOS_HP2P m_P2PHandle;

	// TODO(alex): dont repeat youself
	Networking::optionalMessageData_t receive_(bool doTick = true, size_t len = 0) {
		// Packet params
		EOS_ProductUserId friendAccountId;
		Networking::messageData_t messageData;
		EOS_P2P_ReceivePacketOptions receivePacketOptions = m_ReceivePacketOptions;
		if ( len )
			receivePacketOptions.MaxDataSizeBytes = len;
		EOS_EResult result;
		// to avoid truncating
		{
			EOS_P2P_GetNextReceivedPacketSizeOptions Options = { EOS_P2P_GETNEXTRECEIVEDPACKETSIZE_API_LATEST };
			Options.LocalUserId = m_LocalUserId;
			Options.RequestedChannel = m_requestedChannel;
			uint32_t outPacketSizeBytes = 0;
			result = ::EOS_P2P_GetNextReceivedPacketSize( m_P2PHandle, &Options, &outPacketSizeBytes );
			if ( EOS_EResult::EOS_NotFound == result ) 
				return { };
			if ( EOS_EResult::EOS_Success != result ) 
				throw std::runtime_error( "error EOS_P2P_GetNextReceivedPacketSize" );
			if ( len && len < outPacketSizeBytes ) 
				throw std::runtime_error( "received packet size was larger than out-buffer, truncating disallowed" );
			if ( len && len != outPacketSizeBytes ) 
				throw std::runtime_error( "lenght mismatch" );
		}
		messageData.resize( receivePacketOptions.MaxDataSizeBytes );
		uint32_t bytesWritten = 0;
		do { 
			result = ::EOS_P2P_ReceivePacket( m_P2PHandle, &receivePacketOptions
				, &friendAccountId, &m_SocketId, &m_channel, messageData.data( ), &bytesWritten );
			if ( EOS_EResult::EOS_Success == result ) {
				LOG( "[BaseReceive] bytesWritten: %d", bytesWritten );
				messageData.resize( bytesWritten );
				break;
			}
			if ( !doTick ) {
				return { };
			}
			::EOS_Platform_Tick( m_PlatformHandle );
			std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } );
			if ( EOS_EResult::EOS_NotFound != result ) {
				LOG( "[BaseReceive] error while reading data, code: %s.", EOS_EResult_ToString( result ) );
				return { };
			}
		} while ( true );

		return messageData;
	}

public:
	BaseReceive(
		EOS_HPlatform platformHandle
		, EOS_ProductUserId localUserId
		, const std::string &SocketName = "CHAT"
	) :
		m_PlatformHandle( platformHandle )
		, m_LocalUserId( localUserId )
		, m_SocketName( SocketName )
		, m_P2PHandle( ::EOS_Platform_GetP2PInterface( platformHandle ) )
		, m_ReceivePacketOptions{ }
		, m_SocketId{ }
	{
		m_SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
		strcpy_s( m_SocketId.SocketName , m_SocketName.c_str( ) );

		EOS_P2P_AddNotifyPeerConnectionRequestOptions connectionRequestOptions = { };
		connectionRequestOptions.ApiVersion = EOS_P2P_ADDNOTIFYPEERCONNECTIONREQUEST_API_LATEST;
		connectionRequestOptions.LocalUserId = m_LocalUserId;
		connectionRequestOptions.SocketId = &m_SocketId;

		m_notificationId = ::EOS_P2P_AddNotifyPeerConnectionRequest(
			m_P2PHandle, &connectionRequestOptions, this, OnIncomingConnectionRequest_ );
		if ( m_notificationId == EOS_INVALID_NOTIFICATIONID ) {
			LOG( "[BaseReceive] could not subscribe, bad notification id returned.");
			throw std::runtime_error( "error EOS_P2P_AddNotifyPeerConnectionRequest");
		}

		m_ReceivePacketOptions.ApiVersion = EOS_P2P_RECEIVEPACKET_API_LATEST;
		m_ReceivePacketOptions.LocalUserId = m_LocalUserId;
		m_ReceivePacketOptions.MaxDataSizeBytes = Networking::c_MaxDataSizeBytes;
		m_ReceivePacketOptions.RequestedChannel = m_requestedChannel;
	}

	~BaseReceive() {
		// Get [LogEOSP2P] Successfully closed last connection request listener for this socket...
		::EOS_P2P_RemoveNotifyPeerConnectionRequest( m_P2PHandle, m_notificationId );
	}
};
} // namespace syscross::HelloEOS::Synchronously::Receive
