// src\Async\GradualExecutor.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::detail_ {
class GradualExecutor {
	Selector::demultiplex_t m_demultiplexer;
	const EOS_HPlatform m_platformHandle;
	static constexpr auto c_commandTO{ std::chrono::seconds{ 20 } };
	static constexpr auto now = std::chrono::system_clock::now;

public:
	GradualExecutor(Selector::demultiplex_t const& demultiplexer, EosContext const& ctx) :
		m_demultiplexer( demultiplexer )
		, m_platformHandle( ctx.m_platformHandle )
	{}
	void all() {
		using namespace std::literals::chrono_literals;
		while ( auto task = m_demultiplexer ->pop( ) ) {
			if ( Selector::Direction::Outgoing == task ->direction ) {
				task ->task( { } );
				::EOS_Platform_Tick( m_platformHandle );
			}
			const auto timeout = now( ) + c_commandTO;
			if ( Selector::Direction::Incoming == task ->direction ) {
				Selector::task_function_t function( [this, &timeout] { 
						::EOS_Platform_Tick( m_platformHandle );
						std::this_thread::sleep_for( 300ms );
						return ( now( ) < timeout );
					} );
				task ->task( function );
			}
		}
	}
};
} // namespace syscross::HelloEOS::Async::detail_
