// src\Async\IMultiplex.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::detail_::Selector {
typedef std::function<bool()> task_arg_t;
typedef std::packaged_task< Networking::messageData_t(task_arg_t const&) > task_t;
enum class Direction { Outgoing, Incoming };

struct IMux {
	virtual void outgoing(task_t &&) = 0;
	virtual void incoming(task_t &&) = 0;
	virtual ~IMux() {}
};
typedef std::shared_ptr< IMux > multiplex_t;

struct IDemux {
	virtual ~IDemux() {}
	virtual bool pop(task_t*, Direction *) = 0;
};
typedef std::shared_ptr< IDemux > demultiplex_t;
} // namespace syscross::HelloEOS::Async::detail_::Selector

namespace syscross::HelloEOS::Async::Transport {
	using multiplex_t = detail_::Selector::multiplex_t;
	using task_t = detail_::Selector::task_t;
	using task_arg_t = detail_::Selector::task_arg_t;
} // namespace syscross::HelloEOS::Async::Transport
