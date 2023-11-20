// src\gRpc\OverUdp.h - poc gRPC over UDP, only for "Unary" calls
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::gRpc {
class OverUdp {
	using Ctx = Deferred::Ctx;
	using AcceptEveryone = Deferred::ConnectionRequestListener::AcceptEveryone;
	using messageData_t = Networking::messageData_t;
	using io_send = Deferred::Sending;
	using io_recv = Deferred::Receiving;

	const uint8_t m_channel = 0;
	const Ctx m_ctx;
	const AcceptEveryone m_acceptor;

public:
	// For recv need to have instance of Acceptor
	OverUdp(const Ctx &ctx) : 
		m_ctx{ "gRpc", ctx.m_LocalUserId, ctx.m_PlatformHandle, ctx.m_FriendLocalUserId }
		, m_acceptor( m_ctx )
	{}

	messageData_t callUnary(const std::string &fullySpecifiedMethod, const messageData_t &methodData) {
		// Construct packet
		messageData_t toEos = Packet::Send::calling( fullySpecifiedMethod, methodData );
		//std::cout << Hexdump( toEos.data( ), toEos.size( ) );
		// Send packet
		io_send sending( m_ctx, m_channel );
		sending.vector( toEos );
		LOG( "[callUnary] start ticks" );
		Deferred::QueueCommands::instance( ).ticksAll( );
		LOG( "[callUnary] sended" );
		// Recv packet from sendResponseUnary
		io_recv receiving( m_ctx, m_channel, m_acceptor );
		receiving.vector( );
		auto incomingData = Deferred::QueueCommands::instance( ).ticksAll( );
		if ( incomingData.empty( ) ) 
			return { };

		messageData_t responseData = incomingData[ 0 ];
		messageData_t methodResult;
		Packet::Recv::result( responseData, &methodResult );
		return methodResult;
	}
	bool sendResponseUnary(messageData_t responseData) {
		auto methodResult = Packet::Send::result( responseData );
		// Send packet
		io_send sending( m_ctx, m_channel );
		sending.vector( methodResult );
		LOG( "[sendResponseUnary] start ticks" );
		Deferred::QueueCommands::instance( ).ticksAll( );
		LOG( "[sendResponseUnary] sended" );
		return true;
	}
	bool recvUnary(std::string *fullySpecifiedMethod, messageData_t *methodData) {
		// Recv packet
		io_recv receiving( m_ctx, m_channel, m_acceptor );
		receiving.vector( );
		auto incomingData = Deferred::QueueCommands::instance( ).ticksAll( );
		if ( incomingData.empty( ) ) 
			throw std::runtime_error( "recv error" );
		messageData_t fromEos = incomingData[ 0 ]; 
		if ( fromEos.empty( ) ) 
			return false;
		Packet::Recv::calling( fromEos, fullySpecifiedMethod, methodData );

		return true;
	}
	bool parseFullySpecifiedMethod(
		const std::string &fullySpecifiedMethod
		, std::string *package
		, std::string *service
		, std::string *method
	) {
		// Service name cannot have a dot
		size_t posRDot = fullySpecifiedMethod.rfind( '.' );
		if ( std::string::npos == posRDot || posRDot <= 1 )
			return false;
		*package = fullySpecifiedMethod.substr( 1, posRDot - 1 );
		std::string rightPart = fullySpecifiedMethod.substr( posRDot + 1 );
		size_t posDelim = rightPart.find( '/' );
		if ( std::string::npos == posDelim )
			return false;
		*service = rightPart.substr( 0, posDelim );
		*method = rightPart.substr( posDelim + 1 );
		return true;
	}
};
} // namespace syscross::HelloEOS::gRpc
