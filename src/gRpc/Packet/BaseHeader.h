// src\gRpc\Packet\BaseHeader.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::gRpc::Packet {
namespace detail_ {
struct BaseHeader {
	// Magic, signature `gRpc over udp` protocol implementation
	static const unsigned int c_magic = 0x12345678;
	// Numerical version of current `gRpc over udp` protocol implementation
	static const unsigned int c_version = 1;
	// RFC 9110, section 4.1
	static const unsigned int c_maxStringLength = 8000;
	// 10Mb for example
	static const unsigned int c_maxDataLength = 10*1024*1024;
	enum class Command : uint32_t { start_, 
		CallingSend, CallingResult
	};
#pragma pack( push, 1 )
	// One place, for remember
	struct {
		std::decay_t< decltype( c_magic ) > signature;
		std::decay_t< decltype( c_version ) > version;
		Command command;
	} header_;
#pragma pack( pop )
	static constexpr auto sizeBaseHeader() {
		return sizeof( header_ );
	}
};
} // namespace detail_
} // namespace syscross::HelloEOS::gRpc::Packet
