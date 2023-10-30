// src\Deferred\QueueCommands.h - better then shared_ptr passing
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred {
namespace detail_ { template <typename F, typename... Ts> class Action; } // namespace detail_
class QueueCommands {
	EOS_HPlatform m_platformHandle;
	EOS_HP2P m_P2PHandle;
	EOS_P2P_GetPacketQueueInfoOptions m_queueVer = { EOS_P2P_GETPACKETQUEUEINFO_API_LATEST };
	static constexpr std::chrono::seconds c_commandTO{ 15 };
	static constexpr std::chrono::milliseconds c_sleep{ 100 };

	// Shorten
	uint64_t m_IncomingSize = 0;
	uint64_t m_OutgoingSize = 0;
	// Only for friends
	struct AvoidPush { bool Yes; } m_avoidPush{ true };

	QueueCommands(EOS_HPlatform platformHandle) :
		m_platformHandle( platformHandle )
		, m_P2PHandle( EOS_Platform_GetP2PInterface( platformHandle ) )
	{
		if ( nullptr == platformHandle )
			throw std::runtime_error{ "not initialized" };
	}
	QueueCommands(const QueueCommands &) = delete;
	QueueCommands(QueueCommands &&) = delete;
	QueueCommands& operator=(const QueueCommands &) = delete;
	QueueCommands& operator=(QueueCommands &&) = delete;


public:
	enum class Direction { Outgoing, Incoming };
	struct ICommand {
		virtual Networking::messageData_t act(const QueueCommands::AvoidPush& ) = 0;
		virtual Direction getDirection() const = 0;
	};
	typedef std::shared_ptr< ICommand > sptr_t;

private:
	std::queue< sptr_t > m_fifo;

	static QueueCommands& getInstanceImpl(EOS_HPlatform platformHandle = nullptr) {
		static QueueCommands instance{ platformHandle };
		return instance;
	}

	sptr_t pop() {
		if ( m_fifo.empty( ) )
			return nullptr;
		auto x = m_fifo.front( );
		m_fifo.pop( );
		return x;
	}

	template <typename, typename...>
	friend class detail_::Action;
	void push(const sptr_t &p) {
		m_fifo.push( p );
	}

	void tick() {
		::EOS_Platform_Tick( m_platformHandle );
	}

	void getQueueInfo() {
		EOS_P2P_PacketQueueInfo queueInfo = { };
		auto r = ::EOS_P2P_GetPacketQueueInfo( m_P2PHandle, &m_queueVer, &queueInfo );
		if ( EOS_EResult::EOS_Success != r )
			throw std::runtime_error( "error EOS_P2P_GetPacketQueueInfo" );
		m_IncomingSize = queueInfo.IncomingPacketQueueCurrentSizeBytes;
		m_OutgoingSize = queueInfo.OutgoingPacketQueueCurrentSizeBytes;
	}

	std::string directionToString(const sptr_t &command) {
		if ( Direction::Outgoing == command ->getDirection( ) ) 
			return "Outgoing";
		if ( Direction::Incoming == command ->getDirection( ) ) 
			return "Incoming";
		return { };
	}

	static constexpr auto now = std::chrono::system_clock::now;

	// @insp https://stackoverflow.com/questions/28410697/c-convert-vector-to-tuple
	template <typename T, std::size_t... Indices>
	auto vectorToTupleHelper(const std::vector<T>& v, std::index_sequence<Indices...>) {
		return std::make_tuple(v[Indices]...);
	}
	template <std::size_t N, typename T>
	auto vectorToTuple(const std::vector<T>& v) {
		//assert(v.size() >= N);
		return vectorToTupleHelper(v, std::make_index_sequence<N>());
	}

public:
	static void init(EOS_HPlatform platformHandle) {
		getInstanceImpl( platformHandle );
	}
	static QueueCommands& instance() {
		return getInstanceImpl( );
	}
	auto ticksAll() {
		std::vector< Networking::messageData_t > allIncomingData;
		sptr_t command;
		unsigned int count = 0;
		while ( command = pop( ) ) {
			++count;
			std::string direction = directionToString( command );
			getQueueInfo( );
			if ( !m_OutgoingSize && Direction::Outgoing == command ->getDirection( ) ) {
				LOG( "[ticksAll] already executed command #%d", count );
				continue;
			}
			if ( m_OutgoingSize ) {
				uint64_t lastOutgoingSize = m_OutgoingSize;
				LOG( "[ticksAll] %s, process bytes: %I64d", direction.c_str( ), lastOutgoingSize );
				while ( lastOutgoingSize ) {
					tick( );
					getQueueInfo( );
					// can be accidentialy increased underhood EOS
					if ( lastOutgoingSize != m_OutgoingSize ) {
						if ( m_OutgoingSize )
							LOG( "[ticksAll] %s, left: %I64d", direction.c_str( ), m_OutgoingSize );
						lastOutgoingSize = m_OutgoingSize;
					}
					std::this_thread::sleep_for( c_sleep );
				}
			}

			// +-TODO(alex): timeout and return bool
			bool isContinuation = true;
			auto timeout = now( ) + c_commandTO;
			if ( Direction::Incoming == command ->getDirection( ) ) {
				getQueueInfo( );
				LOG( "[ticksAll] %s, process bytes: %I64d", direction.c_str( ), m_IncomingSize );
				Networking::messageData_t packet = { };
				do {
					packet = command ->act( m_avoidPush );
					if ( packet.size( ) )
						break;
					tick( );
					getQueueInfo( );
					std::this_thread::sleep_for( c_sleep );
				} while ( isContinuation = now( ) < timeout );
				allIncomingData.push_back( packet );
			}
			if ( isContinuation )
				LOG( "[ticksAll] %s, complete command #%d", direction.c_str( ), count );
			else
				LOG( "[ticksAll] %s, timeout command #%d", direction.c_str( ), count );
		}
		return allIncomingData;
	}
};
} // namespace syscross::HelloEOS::Deferred
