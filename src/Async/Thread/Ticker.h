// src\Async\Thread\Ticker.h - MainGameThread
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Thread { class Ticker {
	friend struct FactoryInfiniteWaiting;
	const bool m_isServer;
	std::atomic_bool m_bPrepared = false, m_bError = false;
	SuperclassTicker::uptr_t m_impl;

protected:
	std::atomic_bool m_bStop = false;
	void run_() {
		A0S_SetThreadName( "MainGameThread" );
		auto oes = Async::PrepareEos::ordinary( m_isServer );
		if ( !oes )
			return m_bError = true, (void)0;
		m_impl = SuperclassTicker::uptr_t( new SuperclassTicker( oes ) );
		// ugly, but need only this thread for init and working
		m_bPrepared = true;
		while ( !m_bStop ) {
			std::this_thread::sleep_for( 300ms );
			m_impl ->m_executor.all( );
		}
	}
	explicit Ticker(bool isServer) : 
		 m_isServer( isServer )
	{}

public: 
	Async::Transport::Send createSender(std::string const& m_socketName) const {
		return Async::Transport::Send( m_impl ->m_ctx, m_socketName, m_impl ->m_multiplexer );
	}
	Async::Transport::Recv createReceiver(std::string const& m_socketName) const {
		return Async::Transport::Recv( m_impl ->m_ctx, m_socketName, m_impl ->m_multiplexer );
	}
};
} // namespace syscross::HelloEOS::Async::Thread
