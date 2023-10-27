// src\Anchronously\Send\Chat.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Anchronously::Send {
class Chat : public Synchronously::Send::BaseSend {
	using BaseSend::BaseSend;

public:
	Networking::send_t message(const std::string &text) {
		auto future = std::async( &Chat::sendPacket_< std::decay_t< decltype( text ) > >, this, text );
		return future;
	}
};
} // namespace syscross::HelloEOS::Anchronously::Send
