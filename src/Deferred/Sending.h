// src\Deferred\Sending.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred {
class Sending {
	Ctx m_ctx;
public:
	Sending(Ctx ctx) : 
		m_ctx( ctx )
	{}
	// discardable
	auto text(const std::string &text) {
		auto executor = std::make_shared< Sender::Text >( 
			m_ctx.m_PlatformHandle
			, m_ctx.m_LocalUserId
			, m_ctx.m_FriendLocalUserId
			, m_ctx.m_SocketName
		);
		auto command = detail_::make_action(
				QueueCommands::Direction::Outgoing
				, [text] (const std::shared_ptr< Sender::Text > &p) { 
					p ->sendTextPacket_( text );
					return Networking::messageData_t{ };
				}
				, executor 
			);
		command ->act( );
		return command;
	}
};
} // namespace syscross::HelloEOS::Deferred
