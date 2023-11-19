// src\gRpc\mix.h - poc gRPC over UDP, only for "Unary" calls
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::gRpc {
class OverUdp {
	using Ctx = Deferred::Ctx;
	using AcceptEveryone = Deferred::ConnectionRequestListener::AcceptEveryone;
	using messageData_t = Networking::messageData_t;
	using io_send = Deferred::Sending;
	using io_recv = Deferred::Receiving;

	// Magic, signature `gRpc over udp` protocol implementation
	static const unsigned int c_magic = 0x12345678;
	// Numerical version of current `gRpc over udp` protocol implementation
	static const unsigned int c_version = 1;
	// TODO(alex): commands //static const unsigned int c_commandRequest = 1; //static const unsigned int c_commandResponse = 2;
	enum class Command : uint32_t {
		CallingSend, CallingResult
	};

	// RFC 9110, section 4.1
	static const unsigned int c_maxStringLength = 8000;
	// 10Mb for example
	static const unsigned int c_maxDataLength = 10*1024*1024;
	const uint8_t m_channel = 0;

	const Ctx m_ctx;
	const AcceptEveryone m_acceptor;

public:
	// For recv need to have instance of Acceptor
	OverUdp(const Ctx &ctx) : 
		//m_ctx{ "gRpc", ctx.m_LocalUserId, ctx.m_PlatformHandle, ctx.m_FriendLocalUserId }
		m_ctx{ "CHAT", ctx.m_LocalUserId, ctx.m_PlatformHandle, ctx.m_FriendLocalUserId }
		, m_acceptor( m_ctx )
	{}

	messageData_t callUnary(const std::string &fullySpecifiedMethod, const messageData_t &methodData) {
		// Construct packet
		messageData_t toEos = Packet::send::calling( fullySpecifiedMethod, methodData );
		LOG( "[callUnary] Hexdump1" );
		std::cout << Hexdump( toEos.data( ), toEos.size( ) );
		// Send packet
		io_send sending( m_ctx, m_channel );
		sending.vector( toEos );
		LOG( "[callUnary] start ticks" );
		Deferred::QueueCommands::instance( ).ticksAll( );
		LOG( "[callUnary] sended" );
		// Recv packet from sendResponseUnary
		io_recv receiving( m_ctx, m_channel, m_acceptor );
		Command::CallingResult;
		receiving.vector( );
		auto incomingData = Deferred::QueueCommands::instance( ).ticksAll( );
		LOG( "[callUnary] incomingData size: %zd", incomingData.size( ) );
		LOG( "[callUnary] incomingData[0] size: %zd", incomingData[ 0 ].size( ) );

		messageData_t responseData = incomingData[ 0 ];
		LOG( "[callUnary] Hexdump2" );
		std::cout << Hexdump( responseData.data( ), responseData.size( ) );
		return responseData;
	}
	bool sendResponseUnary(messageData_t responseData) {
		// TODO(alex): signature and version, and command
		// Send packet
		io_send sending( m_ctx, m_channel );
		Command::CallingResult;
		sending.vector( responseData );
		LOG( "[sendResponseUnary] start ticks" );
		Deferred::QueueCommands::instance( ).ticksAll( );
		LOG( "[sendResponseUnary] sended" );
		return true;
	}

	bool recvUnary(std::string *fullySpecifiedMethod, messageData_t *methodData) {
		// Recv packet
		io_recv receiving( m_ctx, m_channel, m_acceptor );
		Command::CallingSend;
		receiving.vector( );
		auto incomingData = Deferred::QueueCommands::instance( ).ticksAll( );
		if ( incomingData.empty( ) ) 
			throw std::runtime_error( "recv error" );
		LOG( "[recvUnary] incomingData size: %zd", incomingData.size( ) );
		// TODO(alex): exceptions or no... decision...
		messageData_t fromEos = incomingData[ 0 ]; 
		if ( fromEos.empty( ) ) 
			//return false;
			throw std::runtime_error( "recv format error" );
		LOG( "[callUnary] Hexdump1" );
		std::cout << Hexdump( fromEos.data( ), fromEos.size( ) );

		//auto packet = 
		Packet::recv::calling( fromEos, fullySpecifiedMethod, methodData );
//		toEos = packet.parseFromArray( );
		unsigned int signature, version, lenMeth, lenData;
		Command command;
		google::protobuf::io::ArrayInputStream ais( fromEos.data( ), fromEos.size( ) );
		google::protobuf::io::CodedInputStream cis( &ais );

		bool b;
		b = cis.ReadLittleEndian32( &signature );
		if ( !b )
			throw std::runtime_error( "read error" );
		if ( c_magic != signature )
			throw std::runtime_error( "signature mismatch" );
		b = cis.ReadLittleEndian32( &version );
		if ( !b )
			throw std::runtime_error( "read error" );
		if ( c_version != version )
			throw std::runtime_error( "version mismatch" );
		b = cis.ReadLittleEndian32( (uint32_t*)&command );
		if ( !b )
			throw std::runtime_error( "read error" );
		if ( Command::CallingSend != command )
			throw std::runtime_error( "command mismatch" );
		b = cis.ReadLittleEndian32( &lenMeth );
		if ( !b )
			throw std::runtime_error( "read error" );
		if ( lenMeth > c_maxStringLength )
			throw std::runtime_error( "fully-specified method name URI exceeds maximum string length" );
		b = cis.ReadString( fullySpecifiedMethod, lenMeth );
		if ( !b )
			throw std::runtime_error( "read error" );
		b = cis.ReadLittleEndian32( &lenData );
		if ( !b )
			throw std::runtime_error( "read error" );
		if ( lenData > c_maxDataLength )
			throw std::runtime_error( "data length exceeds maximum length" );
		methodData ->resize( lenData );
		b = cis.ReadRaw( methodData ->data( ), lenData );
		if ( !b )
			throw std::runtime_error( "read error" );
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
namespace detail_ {
namespace grpce = grpc::experimental;
class TypeIndepInterceptor : public grpce::Interceptor {
	grpce::ClientRpcInfo* m_info;
	// TODO(alex): unique_ptr and std::move, instead ref
	gRpc::OverUdp m_grpcOverUdp;
	Networking::messageData_t m_fromUdp;
	void Intercept(grpce::InterceptorBatchMethods* methods) override {
		using ihp = grpc::experimental::InterceptionHookPoints;
		bool hijack = false;
		std::string DebugString; // tmp
		if (methods->QueryInterceptionHookPoint(ihp::PRE_SEND_INITIAL_METADATA)) {
			// Hijack all calls
			hijack = true;
		}
		if (methods->QueryInterceptionHookPoint(ihp::PRE_SEND_MESSAGE)) {
			LOG( "[Intercept] PRE_SEND_MESSAGE" );
			const google::protobuf::Message* req_msg =
				static_cast<const google::protobuf::Message*>(methods->GetSendMessage());
			if ( req_msg == nullptr ) {
				LOG( "[Intercept] Deserialize not implemented" );
				throw std::runtime_error( "Deserialize not implemented" );
			}
			Networking::messageData_t vecToUdp( req_msg ->ByteSizeLong( ) );
			bool bSerialize = req_msg ->SerializeToArray( vecToUdp.data( ), vecToUdp.size( ) );
			if ( !bSerialize ) {
				LOG( "[Intercept] Error in serialization" );
				throw std::runtime_error( "Error in serialization" );
			}
			// `m_info ->method( )` Its "/helloworld.Greeter/SayHello" Eq helloworld::HelloRequest::FullMessageName, from cpp_generator.cc
			m_fromUdp = m_grpcOverUdp.callUnary( m_info ->method( ), vecToUdp );
	    }
		if (methods->QueryInterceptionHookPoint(ihp::PRE_SEND_CLOSE)) {}
		if (methods->QueryInterceptionHookPoint(ihp::PRE_RECV_MESSAGE)) {
			LOG( "[Intercept] PRE_RECV_MESSAGE" );
			google::protobuf::Message* resp = 
				static_cast<google::protobuf::Message*>(methods->GetRecvMessage());
			bool bParse = resp ->ParseFromArray( m_fromUdp.data( ), m_fromUdp.size( ) );
			if ( !bParse ) {
				LOG( "[Intercept] Error in parse" );
				throw std::runtime_error( "Error in parse" );
			}
			DebugString = resp ->DebugString( );
			LOG( "[Intercept] DebugString: %s", DebugString.c_str( ) );
		}
		if (methods->QueryInterceptionHookPoint(ihp::PRE_RECV_STATUS)) {
			auto* status = methods ->GetRecvStatus( );
			*status = grpc::Status::OK;
		}
		// One of Hijack or Proceed always needs to be called to make progress.
		if ( hijack ) {
			// Hijack is called only once when PRE_SEND_INITIAL_METADATA is present in the hook points
			methods ->Hijack( );
		} else {
			// Proceed is an indicator that the interceptor is done intercepting the batch.
			methods ->Proceed( );
		}
	}
 public:
	TypeIndepInterceptor(grpce::ClientRpcInfo* info, const Deferred::Ctx &ctx) : 
		m_info( info ) 
		, m_grpcOverUdp( ctx )
	{}
};
struct TypeIndepInterceptorFactory : public grpce::ClientInterceptorFactoryInterface {
	Deferred::Ctx m_ctx;
	TypeIndepInterceptorFactory(const Deferred::Ctx &ctx) : m_ctx( ctx )
	{}
	grpce::Interceptor* CreateClientInterceptor(grpce::ClientRpcInfo* info) override {
		return new TypeIndepInterceptor( info, m_ctx );
	}
};
} // detail_
struct Factory {
	// Channel isn't authenticated (use of InsecureChannelCredentials()).
	static
	std::shared_ptr<grpc::Channel> channel(const Deferred::Ctx &ctx) {
		grpc::ChannelArguments args;
		std::vector
			< std::unique_ptr< grpc::experimental::ClientInterceptorFactoryInterface > >
			interceptor_creators;
		interceptor_creators.push_back( 
				std::make_unique<detail_::TypeIndepInterceptorFactory>( ctx ) 
			);
		return grpc::experimental::CreateCustomChannelWithInterceptors(
				"localhost:50051"
				, grpc::InsecureChannelCredentials( )
				, args
				, std::move( interceptor_creators )
			);
	}
};
} // namespace syscross::HelloEOS::gRpc
