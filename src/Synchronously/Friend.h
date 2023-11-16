// src\Synchronously\Friend.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously {
class Friend {
public:
	struct FFriendData {
		EOS_EpicAccountId LocalUserId;
		EOS_EpicAccountId UserId;
		std::wstring Name;
		EOS_EFriendsStatus Status;
	};
	typedef std::vector< FFriendData > friends_t;

private:
	EOS_HPlatform m_PlatformHandle;
	EOS_EpicAccountId m_epicAccountId;
	friends_t m_NewFriends;
	std::atomic_bool m_bFriends = false;

	static void EOS_CALL QueryFriendsCompleteCallbackFn(const EOS_Friends_QueryFriendsCallbackInfo* Data) {
		Friend *self = reinterpret_cast<Friend *>( Data ->ClientData );
		if ( false ) {
		} else if ( EOS_EResult::EOS_Success == Data ->ResultCode ) {
			LOG( "[QueryFriendsCompleteCallbackFn] EOS_Success" );
		} else {
			LOG( "[QueryFriendsCompleteCallbackFn] %d", (int)Data ->ResultCode );
			return;
		}
		LOG( "[QueryFriendsCompleteCallbackFn] Query Friends Complete - User ID: %s"
				, HumanReadable::EpicAccountIDToString_( Data ->LocalUserId ) );

		EOS_HFriends FriendsHandle = ::EOS_Platform_GetFriendsInterface( self ->m_PlatformHandle );
		EOS_Friends_GetFriendsCountOptions FriendsCountOptions = {};
		FriendsCountOptions.ApiVersion = EOS_FRIENDS_GETFRIENDSCOUNT_API_LATEST;
		FriendsCountOptions.LocalUserId = Data ->LocalUserId;
		int32_t FriendCount = ::EOS_Friends_GetFriendsCount( FriendsHandle, &FriendsCountOptions );

		LOG( "[QueryFriendsCompleteCallbackFn] NumFriends: %d", FriendCount );

		EOS_Friends_GetFriendAtIndexOptions IndexOptions = {};
		IndexOptions.ApiVersion = EOS_FRIENDS_GETFRIENDATINDEX_API_LATEST;
		IndexOptions.LocalUserId = Data ->LocalUserId;
		for (int32_t FriendIdx = 0; FriendIdx < FriendCount; ++FriendIdx) {
			IndexOptions.Index = FriendIdx;
			EOS_EpicAccountId FriendUserId = ::EOS_Friends_GetFriendAtIndex(FriendsHandle, &IndexOptions);

			if ( !::EOS_EpicAccountId_IsValid( FriendUserId ) ) {
				LOG( "[QueryFriendsCompleteCallbackFn] Friend ID was invalid!" );
				continue;
			}
			EOS_Friends_GetStatusOptions StatusOptions = {};
			StatusOptions.ApiVersion = EOS_FRIENDS_GETSTATUS_API_LATEST;
			StatusOptions.LocalUserId = Data ->LocalUserId;
			StatusOptions.TargetUserId = FriendUserId;
			EOS_EFriendsStatus FriendStatus = ::EOS_Friends_GetStatus(FriendsHandle, &StatusOptions);

			LOG( "[QueryFriendsCompleteCallbackFn] FriendStatus: %s: %s"
					, HumanReadable::EpicAccountIDToString_( FriendUserId )
					, HumanReadable::FriendStatusToString_( FriendStatus )
				);

			FFriendData FriendDataEntry;
			FriendDataEntry.LocalUserId = Data ->LocalUserId;
			FriendDataEntry.UserId = FriendUserId;
			FriendDataEntry.Name = L"Pending...";
			FriendDataEntry.Status = FriendStatus;

			self ->m_NewFriends.push_back(FriendDataEntry);
		}
		self ->m_bFriends = true;
		return;
	}
public:
	Friend(EOS_HPlatform platformHandle, EOS_EpicAccountId epicAccountId) :
		m_PlatformHandle( platformHandle )
		, m_epicAccountId( epicAccountId )
	{}

	friends_t getAll(bool doTick = true) {
		EOS_HFriends FriendsHandle = ::EOS_Platform_GetFriendsInterface( m_PlatformHandle );
		EOS_Friends_QueryFriendsOptions QueryFriendsOptions = {};
		QueryFriendsOptions.ApiVersion = EOS_FRIENDS_QUERYFRIENDS_API_LATEST;
		QueryFriendsOptions.LocalUserId = m_epicAccountId;
		m_bFriends = false;
		::EOS_Friends_QueryFriends( FriendsHandle, &QueryFriendsOptions, this, QueryFriendsCompleteCallbackFn );
		do {
			if ( doTick ) ::EOS_Platform_Tick( m_PlatformHandle );
			std::this_thread::sleep_for( std::chrono::milliseconds{ 1 } );
		} while( !m_bFriends );
		return m_NewFriends;
	}
};
} // namespace syscross::HelloEOS::Synchronously
