// src\Synchronously\Receive\Chat.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously::Receive {
class Chat {
	EOS_HPlatform m_PlatformHandle;
	EOS_ProductUserId m_LocalUserId;

	static void OnIncomingConnectionRequest(const EOS_P2P_OnIncomingConnectionRequestInfo* Data) {
		if ( !Data )
			return;
		std::string SocketName = Data->SocketId->SocketName;
		if (SocketName != "CHAT") {
			LOG( "[OnIncomingConnectionRequest] bad socket id.");
			return;
		}
		Chat *self = reinterpret_cast<Chat *>( Data ->ClientData );
		EOS_HP2P P2PHandle = EOS_Platform_GetP2PInterface( self ->m_PlatformHandle );
		EOS_P2P_AcceptConnectionOptions Options = {};
		Options.ApiVersion = EOS_P2P_ACCEPTCONNECTION_API_LATEST;
		Options.LocalUserId = self ->m_LocalUserId;
		Options.RemoteUserId = Data->RemoteUserId;

		EOS_P2P_SocketId SocketId = { };
		SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
		strncpy_s(SocketId.SocketName, "CHAT", 5);
		Options.SocketId = &SocketId;

		EOS_EResult Result = EOS_P2P_AcceptConnection( P2PHandle, &Options );
		if (Result != EOS_EResult::EOS_Success) {
			LOG( "[OnIncomingConnectionRequest] error while accepting connection, code: %s."
				, EOS_EResult_ToString(Result) );
		}
		LOG( "[OnIncomingConnectionRequest] AcceptConnection OK" );
	}

public:
	Chat(
		EOS_HPlatform platformHandle
		, EOS_ProductUserId localUserId
	) :
		m_PlatformHandle( platformHandle )
		, m_LocalUserId( localUserId )
	{}

	std::string getMessage() {
		std::string message;

		{
			EOS_HP2P P2PHandle = EOS_Platform_GetP2PInterface( m_PlatformHandle );

			EOS_P2P_SocketId SocketId = {};
			SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
			strncpy_s(SocketId.SocketName, "CHAT", 5);

			EOS_P2P_AddNotifyPeerConnectionRequestOptions Options = {};
			Options.ApiVersion = EOS_P2P_ADDNOTIFYPEERCONNECTIONREQUEST_API_LATEST;
			Options.LocalUserId = m_LocalUserId;
			Options.SocketId = &SocketId;

			auto ConnectionNotificationId = EOS_P2P_AddNotifyPeerConnectionRequest(P2PHandle, &Options, this, OnIncomingConnectionRequest);
			if (ConnectionNotificationId == EOS_INVALID_NOTIFICATIONID)
			{
				LOG( "EOS P2PNAT SubscribeToConnectionRequests: could not subscribe, bad notification id returned.");
			}
		}

		EOS_HP2P P2PHandle = ::EOS_Platform_GetP2PInterface( m_PlatformHandle );
		EOS_P2P_ReceivePacketOptions Options = { };
		Options.ApiVersion = EOS_P2P_RECEIVEPACKET_API_LATEST;
		Options.LocalUserId = m_LocalUserId;
		Options.MaxDataSizeBytes = 4096;
		Options.RequestedChannel = nullptr;
		//Packet params
		EOS_ProductUserId FriendAccountId;
		EOS_P2P_SocketId SocketId = { };
		SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
		uint8_t Channel = 0;
		std::vector<char> MessageData;
		MessageData.resize( Options.MaxDataSizeBytes );
		uint32_t BytesWritten = 0;
		EOS_EResult Result;
		do { 
			Result = EOS_P2P_ReceivePacket(P2PHandle, &Options, &FriendAccountId, &SocketId, &Channel, MessageData.data(), &BytesWritten);
			if ( EOS_EResult::EOS_Success == Result )
				break;
			::EOS_Platform_Tick( m_PlatformHandle );
			std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } );
			if ( EOS_EResult::EOS_NotFound != Result ) {
				LOG( "[~] HandleReceivedMessages: error while reading data, code: %s.", EOS_EResult_ToString( Result ) );
			}
		} while ( true );
		message = MessageData.data( );

		return message;
	}
};
} // namespace syscross::HelloEOS::Synchronously::Receive
