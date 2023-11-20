// src\Main_gRpc.h - code main class, for unittests code coverage
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
//#include "eos_auth.h"
//#include "eos_sdk.h"
//#include "eos_types.h"
//#include "Windows/eos_Windows.h"
//#include "eos_logging.h"
//#include "eos_p2p.h"
//#include "eos_friends.h"
//#include "eos_presence.h"

//#include <iostream>
//#include <atomic>
//#include <thread>
//#include <vector>
//#include <future>
//#include <coroutine>
//#include <chrono>

#include "Version.h"
#include "Log.h"
#include "HumanReadable.h"
#include "Credentials/Hardcode.h"
#include "InitializeEOS.h"
#include "Networking.h"
#include "Synchronously/Auth.h"
#include "Synchronously/Friend.h"
#include "Synchronously/AccountMapping.h"
#include "Synchronously/Presence.h"
#include "Deferred/Ctx.h"
#include "Deferred/ConnectionRequestListener/AcceptEveryone.h"
#include "Deferred/QueueCommands.h"
#include "Deferred/Action.h"
#include "Deferred/Sender/SendText.h"
#include "Deferred/Receiver/RecvText.h"
#include "Deferred/Sending.h"
#include "Deferred/Receiving.h"
#include "Deferred/PingPonger.h"
#include "gRpc/Packet/BaseHeader.h"
#include "gRpc/Packet/Input.h"
#include "gRpc/Packet/Output.h"
#include "gRpc/Packet/Recv.h"
#include "gRpc/Packet/Send.h"
#include "gRpc/OverUdp.h"
#include "gRpc/Channel/TypeIndepInterceptor.h"
#include "gRpc/Channel/Factory.h"

#pragma region gRPC
#ifdef A0S_GRPC
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;
using grpc::Channel;
using grpc::ClientContext;
class GreeterClient {
	std::unique_ptr<Greeter::Stub> m_stub;

 public:
	GreeterClient(std::shared_ptr<Channel> channel)
		: m_stub( Greeter::NewStub( channel ) ) 
	{}
	// Assembles the client's payload, sends it and presents the response back from the server.
	std::string SayHello(const std::string& user) {
		// Data we are sending to the server.
		HelloRequest request;
		request.set_name(user);
		// Container for the data we expect from the server.
		HelloReply reply;
		// Context for the client. It could be used to convey extra information to
		// the server and/or tweak certain RPC behaviors.
		ClientContext context;
		// The actual RPC.
		Status status = m_stub->SayHello(&context, request, &reply);
		// Act upon its status.
		if (status.ok()) {
			return reply.message();
		} else {
			std::cout << status.error_code() << ": " << status.error_message()
					<< std::endl;
			return "RPC failed";
		}
	}
};

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
class GreeterServiceImpl final : public Greeter::Service {
	Status SayHello(ServerContext* context, const HelloRequest* request, HelloReply* reply) override {
		std::string prefix( "Hello " );
		reply ->set_message( prefix + request ->name( ) );
		std::cout << "answer (" << request ->name( ) << ")" << std::endl; // logging mark
		return Status::OK;
	}
};
#endif // A0S_GRPC
#pragma endregion // endregion gRPC

extern "C" int _getch( void );

namespace syscross::HelloEOS { struct Main_gRpc {
	void run(int argc) {
		bool isServer = ( argc > 1 );
#ifndef A0S_GRPC
		LOG( "[~] must be defined A0S_GRPC" );
		return;
#endif // A0S_GRPC
#ifdef NDEBUG
		LOG( "[~] using gRpc release version *.lib's without trace headers and /MD compiling" );
#endif // NDEBUG

#pragma region prepare
		InitializeEOS init;
		EOS_HPlatform platformHandle = init.initialize( );
		if ( !platformHandle )
			return;

		Synchronously::Auth auth( platformHandle );
		std::string tokenDevAuthToolAuth;
		if ( isServer )
			tokenDevAuthToolAuth = "cred2";
		else
			tokenDevAuthToolAuth = "cred1";
		if ( !auth.connectAndLogin( tokenDevAuthToolAuth ) )
			return;
		LOG( "[~] auth.getLocalUserId( ) valid: %s", ( ::EOS_ProductUserId_IsValid( auth.getLocalUserId( ) ) ?"TRUE" :"FALSE" ) );

		Synchronously::Friend friends( platformHandle, auth.getAccount( ) );
		auto allFriends = friends.getAll( );
		if ( allFriends.empty( ) )
			return;

		Synchronously::AccountMapping mapping( platformHandle, auth.getLocalUserId( ), allFriends );
		if ( !mapping.getFirstFriendId( ) )
			return;
		LOG( "[~] mapping.getFriendLocalUserId( ) valid: %s", ( ::EOS_ProductUserId_IsValid( mapping.getFriendLocalUserId( ) ) ?"TRUE" :"FALSE" ) );

		Synchronously::Presence presence( platformHandle, auth.getAccount( ) );
		if ( !presence.setOnlineAndTitle( ) )
			return;

		char timeString[ 64 ];
		std::time_t t = std::time( nullptr );
		std::tm bt; localtime_s( &bt, &t );
		std::strftime( timeString, sizeof( timeString ), "%A %c", &bt );
#pragma endregion // prepare

		Deferred::QueueCommands::init( platformHandle );
		Deferred::Ctx ctx{ "CHAT", auth.getLocalUserId( ), platformHandle, mapping.getFriendLocalUserId( ) };
		Deferred::ConnectionRequestListener::AcceptEveryone acceptEveryone( ctx );
		uint8_t channelReceiving = 1;
		uint8_t channelSending = 2;
		std::chrono::milliseconds sleep{ 100 };
		if ( isServer ) {
			LOG( "[~] server" );
////			Deferred::Receiving receiving( ctx, acceptEveryone );
////			auto len = strlen( "PING" );
////			auto command = receiving.text( len );
////			////command ->act( ); // OR
////			//receiving.text( len ); // OR
//
//			Deferred::Receiving receiving( ctx, channelReceiving, acceptEveryone );
//			Deferred::Sending sending( ctx, channelSending );
//			while ( true ) {
//				receiving.text( strlen( "PING" ) );
//				auto incomingData = Deferred::QueueCommands::instance( ).ticksAll( );
//				const auto &packet = incomingData[ 0 ];
//				std::string string( packet.begin( ), packet.end( ) );
//				LOG( "[>>] '%s'", string.c_str( ) );
//				if ( string.compare( "PING" ) ) 
//					throw std::runtime_error( "ping-pong mismatch" );
//				LOG( "[<<] 'PONG'" );
//				sending.text( "PONG" );
//				Deferred::QueueCommands::instance( ).ticksAll( );
//				LOG( "[~] sleep" );
//				std::this_thread::sleep_for( sleep );
//			}
			//Deferred::PingPonger pingPonger( ctx );
			//pingPonger.infinitePonger( );

#ifdef A0S_GRPC
			// Standart helloworld
			std::string server_address = "localhost:50051";
			GreeterServiceImpl service;
			grpc::EnableDefaultHealthCheckService( true );
			grpc::reflection::InitProtoReflectionServerBuilderPlugin( );
			ServerBuilder serverBuilder;
			serverBuilder.AddListeningPort( server_address, grpc::InsecureServerCredentials( ) );
			serverBuilder.RegisterService( &service );
			std::unique_ptr<Server> server( serverBuilder.BuildAndStart( ) );
			std::cout << "Server listening on " << server_address << std::endl;
			std::thread serverWorker([&server](){
				server ->Wait( );
			});

			gRpc::OverUdp grpcOverUdp( ctx );
			// For client calls and reflection
			std::shared_ptr<grpc::Channel> channelInProc = server ->InProcessChannel( grpc::ChannelArguments{ } );
			::grpc::ProtoReflectionDescriptorDatabase reflection_db( channelInProc );
			google::protobuf::DescriptorPool desc_pool( &reflection_db );
			google::protobuf::DynamicMessageFactory dmf;

			LOG( "[~] press [ESC] to planning stop server loop end exit" );
			// @insp SO/6171132/non-blocking-console-input-c
			std::atomic<int> key = { };
			const int ESC = 27;
			std::thread modernCppSupercheckerCenturyXXI_peekch([&key](){
					while ( true ) { key = _getch( ); }
				});
			modernCppSupercheckerCenturyXXI_peekch.detach( ); // leak

			while ( true ) {
				if ( ESC == key )
					break;
				std::string fullySpecifiedMethod;
				Networking::messageData_t fromUdp;
				if ( !grpcOverUdp.recvUnary( &fullySpecifiedMethod, &fromUdp ) )
					continue;

				// Wrong method will cause grpc::StatusCode::UNIMPLEMENTED
				std::string package, serviceName, method;
				if ( !grpcOverUdp.parseFullySpecifiedMethod( fullySpecifiedMethod, &package, &serviceName, &method ) )
					throw std::runtime_error( "Cant parse fully-specified method name" );
				// Create input_type and output_type via DynamicMessageFactory
				const google::protobuf::MethodDescriptor* method_desc = desc_pool.FindMethodByName( 
						package + "." + serviceName + "." + method
					);
				if ( !method_desc )
					throw std::runtime_error( "Unknown method in reflection" );
				auto fullNameInputType = method_desc ->input_type( ) ->full_name( );
				const google::protobuf::Descriptor* request_desc = 
					desc_pool.FindMessageTypeByName( fullNameInputType );
				auto fullNameOutputType = method_desc ->output_type( ) ->full_name( );
				const google::protobuf::Descriptor* response_desc = 
					desc_pool.FindMessageTypeByName( fullNameOutputType );
				if ( !request_desc || !response_desc )
					throw std::runtime_error( "Reflection error" );
				google::protobuf::Message* requestDyn = dmf.GetPrototype( request_desc ) ->New( );
				google::protobuf::Message* responseDyn = dmf.GetPrototype( response_desc ) ->New( );
				if ( !requestDyn || !responseDyn )
					throw std::runtime_error( "DynamicMessageFactory" );

				const char *suffix_for_stats = nullptr;
				grpc::internal::RpcMethod rmeth( 
						fullySpecifiedMethod.c_str( )
						, suffix_for_stats
						, ::grpc::internal::RpcMethod::NORMAL_RPC 
						, channelInProc
					);
				bool bParse = requestDyn ->ParseFromArray( fromUdp.data( ), fromUdp.size( ) );
				if ( !bParse )
					throw std::runtime_error( "Error ParseFromArray" );

				grpc::ClientContext clientContextSingleCall;
				grpc::Status stat = grpc::internal::BlockingUnaryCall
					<google::protobuf::Message,google::protobuf::Message>
					( channelInProc.get( ), rmeth, &clientContextSingleCall, *requestDyn, responseDyn );		
				if ( stat.error_code() == grpc::StatusCode::UNIMPLEMENTED )
					throw std::runtime_error( "Wrong fully-specified method name" );
				if ( stat.error_code() == grpc::StatusCode::RESOURCE_EXHAUSTED )
					throw std::runtime_error( "Too many queries" );
				if ( !stat.ok( ) )
					throw std::runtime_error( "Error BlockingUnaryCall" );

				Networking::messageData_t toUdp( responseDyn ->ByteSizeLong( ) );
				bool bSerialize = responseDyn ->SerializeToArray( toUdp.data( ), toUdp.size( ) );
				if ( !bSerialize )
					throw std::runtime_error( "Error in serialization" );
				grpcOverUdp.sendResponseUnary( toUdp );
			}

			server ->Shutdown( );
			serverWorker.join( );

#endif // A0S_GRPC

		} else {
			LOG( "[~] client" );
////			Deferred::Sending sending( ctx );
////			auto command = sending.text( "PING" );
////			//// Second
////			//command ->act( );
//
//			std::swap( channelSending, channelReceiving );
//			Deferred::Sending sending( ctx, channelSending );
//			Deferred::Receiving receiving( ctx, channelReceiving, acceptEveryone );
//			while ( true ) {
//				LOG( "[<<] 'PING'" );
//				sending.text( "PING" );
//				receiving.text( strlen( "PONG" ) );
//				auto incomingData = Deferred::QueueCommands::instance( ).ticksAll( );
//				const auto &packet = incomingData[ 0 ];
//				std::string string( packet.begin( ), packet.end( ) );
//				LOG( "[>>] '%s'", string.c_str( ) );
//				if ( string.compare( "PONG" ) ) 
//					throw std::runtime_error( "ping-pong mismatch" );
//				LOG( "[~] sleep" );
//				std::this_thread::sleep_for( sleep );
//			}
			//std::chrono::seconds sleep{ 1 };
			//Deferred::PingPonger pingPonger( ctx );
			//LOG( "[~] press [Ctrl+C] to exit" );
			//while( true ) {
			//	auto milli = pingPonger.pingerMeasure( );
			//	LOG( "Reply from 'Epic-provided relay': bytes=4, time=%lldms", milli.count( ) );
			//	std::this_thread::sleep_for( sleep );
			//}

#ifdef A0S_GRPC
			// naming variants
			//std::shared_ptr<Channel> grpcOverUdpChannel = gRpc::xxx::createChannel( grpcOverUdp );
			//std::shared_ptr<Channel> grpcOverUdpChannel = gRpc::TypeIndepInterceptor::createChannel( grpcOverUdp );
			//std::shared_ptr<Channel> grpcOverUdpChannel = gRpc::Channel::typeIndepInterceptor( grpcOverUdp );
			std::shared_ptr<Channel> grpcOverUdpChannel = gRpc::Channel::Factory::create( ctx );
			// Standart helloworld
			GreeterClient greeter( grpcOverUdpChannel );
			std::string user( "world" );
			std::string reply = greeter.SayHello( user );
			std::cout << "Greeter received: " << reply << std::endl;
#endif // A0S_GRPC

		}
		LOG( "[~] press [Enter] to exit" );
		getchar( );
		return;
	}
};
} // namespace syscross::HelloEOS
