// src\Deferred\ConnectionRequestListener\AcceptEveryone.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred::ConnectionRequestListener {
// TODO(alex): to separate file
class BaseAcceptor {
};
class AcceptEveryone : public BaseAcceptor {
	const std::string m_SocketName;
	EOS_NotificationId m_notificationIdConnectionRequest = EOS_INVALID_NOTIFICATIONID;

protected:
	EOS_HP2P m_p2PHandle;
	EOS_ProductUserId m_LocalUserId;
	EOS_P2P_SocketId m_SocketId;

private:
	// TODO(alex): dont repeat youself
	static void EOS_CALL OnIncomingConnectionRequest_(const EOS_P2P_OnIncomingConnectionRequestInfo* Data) {
		if ( !Data )
			return;
		AcceptEveryone *self = reinterpret_cast<AcceptEveryone *>( Data ->ClientData );
		if ( self ->m_SocketName.compare( Data ->SocketId ->SocketName ) ) 
			throw std::runtime_error( "bad socket id");
		EOS_P2P_AcceptConnectionOptions Options = { EOS_P2P_ACCEPTCONNECTION_API_LATEST };
		Options.LocalUserId = self ->m_LocalUserId;
		Options.RemoteUserId = Data->RemoteUserId;
		Options.SocketId = &self ->m_SocketId;
		EOS_EResult result = ::EOS_P2P_AcceptConnection( self ->m_p2PHandle, &Options );
		// ::EOS_EResult_ToString( result )
		if ( EOS_EResult::EOS_Success != result ) 
			throw std::runtime_error( "error EOS_P2P_AcceptConnection" );
		LOG( "[OnIncomingConnectionRequest] accepted" );
	}

public:
	AcceptEveryone(const Ctx &ctx) :
		m_LocalUserId( ctx.m_LocalUserId )
		, m_SocketName( ctx.m_SocketName )
		, m_p2PHandle( ::EOS_Platform_GetP2PInterface( ctx.m_PlatformHandle ) )
		, m_SocketId{ EOS_P2P_SOCKETID_API_LATEST }
	{
		strcpy_s( m_SocketId.SocketName , m_SocketName.c_str( ) );

		{
			::EOS_P2P_AddNotifyPeerConnectionRequestOptions options = { EOS_P2P_ADDNOTIFYPEERCONNECTIONREQUEST_API_LATEST };
			options.LocalUserId = m_LocalUserId;
			options.SocketId = &m_SocketId;

			m_notificationIdConnectionRequest = ::EOS_P2P_AddNotifyPeerConnectionRequest(
				m_p2PHandle, &options, this, OnIncomingConnectionRequest_ );
			// TODO(alex): tmp comment
			//if ( m_notificationIdConnectionRequest == EOS_INVALID_NOTIFICATIONID ) 
			//	throw std::runtime_error( "error EOS_P2P_AddNotifyPeerConnectionRequest");
			LOG( "[!] AcceptEveryone tmp comment" );
		}
	}
	~AcceptEveryone() {
		// Could be [LogEOSP2P] Successfully closed last connection request listener for this socket...
		::EOS_P2P_RemoveNotifyPeerConnectionRequest( m_p2PHandle, m_notificationIdConnectionRequest );
	}
};
} // namespace syscross::HelloEOS::Deferred::ConnectionRequestListener
