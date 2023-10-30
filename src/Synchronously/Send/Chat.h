// src\Synchronously\Send\Chat.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously::Send {
class Chat : public BaseSend {
	using BaseSend::BaseSend;

public:
	bool message(const std::string &text, bool bTicks = true) {
		if ( !sendPacket_( text ) )
			return false;
		// TODO(alex): no way to known deliverance now... If presence to Offline?
		if ( bTicks ) {
			LOG( "[~] press [Ctrl+C] to exit" );
			ticks_( );
		}
		return true;
	}
};
} // namespace syscross::HelloEOS::Synchronously::Send
