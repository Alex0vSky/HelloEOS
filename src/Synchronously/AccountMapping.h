// src\Synchronously\AccountMapping.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously {
class AccountMapping {
	const EOS_HPlatform m_PlatformHandle;
	const EOS_ProductUserId m_LocalUserId;
	const Friend::friends_t m_friends;
	EOS_ProductUserId m_FriendLocalUserId;

	std::string ExternalAccountId;
	std::vector<const char*> ExternalAccountsToQuery;

	std::atomic_bool m_bMapping = false;

	static void OnQueryExternalAccountMappingsCallback(const EOS_Connect_QueryExternalAccountMappingsCallbackInfo* Data) {
		AccountMapping *self = reinterpret_cast<AccountMapping *>( Data ->ClientData );
		if ( EOS_EResult::EOS_Success != Data ->ResultCode ) {
			LOG( "[OnQueryExternalAccountMappingsCallback] %d", (int)Data ->ResultCode );
			return;
		}

		LOG( "[OnQueryExternalAccountMappingsCallback] Updated external account mappings successfully" );
		// for (const FEpicAccountId& NextId : CurrentlyQueriedExternalAccounts) // friends
		EOS_Connect_GetExternalAccountMappingsOptions Options = {};
		Options.ApiVersion = EOS_CONNECT_GETEXTERNALACCOUNTMAPPINGS_API_LATEST;
		Options.AccountIdType = EOS_EExternalAccountType::EOS_EAT_EPIC;
		Options.LocalUserId = self ->m_LocalUserId;
		std::string NextIdString = self ->ExternalAccountId.c_str( );
		Options.TargetExternalUserId = NextIdString.c_str();

		EOS_HConnect ConnectHandle = ::EOS_Platform_GetConnectInterface( self ->m_PlatformHandle );
		EOS_ProductUserId NewMapping = ::EOS_Connect_GetExternalAccountMapping( ConnectHandle, &Options );
		if ( !NewMapping ) {
			LOG( "[OnQueryExternalAccountMappingsCallback] no new mappig" );
			return;
		}
		auto b = ::EOS_ProductUserId_IsValid( NewMapping );
		LOG( "[OnQueryExternalAccountMappingsCallback] %s", HumanReadable::ProductUserIDToString_( NewMapping ) );
		//ExternalAccountsMap[NextId] = NewMapping;
		self ->m_FriendLocalUserId = NewMapping;
		self ->m_bMapping = true;
		return;
	}
public:
	AccountMapping(
		EOS_HPlatform platformHandle
		, EOS_ProductUserId localUserId
		, Friend::friends_t friends
	) :
		m_PlatformHandle( platformHandle )
		, m_LocalUserId( localUserId )
		, m_friends( friends )
	{}

	bool getFirstFriendId() {

		ExternalAccountId = HumanReadable::EpicAccountIDToString_( m_friends[ 0 ].UserId );
		ExternalAccountsToQuery.push_back( ExternalAccountId.c_str( ) );

		// ExternalAccountsMap[TargetUserId] + ExternalAccountsMap[NextId] = NewMapping + UpdateExternalAccountMappings()
		EOS_Connect_QueryExternalAccountMappingsOptions QueryOptions = {};
		QueryOptions.ApiVersion = EOS_CONNECT_QUERYEXTERNALACCOUNTMAPPINGS_API_LATEST;
		QueryOptions.AccountIdType = EOS_EExternalAccountType::EOS_EAT_EPIC;
		QueryOptions.LocalUserId = m_LocalUserId;
		QueryOptions.ExternalAccountIdCount = 1;
		QueryOptions.ExternalAccountIds = ExternalAccountsToQuery.data( );
		EOS_HConnect ConnectHandle = ::EOS_Platform_GetConnectInterface( m_PlatformHandle );
		m_bMapping = false;
		::EOS_Connect_QueryExternalAccountMappings( 
			ConnectHandle, &QueryOptions, this, OnQueryExternalAccountMappingsCallback );
		do {
//			::EOS_Platform_Tick( m_PlatformHandle );
			std::this_thread::sleep_for( std::chrono::milliseconds{ 1 } );
		} while( !m_bMapping );
 		return true;
	}
	EOS_ProductUserId getFriendLocalUserId() const {
		return m_FriendLocalUserId;
	}
};
} // namespace syscross::HelloEOS::Synchronously
