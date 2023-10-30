// src\Deferred\PingPonger.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred {
class PingPonger {
	const uint8_t m_channel = 0;
	const Ctx m_ctx;
	const ConnectionRequestListener::AcceptEveryone m_acceptor;

public:
	// For recv need to have instance of Acceptor
	PingPonger(const Ctx &ctx) : 
		m_ctx{ "PINGPONG", ctx.m_LocalUserId, ctx.m_PlatformHandle, ctx.m_FriendLocalUserId }
		, m_acceptor( m_ctx )
	{}

	std::chrono::milliseconds pingerMeasure() {
		Deferred::Receiving receiving( m_ctx, m_channel, m_acceptor );
		Deferred::Sending sending( m_ctx, m_channel );
		//LOG( "[<<] 'PING'" );
		sending.text( "PING" );
		receiving.text( strlen( "PONG" ) );
		auto start = std::chrono::system_clock::now( );
		auto incomingData = Deferred::QueueCommands::instance( ).ticksAll( );
		auto end = std::chrono::system_clock::now( ) - start;
		const auto &packet = incomingData[ 0 ];
		std::string string( packet.begin( ), packet.end( ) );
		//LOG( "[>>] '%s'", string.c_str( ) );
		if ( string.compare( "PONG" ) ) 
			throw std::runtime_error( "ping-pong mismatch" );
		return std::chrono::duration_cast< std::chrono::milliseconds >( end );
	}
	void infinitePonger() {
		Deferred::Receiving receiving( m_ctx, m_channel, m_acceptor );
		Deferred::Sending sending( m_ctx, m_channel );
		LOG( "[~] press [Ctrl+C] to exit" );
		while ( true ) {
			receiving.text( strlen( "PING" ) );
			auto incomingData = Deferred::QueueCommands::instance( ).ticksAll( );
			const auto &packet = incomingData[ 0 ];
			std::string string( packet.begin( ), packet.end( ) );
			//LOG( "[>>] '%s'", string.c_str( ) );
			if ( string.compare( "PING" ) ) 
				throw std::runtime_error( "ping-pong mismatch" );
			//LOG( "[<<] 'PONG'" );
			sending.text( "PONG" );
			Deferred::QueueCommands::instance( ).ticksAll( );
		}
	}
};
} // namespace syscross::HelloEOS::Deferred
