// src\Async\Thread\Ticker.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Thread { class Ticker {
	friend struct FactoryInfiniteWaiting;
	const bool m_isServer;
	std::atomic_bool m_bPrepared = false, m_bError = false;
	//PrepareEos::prepared_t m_oes;
	//std::unique_ptr< Async::Context > m_ctx;
	//multiplex_t m_multiplexer;
	//Executor m_executor;
	////static constexpr auto asd = std::literals::chrono_literals::operator"" s;
	struct SuperclassTicker {
		multiplex_t m_multiplexer;
		Async::Context m_ctx;
		Executor m_executor;
		PrepareEos::prepared_t m_oes;
		// TODO(alex): makeme and test client-server
		//Deferred::ConnectionRequestListener::AcceptEveryoneConnectionAware m_acceptor;
		typedef std::unique_ptr< SuperclassTicker > inner_t;
		SuperclassTicker(PrepareEos::prepared_t const& oes) :
			m_multiplexer( std::make_shared< Multiplexer >( ) )
			, m_ctx{ oes ->m_platformHandle, "", oes ->m_auth ->getLocalUserId( ), oes ->m_mapping ->getFriendLocalUserId( ) }
			, m_executor( m_multiplexer, m_ctx )
			, m_oes( oes )
			//, m_acceptor( m_ctx )
		{}
	};
	SuperclassTicker::inner_t m_inner;

protected:
	std::atomic_bool m_bStop = false;
	void run_() {
		A0S_SetThreadName( "MainGameThread" );
		auto m_oes = Async::PrepareEos::ordinary( m_isServer );
		if ( !m_oes )
			return m_bError = true, (void)0;
		// TODO(alex): on this stage is better to create subclass, for example `Executor m_executor` not completely ready for working
		// "object is potentially in an undefined state"
		// to ommit init can place in `union` @insp https://stackoverflow.com/questions/2464296/is-it-possible-to-defer-member-initialization-to-the-constructor-body
		// can move this members to another class
		// `PrepareEos::prepared_t m_oes;`
		// `std::unique_ptr< Async::Context > m_ctx;`
		// `multiplex_t m_multiplexer;`
		// `Executor m_executor;`
		m_inner = SuperclassTicker::inner_t( new SuperclassTicker( m_oes ) );
		// ugly, but need only this thread for init and working
		m_bPrepared = true;
		//m_ctx = std::unique_ptr< Async::Context >( new Async::Context{
		//			m_oes ->m_platformHandle
		//			, ""
		//			, m_oes ->m_auth ->getLocalUserId( )
		//			, m_oes ->m_mapping ->getFriendLocalUserId( )
		//		} );
		while ( !m_bStop ) {
			std::this_thread::sleep_for( 300ms );
			//m_executor.processAll( );
			m_inner ->m_executor.processAll( );
		}
	}
	explicit Ticker(bool isServer) : 
		 m_isServer( isServer )
		//, m_multiplexer( std::make_shared< Multiplexer >( ) )
		//, m_executor( m_multiplexer )
	{}

public: 
	Async::Send createSender(std::string const& m_socketName) const {
		// TODO(alex): ??? what about Factory?
		if ( !m_bPrepared )
			throw std::runtime_error( "undefined state object" );
		Async::Context ctx  = this ->m_inner ->m_ctx;
		const_cast< std::string & >( ctx.m_socketName ) = m_socketName;
		return Async::Send( ctx, this ->m_inner ->m_multiplexer );
	}
};
} // namespace syscross::HelloEOS::Async::Thread
