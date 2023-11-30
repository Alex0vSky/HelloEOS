// src\Async\Thread\JThread.h - naive
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Thread {
class JThread : public Ticker {
	std::thread m_thread;

public: 
	JThread(bool isServer) :
		 Ticker( isServer )
	{
		// TODO(alex): auto future = std::async( task1 ); https://akrzemi1.wordpress.com/2011/09/21/destructors-that-throw/
		m_thread = std::thread{ &JThread::run_, this };
	}
	~JThread() {
		Ticker::m_bStop = true, m_thread.join( ); // C++20 std::jthread?
	}
};
} // namespace syscross::HelloEOS::Async::Thread
