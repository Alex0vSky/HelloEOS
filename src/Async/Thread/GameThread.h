// src\Async\Thread\GameThread.h - EOS calls, "[LogEOSRTC] LibRTCCore: 'InGameThreadChecker' has been alarmed. ExpectedGameThread=[Id=[6308]] UsedThread=[Id=[1520] Name=[]]", "[LogEOSRTC] RTCPlatform Failed to tick LibRTC. ResultCode=[NotGameThread] Description=[The current thread is wrong, this API must be used only from the game thread]"
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Thread::detail_ { 
class GameThread {
	const bool m_isServer;
	using TickerCore = Async::detail_::TickerCore;
	TickerCore::uptr_t m_core;

protected:
	std::atomic_bool m_bPrepared = false, m_bStop = false;
	void run_() {
		A0S_SetThreadName( "GameThread" )
		auto oes = Async::detail_::PrepareEos::ordinary( m_isServer );
		if ( !oes )
			return;
		m_core = TickerCore::uptr_t( new TickerCore( oes ) );
		// ugly, but need only this thread for init and working
		m_bPrepared = true;
		while ( !m_bStop ) {
			std::this_thread::sleep_for( std::chrono::microseconds{ 300 } );
			m_core ->m_executor.all( );
			::EOS_Platform_Tick( m_core ->m_ctx.m_platformHandle );
		}
	}
	explicit GameThread(bool isServer) : 
		 m_isServer( isServer )
	{}

public: 
	virtual ~GameThread() {}
	Transport::Send createSender(std::string const& m_socketName) const {
		return Transport::Send( m_core ->m_ctx, m_socketName, m_core ->m_multiplexer );
	}
	Transport::Recv createReceiver(std::string const& m_socketName) const {
		return Transport::Recv( m_core ->m_ctx, m_socketName, m_core ->m_multiplexer );
	}
};
} // namespace syscross::HelloEOS::Async::Thread::detail_
