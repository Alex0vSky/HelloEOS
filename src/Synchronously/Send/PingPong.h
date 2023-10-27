// src\Synchronously\Send\PingPong.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously::Send {
class PingPong : public BaseSend, public Receive::BaseReceive {
public:
	PingPong( 
		EOS_HPlatform platformHandle
		, EOS_ProductUserId localUserId
		, EOS_ProductUserId friendLocalUserId
	) : 
		BaseSend( 
			platformHandle
			, localUserId
			, friendLocalUserId
		)
		, BaseReceive( platformHandle, localUserId )
	{}
	bool sendPingWaitPong() {
		const std::string stringPing = "PING";
		Networking::messageData_t messageData( stringPing.begin( ), stringPing.end( ) );
		if ( !sendPacket_( messageData ) ) {
			LOG( "[sendPingWaitPong] fail ping" );
			return false;
		}
		// ticks in recv
		messageData = receive_( );
		if ( messageData.empty( ) ) {
			LOG( "[sendPingWaitPong] fail pong" );
			return false;
		}

		std::string stringRecv( messageData.begin( ), messageData.end( ) );
		LOG( "[sendPingWaitPong] stringRecv: '%s'", stringRecv.c_str( ) );
		return 0 == stringRecv.compare( "PONG" );
	}
};
} // namespace syscross::HelloEOS::Synchronously::Send
