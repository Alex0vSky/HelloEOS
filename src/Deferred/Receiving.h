// src\Deferred\Receiving.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred {
class Receiving {
	Ctx m_ctx;
	uint8_t m_channel;

public:
	// For recv need to have instance of Acceptor
	Receiving(Ctx ctx, uint8_t channel, const ConnectionRequestListener::BaseAcceptor &) : 
		m_ctx( ctx )
		, m_channel( channel )
	{}
	auto text(size_t len) {
		if ( len > Networking::c_MaxDataSizeBytes )
			throw std::runtime_error( "exceed maximum packet lenght" );
		auto executor = std::make_shared< Receiver::RecvText >( m_ctx, m_channel );
		auto command = detail_::make_action(
				QueueCommands::Direction::Incoming
				, [len] (const std::shared_ptr< Receiver::RecvText > &p) { 
					Networking::messageData_t messageData = p ->receive_( len );
					return messageData;
				}
				, executor 
			);
		Deferred::QueueCommands::instance( ).push( command );
		return command;
	}
	auto vector(size_t len) {
		auto executor = std::make_unique< Receiver::RecvText >( m_ctx, m_channel );
		auto accumulator = std::make_unique< Networking::messageData_t >( );
		auto command = detail_::make_action(
				QueueCommands::Direction::Incoming
				, [len] (const std::unique_ptr< Receiver::RecvText > &executor, const std::unique_ptr< Networking::messageData_t > &buf) { 
					Networking::messageData_t messageData = executor ->receive_( len );
					if ( !messageData.empty( ) && messageData.size( ) != len ) {
						// accumulate
						std::copy( messageData.begin( ), messageData.end( ),  std::back_inserter( *buf ) );
						// until requirement size
						if ( buf ->size( ) == len )
							return *buf;
						return Networking::messageData_t{ };
					}
					return messageData;
				}
				, std::move( executor )
				, std::move( accumulator )
			);
		Deferred::QueueCommands::instance( ).push( command );
		return command;
	}
};
} // namespace syscross::HelloEOS::Deferred
