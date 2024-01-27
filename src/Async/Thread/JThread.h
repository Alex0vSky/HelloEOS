// src\Async\Thread\JThread.h - naive
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Thread { struct FactoryInfiniteWait; } // forward decl
namespace syscross::HelloEOS::Async::Thread::detail_ {
class JThread final : public GameThread {
	std::future<void> m_future;

public: 
	JThread(bool isServer) :
		 GameThread( isServer )
	{
		// convenient, but leakage may occur if the process is quickly exited
		m_future = std::async( &JThread::run_, this );
	}
	~JThread() {
		if ( m_future.valid( ) ) 
			GameThread::m_bStop = true, m_future.wait( );
	}
	bool isPrepared() const {
		return GameThread::m_bPrepared;
	}
	auto &getFuture() {
		return m_future;
	}
};
} // namespace syscross::HelloEOS::Async::Thread::detail_
