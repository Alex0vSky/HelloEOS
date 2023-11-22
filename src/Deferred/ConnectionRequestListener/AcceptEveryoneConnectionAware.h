// src\Deferred\ConnectionRequestListener\AcceptEveryoneConnectionAware.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred::ConnectionRequestListener {
class AcceptEveryoneConnectionAware : public AcceptEveryone {
	EOS_NotificationId m_notificationIdConnectionEstablished = EOS_INVALID_NOTIFICATIONID;
	EOS_NotificationId m_notificationIdConnectionInterrupted = EOS_INVALID_NOTIFICATIONID;
	EOS_NotificationId m_notificationIdConnectionClosed = EOS_INVALID_NOTIFICATIONID;
	bool m_bEstablished = false;

	static void EOS_CALL ConnectionEstablishedHandler_(const EOS_P2P_OnPeerConnectionEstablishedInfo* Data) {
		if ( !Data )
			return;
		AcceptEveryoneConnectionAware *self = reinterpret_cast<AcceptEveryoneConnectionAware *>( Data ->ClientData );
		self ->m_bEstablished = true;
		LOG( "[AcceptEveryoneConnectionAware] ConnectionEstablishedHandler_" );
	}
	static void EOS_CALL ConnectionInterruptedHandler_(const EOS_P2P_OnPeerConnectionInterruptedInfo* Data) {
		if ( !Data )
			return;
		AcceptEveryoneConnectionAware *self = reinterpret_cast<AcceptEveryoneConnectionAware *>( Data ->ClientData );
		self ->m_bEstablished = false;
		LOG( "[AcceptEveryoneConnectionAware] ConnectionInterruptedHandler_" );
	}
	static void EOS_CALL ConnectionClosedHandler_(const EOS_P2P_OnRemoteConnectionClosedInfo* Data) {
		if ( !Data )
			return;
		AcceptEveryoneConnectionAware *self = reinterpret_cast<AcceptEveryoneConnectionAware *>( Data ->ClientData );
		self ->m_bEstablished = false;
		LOG( "[AcceptEveryoneConnectionAware] ConnectionClosedHandler_" );
	}

public:
	AcceptEveryoneConnectionAware(const Ctx &ctx) :
		AcceptEveryone( ctx )
	{
		//return;
		{
			::EOS_P2P_AddNotifyPeerConnectionEstablishedOptions options = { EOS_P2P_ADDNOTIFYPEERCONNECTIONESTABLISHED_API_LATEST };
			options.LocalUserId = m_LocalUserId;
			options.SocketId = &m_SocketId;
			m_notificationIdConnectionEstablished = ::EOS_P2P_AddNotifyPeerConnectionEstablished(
				m_p2PHandle, &options, this, ConnectionEstablishedHandler_ );
		}
		{
			::EOS_P2P_AddNotifyPeerConnectionInterruptedOptions options = { EOS_P2P_ADDNOTIFYPEERCONNECTIONINTERRUPTED_API_LATEST };
			options.LocalUserId = m_LocalUserId;
			options.SocketId = &m_SocketId;
			m_notificationIdConnectionInterrupted = ::EOS_P2P_AddNotifyPeerConnectionInterrupted(
				m_p2PHandle, &options, this, ConnectionInterruptedHandler_ );
		}
		{
			::EOS_P2P_AddNotifyPeerConnectionClosedOptions options = { EOS_P2P_ADDNOTIFYPEERCONNECTIONCLOSED_API_LATEST };
			options.LocalUserId = m_LocalUserId;
			options.SocketId = &m_SocketId;
			m_notificationIdConnectionClosed = ::EOS_P2P_AddNotifyPeerConnectionClosed(
				m_p2PHandle, &options, this, ConnectionClosedHandler_ );
		}
	}
	~AcceptEveryoneConnectionAware() {
		::EOS_P2P_RemoveNotifyPeerConnectionEstablished( m_p2PHandle, m_notificationIdConnectionEstablished );
		::EOS_P2P_RemoveNotifyPeerConnectionInterrupted( m_p2PHandle, m_notificationIdConnectionInterrupted );
		::EOS_P2P_RemoveNotifyPeerConnectionClosed( m_p2PHandle, m_notificationIdConnectionClosed );

	}

	bool isEstablished() const {
		return m_bEstablished;
	}
};
} // namespace syscross::HelloEOS::Deferred::ConnectionRequestListener
