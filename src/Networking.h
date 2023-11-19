// src\Networking.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS {
struct Networking {
	typedef std::vector< unsigned char > messageData_t;
	typedef std::optional< messageData_t > optionalMessageData_t;
	typedef std::future< bool > send_t;
	typedef std::future< messageData_t > recv_t;
	typedef std::future< bool > ping_t;

//	1170, [LogEOSP2P] Attempted to send a packet that was too large. LocalUserId=[000...6e2] RemoteUserId=[000...7e5] SocketId=[CHAT] PacketSize=[4096/1170]
	static const size_t c_MaxDataSizeBytes = EOS_P2P_MAX_PACKET_SIZE;

//	( 5*1024*1024 ) / c_MaxDataSizeBytes = [LogEOSP2P] Unable to queue outgoing packet, queue is full! QueueSize=[5242854/5242880] NewPacketSize=[1194]
	static const size_t c_countPackets = ( 1*1024*1024 ) / c_MaxDataSizeBytes;
	// 5242854/1170=4481, 4481*1170=5242770
};
} // namespace syscross::HelloEOS
