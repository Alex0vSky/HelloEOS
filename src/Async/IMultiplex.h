// src\Async\IMultiplex.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async {
typedef std::function<bool()> arg0_packaged_task_t;
typedef std::packaged_task< Networking::messageData_t(arg0_packaged_task_t const&) > packaged_task_t;
enum class Direction { Outgoing, Incoming };

struct IMux {
	virtual void outgoing(packaged_task_t &&) = 0;
	virtual void incoming(packaged_task_t &&) = 0;
	virtual ~IMux() {}
};

struct IDemux {
	virtual ~IDemux() {}
	virtual bool pop(packaged_task_t*, Direction *direction) = 0;
};

typedef std::shared_ptr< IMux > multiplex_t;
typedef std::shared_ptr< IDemux > demultiplex_t;
} // namespace syscross::HelloEOS::Async
