// src\Anchronously\Ping.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Anchronously {
class Ping : public Synchronously::Receive::BaseReceive, public Synchronously::Send::BaseSend {

public:
	Ping( 
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
	Networking::ping_t measure(std::chrono::milliseconds *pDuration) {
		return { };
	}
};
} // namespace syscross::HelloEOS::Anchronously
