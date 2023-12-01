// src\Async\Environs.h - just POD/Aggregation
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async {
namespace detail_ {
struct Environs {
	const EOS_HPlatform m_platformHandle;
	EOS_ProductUserId const m_localUserId, m_friendLocalUserId;
};
} // namespace detail_
typedef std::future< Networking::messageData_t > future_t;
} // namespace syscross::HelloEOS::Async

namespace syscross::HelloEOS::Async::Transport {
	using Environs = detail_::Environs;
} // namespace syscross::HelloEOS::Async::Transport
