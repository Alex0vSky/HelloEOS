// src\gRpc\Packet.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::gRpc {
class Packet {
	using ArrayOutputStream = google::protobuf::io::ArrayOutputStream;
	using CodedOutputStream = google::protobuf::io::CodedOutputStream;
	using ArrayInputStream = google::protobuf::io::ArrayInputStream;
	using messageData_t = Networking::messageData_t;
	enum class Command : uint32_t { start_, 
		CallingSend, CallingResult
	};
	struct BaseHeader {
		// Magic, signature `gRpc over udp` protocol implementation
		static const unsigned int c_magic = 0x12345678;
		// Numerical version of current `gRpc over udp` protocol implementation
		static const unsigned int c_version = 1;
		// RFC 9110, section 4.1
		static const unsigned int c_maxStringLength = 8000;
		// 10Mb for example
		static const unsigned int c_maxDataLength = 10*1024*1024;
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
	class InputHeader : public BaseHeader, public google::protobuf::io::CodedInputStream {
		ArrayInputStream m_ais;
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
			if ( CodedInputStream::CurrentPosition( ) != sizeBaseHeader( ) )
				return false;
			return true;
		}
	};
	class Output : protected BaseHeader {
		messageData_t m_messageData;
		std::shared_ptr< ArrayOutputStream > m_aos;
		std::shared_ptr< CodedOutputStream > m_ostream;

		// Pay. Dont want to calculate packet length before creation
		void realloc_(size_t adding) {
			if ( m_messageData.size( ) >= m_ostream ->ByteCount( ) + adding )
				return;
			size_t newSize = m_messageData.size( ) + adding;
			// DEBUG
			messageData_t::value_type *oldPtr = m_messageData.data( );
			int ByteCount = m_ostream ->ByteCount( );
			// flush
			m_ostream ->HadError( );
			m_messageData.resize( newSize );
			m_ostream.reset( );
			m_aos.reset( );
			m_aos = std::make_shared< ArrayOutputStream >( m_messageData.data( ), newSize );
			m_ostream = std::make_shared< CodedOutputStream >( m_aos.get( ) );
			m_ostream ->Skip( ByteCount );
		}
		template<typename T> std::enable_if_t< std::is_same_v< T, uint32_t>, void > 
		resize_(T v) {
			realloc_( sizeof( v ) );
		}
		template<typename T> std::enable_if_t< std::is_same_v< T, std::string>, void > 
		resize_(T v) {
			realloc_( v.length( ) );
		}
		template<typename T> std::enable_if_t< std::is_same_v< T, messageData_t>, void > 
		resize_(T v) {
			realloc_( v.size( ) );
		}
	public:
		// TODO(alex): accept `Command commandExpect` and sizebuf, 
		// and return buf on new method `finalize` to chech hadError and throw on error
//		Output(messageData_t &messageData) :
//			CodedOutputStream( &m_aos )
//			, m_aos( messageData.data( ), messageData.size( ) )
//			, m_ostream( &m_aos ) // tmp
//		{}
//		void writeHeader(Command command) {
//			WriteLittleEndian32( c_magic );
//			WriteLittleEndian32( c_version );
//			WriteLittleEndian32( (uint32_t)command );
//			if ( CodedOutputStream::ByteCount( ) != size( ) )
//				throw std::runtime_error( "header is filled in incorrectly" );
//		}
		Output(Command command) :
			m_messageData( sizeBaseHeader( ) )
			, m_aos( std::make_shared< ArrayOutputStream >( m_messageData.data( ), sizeBaseHeader( ) ) )
			, m_ostream( std::make_shared< CodedOutputStream >( m_aos.get( ) ) )
		{
			m_ostream ->WriteLittleEndian32( c_magic );
			m_ostream ->WriteLittleEndian32( c_version );
			m_ostream ->WriteLittleEndian32( (uint32_t)command );
			if ( m_ostream ->ByteCount( ) != sizeBaseHeader( ) )
				throw std::runtime_error( "internal error, header is filled in incorrectly" );
		}
		// +TODO(alex): auto-increment buffer size
		// +TODO(alex): write payload its not header entity!
		//auto &writer() {
		//	return m_ostream;
		//}
		//messageData_t get() {
		//	if ( m_ostream ->HadError( ) )
		//		throw std::runtime_error( "unkown write error" );
		//	if ( m_payloadSize != m_ostream ->ByteCount( ) )
		//		throw std::runtime_error( "filled in not to end" );
		//	return m_messageData;
		//}

		// Pay. Parentheses-less
		Output &operator <<(uint32_t n) {
			return resize_( n ), m_ostream ->WriteLittleEndian32( n ), *this;
		}
		Output &operator <<(const std::string &s) {
			return resize_( s ), m_ostream ->WriteString( s ), *this;
		}
		template<typename T>
		Output &operator <<(const T &v) {
			return resize_( v ), m_ostream ->WriteRaw( v.data( ), v.size( ) ), *this;
		}
		operator messageData_t() {
			// flush
			if ( m_ostream ->HadError( ) )
				throw std::runtime_error( "unknown write error" );
			return m_messageData;
		}
	};

public:
	// Construct packet CallingSend and serializeToArray
	struct send {
		static
		messageData_t calling(const std::string &fullySpecifiedMethod, const messageData_t &methodData) {
			return Output( Command::CallingSend )
					<< static_cast<unsigned int>( fullySpecifiedMethod.length( ) )
					<< fullySpecifiedMethod
					<< static_cast<unsigned int>( methodData.size( ) )
					<< methodData
				;
		}
	};
	struct recv {
	static
	messageData_t calling(const messageData_t &messageData, std::string *fullySpecifiedMethod, messageData_t *methodData) {
		InputHeader input( Command::CallingResult, messageData );
		unsigned int lenMeth, lenData;
		input
				//>> static_cast<unsigned int>( fullySpecifiedMethod ->length( ) )
				//>> fullySpecifiedMethod
				//>> static_cast<unsigned int>( methodData.size( ) )
				//>> methodData
			;
		//InputHeader header( messageData );
		//header.readHeader( Command::CallingSend );
		//unsigned int lenMeth, lenData;
		//if ( !header.ReadLittleEndian32( &lenMeth ) )
		//	return false;
		//if ( lenMeth > BaseHeader::c_maxStringLength )
		//	throw std::runtime_error( "fully-specified method name URI exceeds maximum string length" );
		//if ( !header.ReadString( fullySpecifiedMethod, lenMeth ) )
		//	return false;
		//if ( !header.ReadLittleEndian32( &lenData ) )
		//	return false;
		//if ( lenData > BaseHeader::c_maxDataLength )
		//	throw std::runtime_error( "data length exceeds maximum length" );
		//methodData ->resize( lenData );
		//if ( !header.ReadRaw( methodData ->data( ), lenData ) )
		//	return false;
		//return true;
		return { };
	}
	};
};
} // namespace syscross::HelloEOS::gRpc
