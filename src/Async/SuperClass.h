// src\Async\SuperClass.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async {
class SuperClass {
	friend class Send;
	const std::string m_socketName;
	EOS_ProductUserId const m_localUserId, m_friendLocalUserId;
	enum class Direction { Outgoing, Incoming };
	typedef std::packaged_task< Networking::messageData_t() > packaged_task_t;
	typedef std::pair< packaged_task_t, Direction > queue_elem_t;
	inline static std::queue< queue_elem_t > m_fifo;

public:
	const EOS_HPlatform m_platformHandle;
	SuperClass(Async::PrepareEos::Prepared const*eos, std::string const& socketName) : 
		m_socketName( socketName )
		, m_platformHandle( eos ->m_platformHandle )
		, m_localUserId( eos ->m_auth ->getLocalUserId( ) )
		, m_friendLocalUserId( eos ->m_mapping ->getFriendLocalUserId( ) )
	{}

	// TODO(alex): To separate class queue
	// TODO(alex): Sync impl to derived class
	inline static std::mutex m_mu;
	void outgoing(packaged_task_t &&task) {
		std::lock_guard lock( m_mu );
		m_fifo.push( std::make_pair( std::move( task ), Direction::Outgoing ) );
	}
	void incoming(packaged_task_t &&task) {
		std::lock_guard lock( m_mu );
		m_fifo.push( std::make_pair( std::move( task ), Direction::Incoming ) );
	}
	void processAll() {
		std::lock_guard lock( m_mu );
	}
	// tmp 
	auto pop() {
		if ( m_fifo.empty( ) )
			return std::optional<packaged_task_t>{ };
		queue_elem_t x = std::move( m_fifo.front( ) );
		m_fifo.pop( );
		return std::optional<packaged_task_t>( std::move( x.first ) );
	}
};
} // namespace syscross::HelloEOS::Async
