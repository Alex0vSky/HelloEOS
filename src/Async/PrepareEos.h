// src\Async\PrepareEos.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async {
struct PrepareEos {
	struct Prepared {
		const EOS_HPlatform m_platformHandle;
		std::unique_ptr< InitializeEOS > m_init;
		std::unique_ptr< Synchronously::Auth > m_auth;
		std::unique_ptr< Synchronously::Friend > m_friends;
		std::unique_ptr< Synchronously::AccountMapping > m_mapping;
		std::unique_ptr< Synchronously::PresenceQueryable > m_presence;
	};
	static 
	std::optional< Prepared > ordinary(bool isServer) {
		auto init = std::make_unique< InitializeEOS >( );
		EOS_HPlatform platformHandle = init ->initialize( );
		if ( !platformHandle )
			return { };

		auto auth = std::make_unique< Synchronously::Auth >( platformHandle );
		std::string tokenDevAuthToolAuth;
		if ( isServer )
			tokenDevAuthToolAuth = "cred2";
		else
			tokenDevAuthToolAuth = "cred1";
		if ( !auth ->connectAndLogin( tokenDevAuthToolAuth ) )
			return { };
		LOG( "[~] auth ->getLocalUserId( ) valid: %s", ( ::EOS_ProductUserId_IsValid( auth ->getLocalUserId( ) ) ?"TRUE" :"FALSE" ) );

		auto friends = std::make_unique< Synchronously::Friend >
			( platformHandle, auth ->getAccount( ) );
		auto allFriends = friends ->getAll( );
		if ( allFriends.empty( ) )
			return { };

		auto mapping = std::make_unique< Synchronously::AccountMapping >
			( platformHandle, auth ->getLocalUserId( ), allFriends );
		if ( !mapping ->getFirstFriendId( ) )
			return { };
		LOG( "[~] mapping ->getFriendLocalUserId( ) valid: %s", ( ::EOS_ProductUserId_IsValid( mapping ->getFriendLocalUserId( ) ) ?"TRUE" :"FALSE" ) );

		auto presence = std::make_unique< Synchronously::PresenceQueryable  >
			( platformHandle, auth ->getAccount( ) );
		if ( !presence ->setOnlineAndTitle( ) )
			return { };
		return Prepared {
				platformHandle
				, std::move( init )
				, std::move( auth )
				, std::move( friends )
				, std::move( mapping )
				, std::move( presence )
			};
	}
};
} // namespace syscross::HelloEOS::Async
