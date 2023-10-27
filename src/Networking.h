// src\Networking.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS {
struct Networking {
	typedef std::vector< unsigned char > messageData_t;
	typedef std::future< bool > send_t;
	typedef std::future< messageData_t > recv_t;

//	[LogEOSP2P] Attempted to send a packet that was too large. LocalUserId=[000...6e2] RemoteUserId=[000...7e5] SocketId=[CHAT] PacketSize=[4096/1170]
	static const size_t c_MaxDataSizeBytes = 1170;

//	( 5*1024*1024 ) / c_MaxDataSizeBytes = [LogEOSP2P] Unable to queue outgoing packet, queue is full! QueueSize=[5242854/5242880] NewPacketSize=[1194]
	static const size_t c_countPackets = ( 1*1024*1024 ) / c_MaxDataSizeBytes;
};
} // namespace syscross::HelloEOS
