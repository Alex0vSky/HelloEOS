// src\Async\Thread\JThread.h - (useless)naive
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Thread {
class JThread : public Ticker {
	std::thread m_thread;

public: 
	JThread(Ticker::Startup const& startup) :
		 Ticker( startup )
	{
		m_thread = std::thread{ &JThread::run_, this };
	}
	~JThread() {
		Ticker::m_bStop = true, m_thread.join( ); // C++20 std::jthread?
	}
};
} // namespace syscross::HelloEOS::Async::Thread
