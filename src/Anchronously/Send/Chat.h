// src\Anchronously\Send\Chat.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Anchronously::Send {
class Chat : public Synchronously::Send::BaseSend {
	using BaseSend::BaseSend;

public:
	static auto makeCommand(EOS_HPlatform platformHandle, EOS_ProductUserId localUserId, EOS_ProductUserId friendLocalUserId, const std::string &SocketName = "CHAT") {
		return std::make_shared<Chat>( 
				platformHandle
				, localUserId
				, friendLocalUserId
				, SocketName
			);
	}

	// @insp https://stackoverflow.com/questions/69692722/how-can-i-have-a-function-pointer-template-as-a-template-parameter
	template <typename T, typename R, typename... Args>
	struct Deduced {
		std::function< R(Args... args) > m_function;
	};
	template <typename T, typename R, typename... Args>
	auto makeDeduce(R (T::*fun)(Args... args)) {
		return Deduced< T, R, Args... >{ };
	}
	auto message2(const std::string &text) {

		EOS_P2P_GetPacketQueueInfoOptions Options = { EOS_P2P_GETPACKETQUEUEINFO_API_LATEST };
		EOS_P2P_PacketQueueInfo OutPacketQueueInfo = { };

		auto executableCommand = makeDeduce( &Chat::sendTextPacket_ );
		executableCommand.m_function = fx::easy_bind( &Chat::sendTextPacket_, this );
		executableCommand.m_function( text );

		::EOS_P2P_GetPacketQueueInfo( m_P2PHandle, &Options, &OutPacketQueueInfo );
		LOG( "[Anchronously::Send::Chat] Outgoing: %I64d", OutPacketQueueInfo.OutgoingPacketQueueCurrentSizeBytes );
		while( OutPacketQueueInfo.OutgoingPacketQueueCurrentSizeBytes ) {
			::EOS_Platform_Tick( m_PlatformHandle );
			::EOS_P2P_GetPacketQueueInfo( m_P2PHandle, &Options, &OutPacketQueueInfo );
			LOG( "[Anchronously::Send::Chat] Outgoing: %I64d", OutPacketQueueInfo.OutgoingPacketQueueCurrentSizeBytes );
		}
		// all data in wires

//		auto greet = std::mem_fn(&Chat::sendTextPacket_);
//		greet( this, text );
		return executableCommand;
	}
};
} // namespace syscross::HelloEOS::Anchronously::Send
