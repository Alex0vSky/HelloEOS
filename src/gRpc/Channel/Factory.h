// src\gRpc\Channel\Factory.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::gRpc::Channel {
class Factory {
	class TypeIndepInterceptorFactory : public grpc::experimental::ClientInterceptorFactoryInterface {
		Deferred::Ctx m_ctx;

	 public:
		explicit TypeIndepInterceptorFactory(const Deferred::Ctx &ctx) : m_ctx( ctx )
		{}
		grpc::experimental::Interceptor* CreateClientInterceptor(
			grpc::experimental::ClientRpcInfo* info
		) override {
			return new detail_::TypeIndepInterceptor( info, m_ctx );
		}
	};

 public:
	// Channel isn't authenticated (use of InsecureChannelCredentials()).
	static
	std::shared_ptr<grpc::Channel> create(const Deferred::Ctx &ctx) {
		std::vector
			< std::unique_ptr< grpc::experimental::ClientInterceptorFactoryInterface > >
			interceptor_creators;
		interceptor_creators.push_back( 
				std::make_unique< TypeIndepInterceptorFactory >( ctx ) 
			);
		return grpc::experimental::CreateCustomChannelWithInterceptors(
				"localhost:50051"
				, grpc::InsecureChannelCredentials( )
				, grpc::ChannelArguments( )
				, std::move( interceptor_creators )
			);
	}
};
} // namespace syscross::HelloEOS::gRpc::Channel
