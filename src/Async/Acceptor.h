// src\Async\Acceptor.h - naive, has nothing to do with asynchrony.
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::detail_ {
class Acceptor {
	const EOS_ProductUserId m_localUserId;
	const std::string m_socketName;
	const EOS_HP2P m_p2PHandle;
	EOS_P2P_SocketId m_socketId;
	EOS_NotificationId m_idRequest = EOS_INVALID_NOTIFICATIONID;
	EOS_NotificationId m_idEstablished = EOS_INVALID_NOTIFICATIONID;
	EOS_NotificationId m_idInterrupted = EOS_INVALID_NOTIFICATIONID;
	EOS_NotificationId m_idClosed = EOS_INVALID_NOTIFICATIONID;
	bool m_bEstablished = false;

	template <typename T>
	static Acceptor *cast_(T const* Data) {
		return reinterpret_cast< Acceptor * >( Data ->ClientData );
	}
	void checkId_(EOS_NotificationId notificationId, char str[]) {
		if ( notificationId == EOS_INVALID_NOTIFICATIONID ) 
			throw std::runtime_error( str );
	}
	static void EOS_CALL Request_(EOS_P2P_OnIncomingConnectionRequestInfo const* Data) {
		if ( !Data )
			return;
		LOG( "[Acceptor] connection request on {%s}", Data ->SocketId ->SocketName );
		Acceptor *self = cast_( Data );
		EOS_P2P_AcceptConnectionOptions options = { EOS_P2P_ACCEPTCONNECTION_API_LATEST };
		options.LocalUserId = self ->m_localUserId;
		options.RemoteUserId = Data->RemoteUserId;
		options.SocketId = &self ->m_socketId;
		EOS_EResult result = ::EOS_P2P_AcceptConnection( self ->m_p2PHandle, &options );
		if ( EOS_EResult::EOS_Success != result ) 
			throw std::runtime_error( "error EOS_P2P_AcceptConnection" );
		LOG( "[Acceptor] accepted on {%s}", Data ->SocketId ->SocketName );
	}
	static void EOS_CALL Established_(EOS_P2P_OnPeerConnectionEstablishedInfo const* Data) {
		if ( !Data )
			return;
		cast_( Data ) ->m_bEstablished = true;
		LOG( "[Acceptor] connection established on {%s}", Data ->SocketId ->SocketName );
	}
	static void EOS_CALL Interrupted_(EOS_P2P_OnPeerConnectionInterruptedInfo const* Data) {
		if ( !Data )
			return;
		cast_( Data ) ->m_bEstablished = false;
		LOG( "[Acceptor] connection interrupted on {%s}", Data ->SocketId ->SocketName );
	}
	static void EOS_CALL Closed_(EOS_P2P_OnRemoteConnectionClosedInfo const* Data) {
		if ( !Data )
			return;
		cast_( Data ) ->m_bEstablished = false;
		LOG( "[Acceptor] connection closed on {%s}", Data ->SocketId ->SocketName );
	}

public:
	Acceptor(Environs const&ctx, std::string const& socketName) :
		m_localUserId( ctx.m_localUserId )
		, m_socketName( socketName )
		, m_p2PHandle( ::EOS_Platform_GetP2PInterface( ctx.m_platformHandle ) )
		, m_socketId{ EOS_P2P_SOCKETID_API_LATEST }
	{
		strcpy_s( m_socketId.SocketName , m_socketName.c_str( ) );

		{
			::EOS_P2P_AddNotifyPeerConnectionRequestOptions options = { EOS_P2P_ADDNOTIFYPEERCONNECTIONREQUEST_API_LATEST };
			options.LocalUserId = m_localUserId;
			options.SocketId = &m_socketId;
			m_idRequest = ::EOS_P2P_AddNotifyPeerConnectionRequest(
				m_p2PHandle, &options, this, Request_ );
			checkId_( m_idRequest, "error EOS_P2P_AddNotifyPeerConnectionRequest" );
		}
		{
			::EOS_P2P_AddNotifyPeerConnectionEstablishedOptions options = { EOS_P2P_ADDNOTIFYPEERCONNECTIONESTABLISHED_API_LATEST };
			options.LocalUserId = m_localUserId;
			options.SocketId = &m_socketId;
			m_idEstablished = ::EOS_P2P_AddNotifyPeerConnectionEstablished(
				m_p2PHandle, &options, this, Established_ );
			checkId_( m_idRequest, "error EOS_P2P_AddNotifyPeerConnectionEstablished" );
		}
		{
			::EOS_P2P_AddNotifyPeerConnectionInterruptedOptions options = { EOS_P2P_ADDNOTIFYPEERCONNECTIONINTERRUPTED_API_LATEST };
			options.LocalUserId = m_localUserId;
			options.SocketId = &m_socketId;
			m_idInterrupted = ::EOS_P2P_AddNotifyPeerConnectionInterrupted(
				m_p2PHandle, &options, this, Interrupted_ );
			checkId_( m_idRequest, "error EOS_P2P_AddNotifyPeerConnectionInterrupted" );
		}
		{
			::EOS_P2P_AddNotifyPeerConnectionClosedOptions options = { EOS_P2P_ADDNOTIFYPEERCONNECTIONCLOSED_API_LATEST };
			options.LocalUserId = m_localUserId;
			options.SocketId = &m_socketId;
			m_idClosed = ::EOS_P2P_AddNotifyPeerConnectionClosed(
				m_p2PHandle, &options, this, Closed_ );
			checkId_( m_idRequest, "error EOS_P2P_AddNotifyPeerConnectionClosed" );
		}
	}
	~Acceptor() {
		::EOS_P2P_RemoveNotifyPeerConnectionRequest( m_p2PHandle, m_idRequest );
		::EOS_P2P_RemoveNotifyPeerConnectionEstablished( m_p2PHandle, m_idEstablished );
		::EOS_P2P_RemoveNotifyPeerConnectionInterrupted( m_p2PHandle, m_idInterrupted );
		::EOS_P2P_RemoveNotifyPeerConnectionClosed( m_p2PHandle, m_idClosed );
	}
};
} // namespace syscross::HelloEOS::Async::detail_
