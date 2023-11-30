// src\MainAsynchronously.h - code main class, for unittests code coverage
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
//#include "eos_auth.h"
//#include "eos_sdk.h"
//#include "eos_types.h"
//#include "Windows/eos_Windows.h"
//#include "eos_logging.h"
//#include "eos_p2p.h"
//#include "eos_friends.h"
//#include "eos_presence.h"

//#include <iostream>
//#include <atomic>
//#include <thread>
//#include <vector>
//#include <future>
//#include <coroutine>
//#include <chrono>

#include "Version.h"
#include "Log.h"
#include "HumanReadable.h"
#include "Credentials/Hardcode.h"
#include "InitializeEOS.h"
#include "Networking.h"
#include "Synchronously/Auth.h"
#include "Synchronously/Friend.h"
#include "Synchronously/AccountMapping.h"
#include "Synchronously/Presence.h"
#include "Synchronously/PresenceQueryable.h"
#include "Deferred/Ctx.h"
#include "Deferred/ConnectionRequestListener/AcceptEveryone.h"
#include "Deferred/ConnectionRequestListener/AcceptEveryoneConnectionAware.h"
#include "Deferred/QueueCommands.h"
#include "Deferred/Action.h"
#include "Deferred/Sender/SendText.h"
#include "Deferred/Receiver/RecvText.h"
#include "Deferred/Sending.h"
#include "Deferred/Receiving.h"
#include "Deferred/PingPonger.h"
// 
namespace syscross::HelloEOS::Async {
struct Context {
	const EOS_HPlatform m_platformHandle;
	const std::string m_socketName;
	EOS_ProductUserId const m_localUserId, m_friendLocalUserId;
	// tmp
	typedef std::packaged_task< Networking::messageData_t() > packaged_task_t;
	void outgoing(packaged_task_t &&) {
	}
};
} // namespace syscross::HelloEOS::Async
#include "Async/PrepareEos.h"
#include "Async/SuperClass.h"
namespace syscross::HelloEOS::Async {
typedef std::packaged_task< Networking::messageData_t() > packaged_task_t;
struct IMultiplex {
	virtual void outgoing(packaged_task_t &&) = 0;
	virtual std::optional<packaged_task_t> pop() = 0;
	virtual ~IMultiplex() {}
};
typedef std::shared_ptr< IMultiplex > multiplex_t;
//typedef	IMultiplex * multiplex_t;
struct Multiplex : public IMultiplex {
	// +TODO(alex): queue functional part, noncopyable
	enum class Direction { Outgoing, Incoming };
	typedef std::pair< packaged_task_t, Direction > queue_elem_t;
	std::queue< queue_elem_t > m_fifo;
	std::mutex m_mu;
	void outgoing(packaged_task_t &&task) override {
		std::lock_guard lock( m_mu );
		m_fifo.push( std::make_pair( std::move( task ), Direction::Outgoing ) );
	}
	std::optional<packaged_task_t> pop() override { // tmp 
		if ( m_fifo.empty( ) )
			return std::optional<packaged_task_t>{ };
		queue_elem_t x;
		{ std::lock_guard lock( m_mu );
			x = std::move( m_fifo.front( ) );
			m_fifo.pop( );
		}
		return std::optional<packaged_task_t>( std::move( x.first ) );
	}
	virtual ~Multiplex() {}
};
typedef std::shared_ptr< PrepareEos::Prepared > eos_t;
// TODO(alex): to separate MapContexts... !!!its useless class!!!
class ContextFactory {
	eos_t m_prepared;
	std::unordered_map< std::string, Async::Context > m_contexts;
public:
	ContextFactory(eos_t prepared) :
		m_prepared( prepared )
	{}
	auto getContext(std::string const& m_socketName) {
	//std::unordered_map< std::string, Async::SuperClass > m_contexts;
		auto it = m_contexts.find( m_socketName );
		if ( m_contexts.end( ) != it )
			return it ->second;
		//auto pair = std::make_pair( m_socketName, Async::SuperClass( &m_prepared, m_socketName ) );
		auto pair = std::make_pair( m_socketName, Async::Context{
				m_prepared ->m_platformHandle
				, m_socketName
				, m_prepared ->m_auth ->getLocalUserId( )
				, m_prepared ->m_mapping ->getFriendLocalUserId( )
			} );
		m_contexts.insert( pair );
		return pair.second;
	}
};
} // namespace syscross::HelloEOS::Async
#include "Async/Send.h"
#include "Async/Thread/Ticker.h"
//#include "Async/Thread/JThread.h"
//namespace syscross::HelloEOS::Async {
//typedef std::unique_ptr< Thread::JThread > ticker_t;
//ticker_t Thread::Ticker::create(Thread::Ticker::Startup const& startup) {
//	auto ticker = ticker_t( new JThread( startup ) );
//	while ( !ticker ->m_bPrepared )
//		std::this_thread::yield( );
//	return ticker;
//}
//} // namespace syscross::HelloEOS::Async

struct Foo {
	EOS_HPlatform m_platformHandle;
	std::unique_ptr< int > m_init;
	Foo() :
		m_platformHandle( 0 )
	{ std::cout << "Foo\n"; }
	~Foo() 
	{ std::cout << "~Foo\n"; }
};
struct Bar {
	std::shared_ptr< Foo > qwe2;
};
namespace syscross::HelloEOS { struct MainAsynchronously {
	void run(int argc) {
		bool isServer = ( argc > 1 );
		//Foo foo0;
		//////Foo foo1 = foo0;
		//Foo foo1 = std::move( foo0 );
		//////Foo foo2( foo0 );
		//Foo foo2( std::move( foo0 ) );
		//////auto prepared0 = Async::PrepareEos::ordinary( isServer );
		//////if ( !prepared0 ) 
		//////	return;
		//////Async::SuperClass ctx( prepared0.value( ), "CHAT" );

		//{
		//	std::shared_ptr< Foo > qwe1;
		//	{
		//		auto qwe0 = std::make_shared< Foo >( );
		//		qwe1 = qwe0;
		//		__nop( );
		//	}
		//}
		//{
		//	std::shared_ptr< Foo > qwe1;
		//	{
		//		Bar bar;
		//		bar.qwe2 = std::make_shared< Foo >( );
		//		qwe1 = bar.qwe2;
		//	}
		//}
		//new char[6]{ "Hello" }; // leak
		//return;

		//Async::Thread::Ticker ticker( { isServer, /*, ctx */} );
		auto ticker = Async::Thread::Ticker::createTicker( { isServer } );
		//Async::Send send0 = ticker.getFactory( "CHAT" ).createSender( );
		//auto chat_ctx = ticker.getContext( "CHAT" );
		//Async::SuperClass superClass( eos, "CHAT" );
		//Async::Send send( chat_ctx );
		Async::Send send = ticker ->createSender( "CHAT" );
		Async::Send::future_t commandSendText = send.text( "asd" ); // std::future
		auto zxc0 = commandSendText.get( ); // waiting...

		LOG( "[~] press [Enter] to exit" );
		getchar( );
		return;
	}
};
} // namespace syscross::HelloEOS
