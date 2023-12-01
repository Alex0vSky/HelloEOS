// src\Async\TickerCore.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::detail_ {
struct TickerCore {
	std::shared_ptr< Selector::Multiplexer > m_mux;
	Selector::multiplex_t m_multiplexer;
	Selector::demultiplex_t m_demultiplexer;
	Environs m_ctx;
	GradualExecutor m_executor;
	typedef std::unique_ptr< TickerCore > uptr_t;
	TickerCore(PrepareEos::prepared_t const& oes) :
		m_mux( std::make_shared< Selector::Multiplexer >( ) )
		, m_multiplexer( m_mux )
		, m_demultiplexer( m_mux )
		, m_ctx{ oes ->m_platformHandle, oes ->m_auth ->getLocalUserId( ), oes ->m_mapping ->getFriendLocalUserId( ) }
		, m_executor( m_demultiplexer, m_ctx )
	{}
};
} // namespace syscross::HelloEOS::Async::detail_
