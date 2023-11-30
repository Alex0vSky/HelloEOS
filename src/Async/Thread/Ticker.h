// src\Async\Thread\Ticker.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Thread {
class Ticker {
	eos_t m_prepared;
	std::atomic_bool m_bPrepared = false;
	multiplex_t m_multiplex;

	std::atomic_bool m_bStop = false;
	std::thread m_thread;
	// +TODO(alex): its demultiplexer???
// In computing, I/O multiplexing can also be used to refer to the concept of processing multiple input/output events from a single event loop, with system calls like poll[1] and select (Unix).[2]
// https://habr.com/ru/companies/lineate/articles/585050/
// https://habr.com/ru/companies/lineate/articles/674144/

	//ContextFactory m_contextFactory;
	void run_() {
#ifdef _DEBUG
		::SetThreadDescription( ::GetCurrentThread( ), L"MainGameThread" );
#endif
		// +TODO(alex): move preparing EOS to here
		auto prepared = Async::PrepareEos::ordinary( m_startup.m_isServer );
		if ( !prepared ) 
			return;
		m_prepared = std::make_shared< PrepareEos::Prepared >( std::move( prepared.value( ) ) );
		//m_contextFactory = ContextFactory( m_prepared );
		m_bPrepared = true;
		// TODO(alex): auto future = std::async( task1 ); https://akrzemi1.wordpress.com/2011/09/21/destructors-that-throw/
		while ( !m_bStop ) {
			std::this_thread::sleep_for( std::chrono::milliseconds{ 300 } );
			//m_startup.ctx.processAll( );
			auto task = m_multiplex ->pop( );
			if ( task ) {
				task.value( )( );
			}
			::EOS_Platform_Tick( m_prepared ->m_platformHandle );
		}
	}
	struct Startup {
		const bool m_isServer;
	} m_startup;

	bool z; explicit Ticker(Startup const& startup) : z( false )
		 , m_startup( startup )
		 , m_multiplex( std::make_shared< Multiplex >( ) )
		 //, m_contextFactory( nullptr )
	{
		m_thread = std::thread{ &Ticker::run_, this };
	}

public: 
	~Ticker() {
		Ticker::m_bStop = true, m_thread.join( ); // C++20 std::jthread?
	}
	typedef std::unique_ptr< class Ticker > ticker_t;
	static ticker_t createTicker(Startup const& startup) {
		auto ticker = ticker_t( new Ticker( startup ) );
		while ( !ticker ->m_bPrepared )
			std::this_thread::yield( );
		return ticker;
	}

	//// poc
	//class Factory {
	//	Ticker *m_ticker;
	//	std::string m_socketName;
	//public:
	//	Factory(Ticker *ticker, std::string const& socketName) :
	//		m_ticker( ticker ) 
	//		, m_socketName( socketName ) 
	//	{
	//		std::cout << "Factory\n";
	//	}
	//	Async::Send createSender() {
	//		//return Async::Send({ m_ticker ->m _prepared.m_platformHandle
	//		//		, m_socketName
	//		//		, m_ticker ->m _prepared.m_auth ->getLocalUserId( )
	//		//		, m_ticker ->m _prepared.m_mapping ->getFriendLocalUserId( )
	//		//	});
	//		//return Async::Send( m_ticker ->g etContext( m_socketName ), m_ticker );
	//		return Async::Send( m_ticker ->g etContext( m_socketName ), m_ticker ->m_multiplex );
	//	}
	//	// chain usage only
	//	Factory(Factory&&) = delete;
	//	//Factory(const Factory&) = delete;
	//	//Factory& operator=(Factory&&) = delete;
	//	//Factory& operator=(const Factory&) = delete;
	//};
//#pragma warning( push )
//#pragma warning( disable: 4172 )
//	auto &&getFactory(std::string const& m_socketName) {
//		return Factory{ this, m_socketName };
//	}
//#pragma warning( pop )	
	Async::Send createSender(std::string const& m_socketName) {
		// -TODO(alex): doing req waitEos() mandatory or reuse thread in factory::createTicker() and run()
		//waitEos( );
		//return Async::Send( m_contextFactory.getContext( m_socketName ), this ->m_multiplex );
		return Async::Send( 
				Async::Context{
					m_prepared ->m_platformHandle
					, m_socketName
					, m_prepared ->m_auth ->getLocalUserId( )
					, m_prepared ->m_mapping ->getFriendLocalUserId( )
				}
				, this ->m_multiplex 
			);
	}

};
} // namespace syscross::HelloEOS::Async::Thread
