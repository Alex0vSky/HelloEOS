// src\Synchronously\Receive\Chat.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously::Receive {
class Chat : public BaseReceive {
	using BaseReceive::BaseReceive;

public:
	std::string getMessage() {
		Networking::optionalMessageData_t messageData = receive_( );
		if ( !messageData )
			return { };
		return std::string( messageData.value( ).begin( ), messageData.value( ).end( ) );
	}
};
} // namespace syscross::HelloEOS::Synchronously::Receive
