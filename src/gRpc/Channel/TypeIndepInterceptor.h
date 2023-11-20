// src\gRpc\Channel\TypeIndepInterceptor.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::gRpc::Channel::detail_ {
namespace grpce = grpc::experimental;
class TypeIndepInterceptor : public grpce::Interceptor {
	grpce::ClientRpcInfo* m_info;
	gRpc::OverUdp m_grpcOverUdp;
	Networking::messageData_t m_fromUdp;
	void Intercept(grpce::InterceptorBatchMethods* methods) override {
		using ihp = grpce::InterceptionHookPoints;
		bool hijack = false;
		if (methods->QueryInterceptionHookPoint(ihp::PRE_SEND_INITIAL_METADATA)) {
			hijack = true;
		}
		if (methods->QueryInterceptionHookPoint(ihp::PRE_SEND_MESSAGE)) {
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
			// `m_info ->method( )` Its "/helloworld.Greeter/SayHello" 
			// Eq helloworld::HelloRequest::FullMessageName, from cpp_generator.cc
			m_fromUdp = m_grpcOverUdp.callUnary( m_info ->method( ), vecToUdp );
	    }
		if (methods->QueryInterceptionHookPoint(ihp::PRE_SEND_CLOSE)) {}
		if (methods->QueryInterceptionHookPoint(ihp::PRE_RECV_MESSAGE)) {
			google::protobuf::Message* resp = 
				static_cast<google::protobuf::Message*>(methods->GetRecvMessage());
			bool bParse = resp ->ParseFromArray( m_fromUdp.data( ), m_fromUdp.size( ) );
			if ( !bParse ) {
				LOG( "[Intercept] Error in parse" );
				throw std::runtime_error( "Error in parse" );
			}
		}
		if (methods->QueryInterceptionHookPoint(ihp::PRE_RECV_STATUS)) {
			auto* status = methods ->GetRecvStatus( );
			*status = grpc::Status::OK;
		}
		if ( hijack ) {
			methods ->Hijack( );
		} else {
			methods ->Proceed( );
		}
	}

 public:
	TypeIndepInterceptor(grpce::ClientRpcInfo* info, const Deferred::Ctx &ctx) : 
		m_info( info ) 
		, m_grpcOverUdp( ctx )
	{}
};
} // namespace syscross::HelloEOS::gRpc::Channel::detail_
