// src\Deferred\Receiving.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred {
class Receiving {
	Ctx m_ctx;
public:
	// For recv need to have instance of Acceptor
	Receiving(Ctx ctx, const ConnectionRequestListener::BaseAcceptor &) : 
		m_ctx( ctx )
	{}
	auto text(size_t len) {
		if ( len > Networking::c_MaxDataSizeBytes )
			throw std::runtime_error( "exceed maximum packet lenght" );
		auto executor = std::make_shared< Receiver::Text >( m_ctx );
		auto command = detail_::make_action(
				QueueCommands::Direction::Incoming
				, [len] (const std::shared_ptr< Receiver::Text > &p) { 
					Networking::messageData_t messageData = p ->receive_( false, len );
					return messageData;
				}
				, executor 
			);
		command ->act( );
		return command;
	}
};
} // namespace syscross::HelloEOS::Deferred
