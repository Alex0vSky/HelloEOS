// src\gRpc\Packet\Output.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::gRpc::Packet {
namespace detail_ {
class Output : protected BaseHeader {
	using ArrayOutputStream = google::protobuf::io::ArrayOutputStream;
	using CodedOutputStream = google::protobuf::io::CodedOutputStream;
	using messageData_t = Networking::messageData_t;
	messageData_t m_messageData;
	std::unique_ptr< ArrayOutputStream > m_aos;
	std::unique_ptr< CodedOutputStream > m_ostream;

	// Pay. Dont want to calculate packet length before creation
	void realloc_(size_t adding) {
		if ( m_messageData.size( ) >= m_ostream ->ByteCount( ) + adding )
			return;
		// TODO(alex): new size greater then adding
		size_t newSize = m_messageData.size( ) + adding;
		// DEBUG
		messageData_t::value_type *oldPtr = m_messageData.data( );
		int ByteCount = m_ostream ->ByteCount( );
		// flush
		m_ostream ->HadError( );
		m_messageData.resize( newSize );
		m_ostream.reset( );
		m_aos.reset( );
		m_aos = std::make_unique< ArrayOutputStream >( m_messageData.data( ), newSize );
		m_ostream = std::make_unique< CodedOutputStream >( m_aos.get( ) );
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
	explicit Output(Command command) : 
		m_messageData( sizeBaseHeader( ) )
		, m_aos( std::make_unique< ArrayOutputStream >( m_messageData.data( ), sizeBaseHeader( ) ) )
		, m_ostream( std::make_unique< CodedOutputStream >( m_aos.get( ) ) )
	{
		m_ostream ->WriteLittleEndian32( c_magic );
		m_ostream ->WriteLittleEndian32( c_version );
		m_ostream ->WriteLittleEndian32( (uint32_t)command );
		if ( m_ostream ->ByteCount( ) != sizeBaseHeader( ) )
			throw std::runtime_error( "internal error, header is filled in incorrectly" );
	}
	// Pay. Parentheses-less
	Output &operator <<(uint32_t n) {
		return resize_( n ), m_ostream ->WriteLittleEndian32( n ), *this;
	}
	Output &operator <<(const std::string &s) {
		(*this) << static_cast<unsigned int>( s.length( ) );
		return resize_( s ), m_ostream ->WriteString( s ), *this;
	}
	template<typename T>
	Output &operator <<(const T &v) {
		(*this) << static_cast<unsigned int>( v.size( ) );
		return resize_( v ), m_ostream ->WriteRaw( v.data( ), v.size( ) ), *this;
	}
	operator messageData_t() {
		// flush
		if ( m_ostream ->HadError( ) )
			throw std::runtime_error( "unknown write error" );
		return m_messageData;
	}
};
} // namespace detail_
} // namespace syscross::HelloEOS::gRpc::Packet
