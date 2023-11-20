// src\gRpc\Packet\Send.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::gRpc::Packet {
struct Send {
	using messageData_t = Networking::messageData_t;
	static messageData_t calling(
		const std::string &fullySpecifiedMethod
		, const messageData_t &methodData
	) {
		return detail_::Output( detail_::BaseHeader::Command::CallingSend )
				<< fullySpecifiedMethod
				<< methodData
			;
	}
	static messageData_t result(const messageData_t &methodData) {
		return detail_::Output( detail_::BaseHeader::Command::CallingResult )
				<< methodData
			;
	}
};
} // namespace syscross::HelloEOS::gRpc::Packet
