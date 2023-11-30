// src\Async\Multiplexer.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async {
class Multiplexer : public IMultiplex {
	typedef std::pair< packaged_task_t, Direction > queue_elem_t;
	std::queue< queue_elem_t > m_fifo;
	std::mutex m_mu;
public:
	void outgoing(packaged_task_t &&task) override {
		std::lock_guard lock( m_mu );
		m_fifo.push( std::make_pair( std::move( task ), Direction::Outgoing ) );
	}
	void incoming(packaged_task_t &&task) {
		std::lock_guard lock( m_mu );
		m_fifo.push( std::make_pair( std::move( task ), Direction::Incoming ) );
	}
	bool pop(packaged_task_t *p, Direction *direction) override { // tmp 
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
	virtual ~Multiplexer() {}
};
} // namespace syscross::HelloEOS::Async
