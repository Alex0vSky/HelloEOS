// src\Async\_Bench\Transport.h - mock for _Bench, see A0S_BENCH_P2P macro. Naive architecture
#pragma once // Copyright 2024 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Transport {
using tcp = boost::asio::ip::tcp;
struct Sender {
	Sender(EosContext const&, std::string const&, multiplexer_t const&)
	{}
	tcp::socket *m_socket = nullptr;
	explicit Sender(tcp::socket *socket) :
		m_socket( socket )
	{}
	command_t sendText(std::string const& text) const { //Networking::messageData_t data( text.begin( ), text.end( ) );
		boost::asio::write( *m_socket, boost::asio::buffer( text ) );
		return std::promise< Networking::messageData_t >( ).get_future( );
	}
	command_t sendVector(Networking::messageData_t const& vector) const {
		boost::asio::write( *m_socket, boost::asio::buffer( vector ) );
		return std::promise< Networking::messageData_t >( ).get_future( );
	}
};
struct Recv {
	Recv(EosContext const&, std::string const&, multiplexer_t const&)
	{}
	tcp::socket *m_socket = nullptr;
	explicit Recv(tcp::socket *socket) :
		m_socket( socket )
	{}
	command_t byLength(size_t len) const {
		Networking::messageData_t data( len );
		size_t received = boost::asio::read( *m_socket, boost::asio::buffer( data ) );
		LOG( "[Recv] received: %zd", received );

		// trace
		auto str = ( std::stringstream( )<< Hexdump( data.data( ), len ) ).str( ); LOG( "[Recv] '%s' Hexdump of amout bytes: %zu\n%s", "SocketName", len, str.c_str( ) );

		std::promise< Networking::messageData_t > pr;
		pr.set_value( data );
		return pr.get_future( );
	}
};
} // namespace syscross::HelloEOS::Async::Transport
