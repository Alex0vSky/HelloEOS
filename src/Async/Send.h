// src\Async\Send.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async {
class Send {
	Async::Context m_ctx;
	multiplex_t m_multiplexer = nullptr;
	typedef Networking::messageData_t messageData_t;

public:
	Send(Async::Context const& ctx, multiplex_t multiplexer) :
		m_ctx( ctx )
		, m_multiplexer( multiplexer )
	{}
	typedef std::future<messageData_t> future_t;
	[[nodiscard]] future_t text(std::string const& text) {
		std::packaged_task< messageData_t() > task = 
			std::packaged_task( [this, &text]() ->messageData_t
			{
				::EOS_Platform_GetP2PInterface( m_ctx.m_platformHandle );
				//const EOS_HP2P m_p2PHandle = ::EOS_Platform_GetP2PInterface( m_ctx.m_platformHandle );
				//EOS_P2P_SendPacketOptions m_options{ EOS_P2P_SENDPACKET_API_LATEST };
				//EOS_EResult result = ::EOS_P2P_SendPacket( m_p2PHandle, &m_options );
				return { 'h', 'e', 'l', 'l', 'o' };
			});
		auto future = task.get_future( );
		m_multiplexer ->outgoing( std::move( task ) );
		return future;
	}
};
} // namespace syscross::HelloEOS::Async
