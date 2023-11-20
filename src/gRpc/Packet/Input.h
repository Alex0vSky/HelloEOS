// src\gRpc\Packet\Input.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::gRpc::Packet {
namespace detail_ {
class Input : protected BaseHeader, protected google::protobuf::io::CodedInputStream {
public:
	Input(Command commandExpect, const Networking::messageData_t &messageData) :
		CodedInputStream( messageData.data( ), messageData.size( ) )
	{
		unsigned int signature, version;
		Command command;
		if ( !ReadLittleEndian32( &signature ) )
			throw std::runtime_error( "read error" );
		if ( c_magic != signature )
			throw std::runtime_error( "signature mismatch" );
		if ( !ReadLittleEndian32( &version ) )
			throw std::runtime_error( "read error" );
		if ( c_version != version )
			throw std::runtime_error( "version mismatch" );
		if ( !ReadLittleEndian32( reinterpret_cast<uint32_t*>( &command ) ) )
			throw std::runtime_error( "read error" );
		if ( commandExpect != command )
			throw std::runtime_error( "unexpected command" );
		if ( CodedInputStream::CurrentPosition( ) != sizeBaseHeader( ) )
			throw std::runtime_error( "read error" );
	}
	Input &operator >>(std::string &s) {
		unsigned int length;
		if ( !ReadLittleEndian32( &length ) )
			throw std::runtime_error( "read error" );
		if ( length > BaseHeader::c_maxStringLength )
			throw std::runtime_error( "string length exceeds maximum" );
		if ( !ReadString( &s, length ) )
			throw std::runtime_error( "read error" );
		return *this;
	}
	Input &operator >>(Networking::messageData_t &md) {
		unsigned int size;
		if ( !ReadLittleEndian32( &size ) )
			throw std::runtime_error( "read error" );
		if ( size > BaseHeader::c_maxDataLength )
			throw std::runtime_error( "size exceeds maximum" );
		md.resize( size );
		if ( !ReadRaw( md.data( ), size ) )
			throw std::runtime_error( "read error" );
		return *this;
	}
};
} // namespace detail_
} // namespace syscross::HelloEOS::gRpc::Packet
