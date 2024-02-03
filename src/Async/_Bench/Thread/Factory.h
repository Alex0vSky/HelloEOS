// src\Async\_Bench\Thread\Factory.h - mock for _Bench, see A0S_BENCH_P2P macro
#pragma once // Copyright 2024 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::Thread {
using tcp = boost::asio::ip::tcp;
using error_code = boost::system::error_code;
struct Factory {
	typedef std::unique_ptr< boost::asio::io_context > uptrIoContext_t;
	class MockGameThread {
		Async::detail_::PrepareEos::prepared_t m_uptrOes;
		Async::detail_::TickerCore m_core;
		uptrIoContext_t m_ioContext;
		tcp::socket m_socket;
		std::thread m_thread;
	public:
		MockGameThread(uptrIoContext_t ioContext, tcp::socket &&socket) :
			m_uptrOes( new Async::detail_::PrepareEos::Prepared{ 
					nullptr
					, std::move( nullptr )
					, std::make_unique< Synchronously::Auth >( nullptr )
					, std::move( nullptr )
					, std::make_unique< Synchronously::AccountMapping >
						( nullptr, nullptr, Synchronously::Friend::friends_t{ } )
					, std::move( nullptr )
				} )
			, m_core( m_uptrOes )
			, m_ioContext( std::move( ioContext ) )
			, m_socket( std::move( socket ) )
			// @insp SO how-to-check-if-socket-is-closed-in-boost-asio
			, m_thread( [this] { 
					m_ioContext ->run( );
					char data[ 1 ]; error_code error_code = { }; // boost::asio::error::
					while ( true ) {
						m_socket.receive( boost::asio::buffer( data ), tcp::socket::message_peek, error_code );
						if ( error_code.failed( ) ) {
							__nop( );
						}
						std::this_thread::sleep_for( std::chrono::milliseconds{ 300 } );
					}
				} )
		{}
		~MockGameThread() {
			m_thread.join( );
		}
		Transport::Sender createSender(std::string const& socketName) {
			return Transport::Sender( &m_socket );
		}
		Transport::Recv createReceiver(std::string const& socketName) {
			return Transport::Recv( &m_socket );
		}
	};
	static auto create(bool) {
		uptrIoContext_t ioContext = std::make_unique< boost::asio::io_context >( 1 );
		tcp::socket socket( *ioContext );
		USHORT port = 55555;
		socket.connect( { boost::asio::ip::address_v4::loopback( ), port } );
		BenchP2p::data_t data;
		using namespace BenchP2p;
		while ( true ) {
			socket.send( boost::asio::buffer( BenchP2p::Command::Pair::Hello_ ) );
			socket.receive( boost::asio::buffer( data ) );
			if ( BenchP2p::Command::Pair::Ready_ == data )
				break;
			std::this_thread::sleep_for( std::chrono::milliseconds{ 300 } );
		}
		return std::make_unique< MockGameThread >( std::move( ioContext ), std::move( socket ) );
	}
};
} // namespace syscross::HelloEOS::Async::Thread
