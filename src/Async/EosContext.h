// src\Async\EosContext.h - just POD/Aggregation
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async {
namespace detail_ {
struct EosContext {
	const EOS_HPlatform m_platformHandle;
	EOS_ProductUserId const m_localUserId, m_friendLocalUserId;
};
} // namespace detail_
typedef std::future< Networking::messageData_t > command_t;
} // namespace syscross::HelloEOS::Async

namespace syscross::HelloEOS::Async::Transport {
	using EosContext = detail_::EosContext;
} // namespace syscross::HelloEOS::Async::Transport
