// src\Synchronously\Receive\BaseReceive.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously::Receive {
class BaseReceive {
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
	}
protected:
	const EOS_HPlatform m_PlatformHandle;
	const EOS_ProductUserId m_LocalUserId;
	const std::string m_SocketName;
	const EOS_HP2P m_P2PHandle;
	EOS_P2P_ReceivePacketOptions m_ReceivePacketOptions;
	EOS_P2P_SocketId m_SocketId;

	Networking::messageData_t receive_(bool doTick = true) {
		//Packet params
		EOS_ProductUserId friendAccountId;
		uint8_t channel = 0;
		Networking::messageData_t messageData;
		messageData.resize( m_ReceivePacketOptions.MaxDataSizeBytes );
		uint32_t bytesWritten = 0;
		EOS_EResult result;
		do { 
			result = ::EOS_P2P_ReceivePacket( m_P2PHandle, &m_ReceivePacketOptions
				, &friendAccountId, &m_SocketId, &channel, messageData.data(), &bytesWritten );
			if ( EOS_EResult::EOS_Success == result ) {
				messageData.resize( bytesWritten );
				break;
			}
			//if ( doTick ) ::EOS_Platform_Tick( m_PlatformHandle );
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

		EOS_NotificationId connectionNotificationId = ::EOS_P2P_AddNotifyPeerConnectionRequest(
			m_P2PHandle, &connectionRequestOptions, this, OnIncomingConnectionRequest_ );
		if ( connectionNotificationId == EOS_INVALID_NOTIFICATIONID )
			LOG( "EOS P2PNAT SubscribeToConnectionRequests: could not subscribe, bad notification id returned.");

		m_ReceivePacketOptions.ApiVersion = EOS_P2P_RECEIVEPACKET_API_LATEST;
		m_ReceivePacketOptions.LocalUserId = m_LocalUserId;
		m_ReceivePacketOptions.MaxDataSizeBytes = Networking::c_MaxDataSizeBytes;
		m_ReceivePacketOptions.RequestedChannel = nullptr;
	}
};
} // namespace syscross::HelloEOS::Synchronously::Receive
