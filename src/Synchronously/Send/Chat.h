// src\Synchronously\Send\Chat.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously::Send {
class Chat {
	EOS_HPlatform m_PlatformHandle;
	EOS_ProductUserId m_LocalUserId;
	EOS_ProductUserId m_FriendLocalUserId;

public:
	Chat(
		EOS_HPlatform platformHandle
		, EOS_ProductUserId localUserId
		, EOS_ProductUserId friendLocalUserId
	) :
		m_PlatformHandle( platformHandle )
		, m_LocalUserId( localUserId )
		, m_FriendLocalUserId( friendLocalUserId )
	{}

	bool message(const std::string &text) {
		EOS_EResult Result;
		{
			EOS_HP2P P2PHandle = ::EOS_Platform_GetP2PInterface( m_PlatformHandle );
			EOS_P2P_SocketId SocketId = { };
			SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
			strncpy_s(SocketId.SocketName, "CHAT", 5);

			EOS_P2P_SendPacketOptions Options = { };
			Options.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
			// Options.LocalUserId = Player ->GetProductUserID();
			Options.LocalUserId = m_LocalUserId;
			// Options.RemoteUserId = FriendId;
			Options.RemoteUserId = m_FriendLocalUserId;
			Options.SocketId = &SocketId;
			Options.bAllowDelayedDelivery = EOS_TRUE;
			Options.Channel = 0;
			Options.Reliability = EOS_EPacketReliability::EOS_PR_ReliableOrdered;
			Options.bDisableAutoAcceptConnection = EOS_FALSE;

			Options.DataLengthBytes = static_cast<uint32_t>( text.size());
			Options.Data = text.data();
			Result = ::EOS_P2P_SendPacket( P2PHandle, &Options );
			// TODO(alex): no way to known deliverance now, and UDP too
			LOG( "[~] press [Ctrl+C] to exit" );
			do {
				::EOS_Platform_Tick( m_PlatformHandle );
				std::this_thread::sleep_for( std::chrono::milliseconds{ 1 } );
			} while( true );
		}
		return true;
	}
};
} // namespace syscross::HelloEOS::Synchronously::Send
