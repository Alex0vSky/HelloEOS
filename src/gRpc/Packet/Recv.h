// src\gRpc\Packet\Recv.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::gRpc::Packet {
struct Recv {
	using messageData_t = Networking::messageData_t;
	static void calling(
		const messageData_t &messageData
		, std::string *fullySpecifiedMethod
		, messageData_t *methodData
	) {
		detail_::Input( detail_::BaseHeader::Command::CallingSend, messageData )
				>> *fullySpecifiedMethod
				>> *methodData
			;
	}
	static void result(const messageData_t &messageData, messageData_t *methodData) {
		detail_::Input( detail_::BaseHeader::Command::CallingResult, messageData )
				>> *methodData
			;
	}
};
} // namespace syscross::HelloEOS::gRpc::Packet
