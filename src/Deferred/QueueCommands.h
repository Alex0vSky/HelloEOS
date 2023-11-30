// src\Deferred\QueueCommands.h - better then shared_ptr passing everywhere
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred {
namespace detail_ { template <typename F, typename... Ts> class Action; } // namespace detail_
class QueueCommands {
	static constexpr std::chrono::seconds c_commandTO{ 30 };
	//static constexpr std::chrono::milliseconds c_sleep{ 15 };
	static constexpr std::chrono::milliseconds c_sleep{ 200 };
	static constexpr auto now = std::chrono::system_clock::now;
	EOS_HPlatform m_platformHandle;
	EOS_HP2P m_p2PHandle;
	EOS_P2P_GetPacketQueueInfoOptions m_queueVer = { EOS_P2P_GETPACKETQUEUEINFO_API_LATEST };
	// Shortcut
	uint64_t m_IncomingSize = 0;
	uint64_t m_OutgoingSize = 0;
	// Only for friends
	struct AvoidPush { bool Yes; } m_avoidPush{ true };

	QueueCommands(EOS_HPlatform platformHandle) :
		m_platformHandle( platformHandle )
		, m_p2PHandle( EOS_Platform_GetP2PInterface( platformHandle ) )
	{
		// TODO(alex): tmp comment
		//if ( nullptr == platformHandle )
		//	throw std::runtime_error{ "not initialized" };
		LOG( "[!] QueueCommands tmp comment" );
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
		virtual ~ICommand() {}
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
	template <typename, typename...> friend class detail_::Action;
	friend class Receiving;
	friend class Sending;
	void push(const sptr_t &p) {
		m_fifo.push( p );
	}
	void tick() {
		::EOS_Platform_Tick( m_platformHandle );
	}
	void getQueueInfo() {
		EOS_P2P_PacketQueueInfo queueInfo = { };
		auto r = ::EOS_P2P_GetPacketQueueInfo( m_p2PHandle, &m_queueVer, &queueInfo );
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

public:
	static void init(EOS_HPlatform platformHandle) {
		getInstanceImpl( platformHandle );
	}
	static QueueCommands& instance() {
		return getInstanceImpl( );
	}
	auto ticksAll() {
		if ( m_fifo.size( ) )
			LOG( "[ticksAll] fifo size: %zd", m_fifo.size( ) );
		std::vector< Networking::messageData_t > allIncomingData;
		sptr_t command;
		unsigned int count = 0;
		while ( command = pop( ) ) {
			++count;
			std::string direction = directionToString( command );
			// If there is already an open connection to this peer, it will be sent immediately
			if ( Direction::Outgoing == command ->getDirection( ) ) {
				command ->act( m_avoidPush );
				// TODO(alex): process "[LogEOSP2P] Connection closed. LocalUserId=[000...6e2] RemoteUserId=[000...7e5] SocketId=[CHAT] Reason=[TimedOut]"
				tick( );
				getQueueInfo( );
				if ( !m_OutgoingSize ) {
					LOG( "[ticksAll] Outgoing, already executed command #%d", count );
					continue;
				}
				LOG( "[ticksAll] %s, process bytes: %I64d", direction.c_str( ), m_OutgoingSize );
				while ( m_OutgoingSize ) {
					std::this_thread::sleep_for( c_sleep );
					tick( );
					getQueueInfo( );
				}
			}

			// +-TODO(alex): timeout and return bool
			bool isContinuation = true;
			auto timeout = now( ) + c_commandTO;
			if ( Direction::Incoming == command ->getDirection( ) ) {
				tick( );
				getQueueInfo( );
				//LOG( "[ticksAll] %s, process bytes: %I64d", direction.c_str( ), m_IncomingSize );
				Networking::messageData_t packet = { };
				do {
					packet = command ->act( m_avoidPush );
					if ( packet.size( ) )
						break;
					tick( );
					getQueueInfo( );
					std::this_thread::sleep_for( c_sleep );
					isContinuation = ( now( ) < timeout );
				} while ( isContinuation );
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
