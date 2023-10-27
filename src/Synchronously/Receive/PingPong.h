// src\Synchronously\Receive\PingPong.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously::Receive {
class PingPong : public BaseReceive, public Send::BaseSend {
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
	bool recvPingAndAnswerPong() {
		Networking::messageData_t messageData = receive_( );
		std::string stringRecv( messageData.begin( ), messageData.end( ) );
		LOG( "[recvPingAndAnswerPong] stringRecv: '%s'", stringRecv.c_str( ) );
		if ( 0 != stringRecv.compare( "PING" ) ) {
			LOG( "[recvPingAndAnswerPong] fail ping" );
			return false;
		}

		std::string stringPong = "PONG";
		messageData.assign( stringPong.begin( ), stringPong.end( ) );
		if ( !sendPacket_( messageData ) ) {
			LOG( "[recvPingAndAnswerPong] fail pong" );
			return false;
		}

		LOG( "[~] press [Ctrl+C] to exit" );
		ticks_( );

		return true;
	}
};
} // namespace syscross::HelloEOS::Synchronously::Receive
