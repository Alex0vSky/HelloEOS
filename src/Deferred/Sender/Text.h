// src\Deferred\Sender\Text.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred::Sender {
struct Text : public Synchronously::Send::BaseSend {
	using BaseSend::BaseSend;
	//static constexpr auto sendTextPacket = &BaseSend::sendTextPacket_;
	using BaseSend::sendTextPacket_;
	~Text() {
	}
};
} // namespace syscross::HelloEOS::Deferred::Sender
