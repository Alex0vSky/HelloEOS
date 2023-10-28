// src\Anchronously\Send\Chat.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Anchronously::Send { 
static class Chat *g_chat; // tmp
class Chat : public Synchronously::Send::BaseSend {
	using BaseSend::BaseSend;
//	using BaseSend::sendTextPacket_;
//	//using Base<int>::print;

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
	class EngineSystem {
		std::function< R(Args... args) > m_function;
	public:
		EngineSystem(R (T::*function)(Args... args), T *that) : 
			m_function( std::bind( function, that, std::placeholders::_1 ) )
		{}
		R call(Args... args) {
			return m_function( args... );
		}
	};

	template <typename T, typename T2, typename R, typename... Args>
	auto makeEngine(R (T::*fun)(Args... args), T2 *that) {
		return EngineSystem< T, R, Args... >( fun, that );
	}

	bool foo(const std::string &text) {
		return true;
	}

//	bool sendTextPacket_(const std::string &value) = delete;
	auto message2(const std::string &text) {
		g_chat = this;
//		auto executableCommand = makeEngine( &Chat::foo, this );
		auto executableCommand = makeEngine( 
			&Chat::sendTextPacket_
			, this 
		);
		//executableCommand.function = std::bind( &Chat::sendTextPacket_, this, std::placeholders::_1 );
		//executableCommand.function( text );
		executableCommand.call( text );

		return executableCommand;
	}
};
} // namespace syscross::HelloEOS::Anchronously::Send
