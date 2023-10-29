// src\Synchronously\Send\BaseSend.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously::Send {
class BaseSend {
protected:
	const EOS_HPlatform m_PlatformHandle;
	const EOS_ProductUserId m_LocalUserId;
	const EOS_ProductUserId m_FriendLocalUserId;
	const EOS_HP2P m_P2PHandle;
	EOS_P2P_SocketId m_SocketId;
	EOS_P2P_SendPacketOptions m_Options;
	// TODO(alex): useless
	std::atomic_bool m_bStop = false;

	// TODO(alex): sync only, move to separate or remove
	void ticks_(bool doTick = true) const {
		do {
			if ( doTick ) ::EOS_Platform_Tick( m_PlatformHandle );
			std::this_thread::sleep_for( std::chrono::milliseconds{ 1 } );
		} while( !m_bStop );
	}

	template<typename T>
	bool sendPacket_(const T &value) {
		Networking::messageData_t messageData( value.begin( ), value.end( ) );
		auto dataLengthBytes = messageData.size( ) * sizeof( Networking::messageData_t::value_type );
		m_Options.DataLengthBytes = static_cast<uint32_t>( dataLengthBytes );
		m_Options.Data = messageData.data( );
		EOS_EResult Result = ::EOS_P2P_SendPacket( m_P2PHandle, &m_Options );
		return EOS_EResult::EOS_Success == Result;
	}
	bool sendTextPacket_(const std::string &value) {
		return sendPacket_( value );
	}

public:
	BaseSend(
		EOS_HPlatform platformHandle
		, EOS_ProductUserId localUserId
		, EOS_ProductUserId friendLocalUserId
		, const std::string &SocketName = "CHAT"
	) :
		m_PlatformHandle( platformHandle )
		, m_LocalUserId( localUserId )
		, m_FriendLocalUserId( friendLocalUserId )
		, m_P2PHandle( ::EOS_Platform_GetP2PInterface( platformHandle ) )
		, m_SocketId{ }
		, m_Options{ }
	{
		m_SocketId.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
		strcpy_s( m_SocketId.SocketName , SocketName.c_str( ) );

		m_Options.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
		// m_Options.LocalUserId = Player ->GetProductUserID();
		m_Options.LocalUserId = m_LocalUserId;
		// m_Options.RemoteUserId = FriendId;
		m_Options.RemoteUserId = m_FriendLocalUserId;
		m_Options.SocketId = &m_SocketId;
		m_Options.bAllowDelayedDelivery = EOS_TRUE;
		m_Options.Channel = 0;
		m_Options.Reliability = EOS_EPacketReliability::EOS_PR_ReliableOrdered;
		m_Options.bDisableAutoAcceptConnection = EOS_FALSE;
	}
};
} // namespace syscross::HelloEOS::Synchronously::Send
