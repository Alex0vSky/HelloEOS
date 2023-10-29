// src\Deferred\QueueCommands.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred {
class QueueCommands {
	EOS_HPlatform m_platformHandle;
	QueueCommands(EOS_HPlatform platformHandle) :
		m_platformHandle( platformHandle )
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
		virtual void act() = 0;
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

public:
	static void init(EOS_HPlatform platformHandle) {
		getInstanceImpl( platformHandle );
	}
	static QueueCommands& instance() {
		return getInstanceImpl( );
	}
	void push(const sptr_t &p) {
		m_fifo.push( p );
	}
	void ticksAll() {
		EOS_P2P_GetPacketQueueInfoOptions queueVer = { EOS_P2P_GETPACKETQUEUEINFO_API_LATEST };
		EOS_P2P_PacketQueueInfo queueInfo = { };
		auto m_P2PHandle = ::EOS_Platform_GetP2PInterface( m_platformHandle );
		Deferred::QueueCommands::sptr_t command;
		while ( command = Deferred::QueueCommands::instance( ).pop( ) ) {
			// TODO(alex): timeout
			auto start = std::chrono::system_clock::now( );
			Deferred::QueueCommands::Direction direction = command ->getDirection( );
			if ( Deferred::QueueCommands::Direction::Outgoing == direction ) {
				::EOS_P2P_GetPacketQueueInfo( m_P2PHandle, &queueVer, &queueInfo );
				uint64_t outgoingSize = queueInfo.OutgoingPacketQueueCurrentSizeBytes;
				LOG( "[ticks] ready drain bytes: %I64d", outgoingSize );
				while ( outgoingSize ) {
					::EOS_Platform_Tick( m_platformHandle );
					::EOS_P2P_GetPacketQueueInfo( m_P2PHandle, &queueVer, &queueInfo );
					// can be accidentialy increased underhood EOS
					if ( outgoingSize != queueInfo.OutgoingPacketQueueCurrentSizeBytes ) {
						LOG( "[ticks] left: %I64d", queueInfo.OutgoingPacketQueueCurrentSizeBytes );
						outgoingSize = queueInfo.OutgoingPacketQueueCurrentSizeBytes;
					}
					std::this_thread::sleep_for( std::chrono::milliseconds{ 100 } );
				}
				LOG( "[ticks] command complete" );
			}
		}
	}
	// TODO(alex): makeme
	void execute_next() 
	{}
	void ticksWhileOutgoing() {
		if ( m_fifo.empty( ) )
			return;
		ICommand *command = m_fifo.front( ).get( );
		//.isOutgoing;
		command ->getDirection( );
		LOG( "remain commands: " );
	}
	void ticksWhileIncoming() {
		//.isIncoming;
	}

};
} // namespace syscross::HelloEOS::Deferred
