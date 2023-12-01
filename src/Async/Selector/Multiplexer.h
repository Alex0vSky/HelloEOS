// src\Async\Multiplexer.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::detail_::Selector { 
class Multiplexer : public IMux, public IDemux {
	typedef std::pair< task_t, Direction > queue_elem_t;
	std::queue< queue_elem_t > m_fifo;
	std::mutex m_mu;

	bool pop(task_t *p, Direction *direction) override { 
		if ( m_fifo.empty( ) )
			return false;
		queue_elem_t x;
		{ std::lock_guard lock( m_mu );
			x = std::move( m_fifo.front( ) );
			m_fifo.pop( );
		}
		*p = std::move( x.first );
		*direction = x.second;
		return true;
	}

public:
	void outgoing(task_t &&task) override {
		std::lock_guard lock( m_mu );
		m_fifo.push( { std::move( task ), Direction::Outgoing } );
	}
	void incoming(task_t &&task) override {
		std::lock_guard lock( m_mu );
		m_fifo.push( { std::move( task ), Direction::Incoming } );
	}
	virtual ~Multiplexer() {}
};
} // namespace syscross::HelloEOS::Async::detail_::Selector
