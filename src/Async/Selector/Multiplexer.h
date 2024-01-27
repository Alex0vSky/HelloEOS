// src\Async\Multiplexer.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::detail_::Selector { 
class Multiplexer : public IMux, public IDemux {
	std::queue< Task > m_tasks;
	std::mutex m_mutex;

	std::optional<Task> pop() override { 
		if ( m_tasks.empty( ) )
			return std::nullopt;
		Task task;
		{ std::lock_guard lock( m_mutex );
			task = std::move( m_tasks.front( ) );
			m_tasks.pop( );
		}
		return task;
	}

public:
	void push_outgoing(task_t &&task) override {
		std::lock_guard lock( m_mutex );
		m_tasks.push( { std::move( task ), Direction::Outgoing } );
	}
	void push_incoming(task_t &&task) override {
		std::lock_guard lock( m_mutex );
		m_tasks.push( { std::move( task ), Direction::Incoming } );
	}
	virtual ~Multiplexer() {}
};
} // namespace syscross::HelloEOS::Async::detail_::Selector
