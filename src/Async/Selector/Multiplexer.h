// src\Async\Multiplexer.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::detail_::Selector { 
class Multiplexer : public IMux, public IDemux {
	typedef std::pair< task_t, Direction > queue_item_t;
	std::queue< queue_item_t > m_tasks;
	std::mutex m_mutex;

	bool pop(task_t *p, Direction *direction) override { 
		if ( m_tasks.empty( ) )
			return false;
		queue_item_t x;
		{ std::lock_guard lock( m_mutex );
			x = std::move( m_tasks.front( ) );
			m_tasks.pop( );
		}
		*p = std::move( x.first );
		*direction = x.second;
		return true;
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
