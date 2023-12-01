// src\Async\Thread\Factory.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Thread {
struct FactoryInfiniteWait {
	typedef std::unique_ptr< detail_::GameThread > gameThread_t;
	static gameThread_t gameThread(bool isServer) {
		using namespace std::literals::chrono_literals;
		const auto timeout = std::future_status::timeout;
		// hide JThread from user
		auto *p = new detail_::JThread( isServer );
		auto &future = p ->getFuture( );
		auto gameThread = gameThread_t( p );
		std::future_status stat = std::future_status::ready;
		while ( true 
			&& !p ->isValid( )
			&& ( timeout == ( stat = future.wait_for( 300ms ) ) )
		) (void)0;

		if ( timeout == stat )
			return gameThread;
		// void or rethrow exception
		future.get( );
		return { };
	}
};
} // namespace syscross::HelloEOS::Async::Thread
