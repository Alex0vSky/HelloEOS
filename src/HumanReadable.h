// src\HumanReadable.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS {
struct HumanReadable {
	static char const* EpicAccountIDToString_(EOS_EpicAccountId InAccountId) {
		if (InAccountId == nullptr)
		{
			return "NULL";
		}

		static char TempBuffer[EOS_EPICACCOUNTID_MAX_LENGTH + 1];
		int32_t TempBufferSize = sizeof(TempBuffer);
		EOS_EResult Result = EOS_EpicAccountId_ToString(InAccountId, TempBuffer, &TempBufferSize);

		if (Result == EOS_EResult::EOS_Success)
		{
			return TempBuffer;
		}

		LOG( "[EpicAccountIDToString_] Error: %d", (int32_t)Result );

		return "ERROR";
	}

	static char const* ProductUserIDToString_(EOS_ProductUserId AccountId) {
		if (AccountId == nullptr)
		{
			return "NULL";
		}

		static char TempBuffer[EOS_EPICACCOUNTID_MAX_LENGTH + 1];
		int32_t TempBufferSize = sizeof(TempBuffer);
		EOS_EResult Result = EOS_ProductUserId_ToString( AccountId, TempBuffer, &TempBufferSize);

		if (Result == EOS_EResult::EOS_Success)
		{
			return TempBuffer;
		}

		LOG( "[ProductUserIdToString_] Error: %d", (int32_t)Result );

		return "ERROR";
	}

	static char const* FriendStatusToString_(EOS_EFriendsStatus status) {
		switch (status)
		{
		case EOS_EFriendsStatus::EOS_FS_NotFriends:
			return "Not Friends";
		case EOS_EFriendsStatus::EOS_FS_InviteSent:
			return "Invite Sent";
		case EOS_EFriendsStatus::EOS_FS_InviteReceived:
			return "Invite Received";
		case EOS_EFriendsStatus::EOS_FS_Friends:
			return "Friends";
		}

		return "Unknown";
	}
};
} // namespace syscross::HelloEOS
