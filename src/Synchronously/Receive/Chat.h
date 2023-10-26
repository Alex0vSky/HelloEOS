// src\Synchronously\Receive\Chat.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously::Receive {
class Chat : public BaseReceive {
	using BaseReceive::BaseReceive;

public:
	std::string getMessage() {
		Networking::messageData_t messageData = receive_( );
		return std::string( messageData.begin( ), messageData.end( ) );
	}
};
} // namespace syscross::HelloEOS::Synchronously::Receive
