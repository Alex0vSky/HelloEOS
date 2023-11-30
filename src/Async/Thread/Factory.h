// src\Async\Thread\Factory.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Thread {
struct FactoryInfiniteWaiting {
	typedef std::unique_ptr< JThread > ticker_t;
	static ticker_t createTicker(bool isServer) {
		auto ticker = ticker_t( new JThread( isServer ) );
		while ( !ticker ->m_bPrepared && !ticker ->m_bError )
			std::this_thread::yield( );
		if ( ticker ->m_bError )
			return { };
		return ticker;
	}
};
} // namespace syscross::HelloEOS::Async::Thread
