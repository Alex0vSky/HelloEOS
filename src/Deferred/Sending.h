// src\Deferred\Sending.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred {
class Sending {
	Ctx m_ctx;
	uint8_t m_channel;

public:
	Sending(Ctx ctx, uint8_t channel) : 
		m_ctx( ctx )
		, m_channel( channel )
	{}
	auto text(const std::string &text) {
		auto executor = std::make_shared< Sender::SendText >( m_ctx, m_channel );
		auto command = detail_::make_action(
				QueueCommands::Direction::Outgoing
				, [text] (const std::shared_ptr< Sender::SendText > &p) { 
					p ->sendTextPacket_( text );
					return Networking::messageData_t{ };
				}
				, executor 
			);
		Deferred::QueueCommands::instance( ).push( command );
		return command;
	}
	auto vector(const Networking::messageData_t &vector) {
		auto executor = std::make_shared< Sender::SendText >( m_ctx, m_channel );
		auto command = detail_::make_action(
				QueueCommands::Direction::Outgoing
				, [vector] (const std::shared_ptr< Sender::SendText > &p) { 
					p ->sendPacket_( vector );
					return Networking::messageData_t{ };
				}
				, executor 
			);
		Deferred::QueueCommands::instance( ).push( command );
		return command;
	}
};
} // namespace syscross::HelloEOS::Deferred
