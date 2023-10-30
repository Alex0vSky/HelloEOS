// src\Deferred\Ctx.h - context
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred {
struct Ctx {
	const std::string m_SocketName;
	const EOS_HPlatform m_PlatformHandle;
	const EOS_ProductUserId m_LocalUserId;
	const EOS_ProductUserId m_FriendLocalUserId;
};
} // namespace syscross::HelloEOS::Deferred
