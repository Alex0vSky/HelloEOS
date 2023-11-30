// src\Async\IMultiplex.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async {
typedef std::packaged_task< Networking::messageData_t(bool *) > packaged_task_t;
enum class Direction { Outgoing, Incoming };

struct IMultiplex {
	virtual void outgoing(packaged_task_t &&) = 0;
	virtual bool pop(packaged_task_t*, Direction *direction) = 0;
	virtual ~IMultiplex() {}
};

typedef std::shared_ptr< IMultiplex > multiplex_t;
} // namespace syscross::HelloEOS::Async
