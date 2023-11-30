// src\Async\SuperclassTicker.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async {
struct SuperclassTicker {
	std::shared_ptr< Multiplexer > m_mux;
	multiplex_t m_multiplexer;
	demultiplex_t m_demultiplexer;
	Async::Context m_ctx;
	GradualExecutor m_executor;
	typedef std::unique_ptr< SuperclassTicker > uptr_t;
	SuperclassTicker(PrepareEos::prepared_t const& oes) :
		m_mux( std::make_shared< Multiplexer >( ) )
		, m_multiplexer( m_mux )
		, m_demultiplexer( m_mux )
		, m_ctx{ oes ->m_platformHandle, oes ->m_auth ->getLocalUserId( ), oes ->m_mapping ->getFriendLocalUserId( ) }
		, m_executor( m_demultiplexer, m_ctx )
	{}
};
} // namespace syscross::HelloEOS::Async
