// src\Async\PrepareEos.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Async::detail_ {
struct PrepareEos {
	struct Prepared {
		const EOS_HPlatform m_platformHandle;
		const std::unique_ptr< InitializeEOS > m_init;
		const std::unique_ptr< Synchronously::Auth > m_auth;
		const std::unique_ptr< Synchronously::Friend > m_friends;
		const std::unique_ptr< Synchronously::AccountMapping > m_mapping;
		const std::unique_ptr< Synchronously::PresenceQueryable > m_presence;
	};
	typedef std::unique_ptr< PrepareEos::Prepared > prepared_t;
	static prepared_t ordinary(bool isServer) {
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
	
		auto friends = std::make_unique< Synchronously::Friend >
			( platformHandle, auth ->getAccount( ) );
		auto allFriends = friends ->getAll( );
		if ( allFriends.empty( ) )
			return { };

		auto mapping = std::make_unique< Synchronously::AccountMapping >
			( platformHandle, auth ->getLocalUserId( ), allFriends );
		if ( !mapping ->getFirstFriendId( ) )
			return { };
	
		auto presence = std::make_unique< Synchronously::PresenceQueryable  >
			( platformHandle, auth ->getAccount( ) );
		if ( !presence ->setOnlineAndTitle( ) )
			return { };
		return prepared_t( new PrepareEos::Prepared {
				platformHandle
				, std::move( init )
				, std::move( auth )
				, std::move( friends )
				, std::move( mapping )
				, std::move( presence )
			} );
	}
};
} // namespace syscross::HelloEOS::Async::detail_
