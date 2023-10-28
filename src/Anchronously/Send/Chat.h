// src\Anchronously\Send\Chat.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Anchronously::Send { 
class Chat : public Synchronously::Send::BaseSend {
	using BaseSend::BaseSend;

public:
	struct ExecutableCommand {
		std::function<bool(const std::string &text)> function;
		bool bRuned;
		bool operator()(const std::string &text) {
			bRuned = true;
			function( text );
		}
	};
	Networking::send_t message(const std::string &text) {
		auto future = std::async( &Chat::sendPacket_< std::decay_t< decltype( text ) > >, this, text );
		return future;
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
		auto executableCommand = makeDeduce( &Chat::sendTextPacket_ );
		executableCommand.m_function = std::bind( &Chat::sendTextPacket_, this, std::placeholders::_1 );
		executableCommand.m_function( text );

		return executableCommand;
	}
};
} // namespace syscross::HelloEOS::Anchronously::Send
