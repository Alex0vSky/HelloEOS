// src\gRpc\Packet.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::gRpc {
class Packet {
	using messageData_t = Networking::messageData_t;
	// Magic, signature `gRpc over udp` protocol implementation
	static const unsigned int c_magic = 0x12345678;
	// Numerical version of current `gRpc over udp` protocol implementation
	static const unsigned int c_version = 1;
	// RFC 9110, section 4.1
	static const unsigned int c_maxStringLength = 8000;
	// 10Mb for example
	static const unsigned int c_maxDataLength = 10*1024*1024;

	enum class Command : uint32_t {
		CallingSend, CallingResult
	};
	/*
	class Header {
		google::protobuf::io::ArrayOutputStream m_aos;
		google::protobuf::io::ArrayInputStream m_ais;
		std::unique_ptr< google::protobuf::io::CodedOutputStream > m_ostream;
		std::unique_ptr< google::protobuf::io::CodedInputStream > m_istream;
	public:
		Header(messageData_t &messageData) :
			m_aos( messageData.data( ), messageData.size( ) )
			, m_ais( messageData.data( ), messageData.size( ) )
		{
			m_ostream = std::make_unique< google::protobuf::io::CodedOutputStream >
				( &m_aos );
			m_istream = std::make_unique< google::protobuf::io::CodedInputStream >
				( &m_ais );
		}
		bool writeHeader(Command command) {
			m_ostream ->WriteLittleEndian32( c_magic );
			m_ostream ->WriteLittleEndian32( c_version );
			m_ostream ->WriteLittleEndian32( (uint32_t)command );
			return !m_ostream ->HadError( );
		}
		bool readHeader(const Command commandExpect) {
			unsigned int signature, version;
			Command command;
			if ( !m_istream ->ReadLittleEndian32( &signature ) )
				return false;
			if ( c_magic != signature )
				throw std::runtime_error( "signature mismatch" );
			if ( !m_istream ->ReadLittleEndian32( &version ) )
				return false;
			if ( c_version != version )
				throw std::runtime_error( "version mismatch" );
			if ( !m_istream ->ReadLittleEndian32( (uint32_t*)&command ) )
				return false;
			if ( commandExpect != command )
				return false;
			return true;
		}
	};//*/
	class InputHeader : 
		public google::protobuf::io::CodedInputStream 
	{
		google::protobuf::io::ArrayInputStream m_ais;
	public:
		InputHeader(const messageData_t &messageData) :
			CodedInputStream( &m_ais )
			, m_ais( messageData.data( ), messageData.size( ) )
		{}
		bool readHeader(const Command commandExpect) {
			unsigned int signature, version;
			Command command;
			if ( !ReadLittleEndian32( &signature ) )
				return false;
			if ( c_magic != signature )
				throw std::runtime_error( "signature mismatch" );
			if ( !ReadLittleEndian32( &version ) )
				return false;
			if ( c_version != version )
				throw std::runtime_error( "version mismatch" );
			if ( !ReadLittleEndian32( (uint32_t*)&command ) )
				return false;
			if ( commandExpect != command )
				return false;
			return true;
		}
	};
	class OutputHeader : 
		public google::protobuf::io::CodedOutputStream 
	{
		google::protobuf::io::ArrayOutputStream m_aos;
	public:
		// TODO(alex): accept `Command commandExpect` and sizebuf, 
		// and return buf on new method `finalize` to chech hadError and throw on error
		OutputHeader(messageData_t &messageData) :
			CodedOutputStream( &m_aos )
			, m_aos( messageData.data( ), messageData.size( ) )
		{}
		void writeHeader(Command command) {
			WriteLittleEndian32( c_magic );
			WriteLittleEndian32( c_version );
			WriteLittleEndian32( (uint32_t)command );
		}
	};

public:
	// Construct packet CallingSend and serializeToArray
	static
	messageData_t callingSend(const std::string &fullySpecifiedMethod, const messageData_t &methodData) {
		auto lenMeth = static_cast<unsigned int>( fullySpecifiedMethod.length( ) );
		auto lenData = static_cast<unsigned int>( methodData.size( ) );
		messageData_t messageData( 0
				+ sizeof( c_magic ) + sizeof( c_version ) + sizeof( Command ) 
				+ sizeof( lenMeth ) 
				+ fullySpecifiedMethod.length( ) 
				+ sizeof( lenData ) 
				+ methodData.size( ) 
			);
		OutputHeader header( messageData ); // join bellow
		header.writeHeader( Command::CallingSend ); // join above
		header.WriteLittleEndian32( lenMeth );
		header.WriteString( fullySpecifiedMethod );
		header.WriteLittleEndian32( lenData );
		header.WriteRaw( methodData.data( ), methodData.size( ) );
		if ( header.HadError( ) )
			return { };
		return messageData;
	}
	static
	messageData_t callingResult(const messageData_t &methodData) {
		auto lenData = static_cast<unsigned int>( methodData.size( ) );
		messageData_t messageData( 0
				+ sizeof( c_magic ) + sizeof( c_version ) + sizeof( Command ) 
				+ sizeof( lenData ) 
				+ methodData.size( ) 
			);
		OutputHeader header( messageData );
		header.writeHeader( Command::CallingResult );
		header.WriteLittleEndian32( lenData );
		header.WriteRaw( methodData.data( ), methodData.size( ) );
		if ( header.HadError( ) )
			return { };
		return messageData;
	}
	// Construct packet CallingSend from parseFromArray
	static
	bool callingSend(const messageData_t &messageData, std::string *fullySpecifiedMethod, messageData_t *methodData) {
		InputHeader header( messageData );
		header.readHeader( Command::CallingSend );
		unsigned int lenMeth, lenData;
		if ( !header.ReadLittleEndian32( &lenMeth ) )
			return false;
		if ( lenMeth > c_maxStringLength )
			throw std::runtime_error( "fully-specified method name URI exceeds maximum string length" );
		if ( !header.ReadString( fullySpecifiedMethod, lenMeth ) )
			return false;
		if ( !header.ReadLittleEndian32( &lenData ) )
			return false;
		if ( lenData > c_maxDataLength )
			throw std::runtime_error( "data length exceeds maximum length" );
		methodData ->resize( lenData );
		if ( !header.ReadRaw( methodData ->data( ), lenData ) )
			return false;
		return true;
	}
};
} // namespace syscross::HelloEOS::gRpc
