// src\Async\Thread\Factory.h - waiting for the "EOS SDK" to initialize until the `clockTimeout`.
#pragma once // Copyright 2024 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Thread {
struct Factory {
	typedef std::unique_ptr< detail_::GameThread > gameThread_t;
	static gameThread_t create(bool isServer) {
		using namespace std::literals::chrono_literals;
		const auto futureTimeout = std::future_status::timeout;
		static constexpr auto now = std::chrono::system_clock::now;

		// give out only parent to hide JThread from the user
		detail_::JThread *async = new detail_::JThread( isServer );
		gameThread_t gameThread( async );

		// waiting for "EOS SDK" to initialize in a separate thread
		auto &future = async ->getFuture( );
		std::future_status stat = std::future_status::ready;
		auto clockTimeout = now( ) + 15s;
		while ( true 
			&& !async ->isPrepared( )
			&& now( ) > clockTimeout
			&& ( futureTimeout == ( stat = future.wait_for( 300ms ) ) )
		) (void)0;

		if ( futureTimeout == stat )
			return gameThread;
		// void or rethrow exception
		future.get( );
		return { };
	}
};
} // namespace syscross::HelloEOS::Async::Thread
