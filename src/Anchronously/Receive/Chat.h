// src\Anchronously\Receive\Chat.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Anchronously::Receive {
class Chat : public Synchronously::Receive::BaseReceive {
	using BaseReceive::BaseReceive;

public:
	Networking::recv_t getMessage() {
		auto future = std::async( &Chat::receive_, this, false );
		return future;
	}
};
} // namespace syscross::HelloEOS::Anchronously::Receive
