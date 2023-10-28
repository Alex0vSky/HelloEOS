// src\Auth\Synchronously.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously {
class Auth {
	EOS_HPlatform m_PlatformHandle;
	std::atomic_bool m_bError = false;
	std::atomic_bool m_bSuccess = false;
	EOS_EpicAccountId m_Account = nullptr;
	std::atomic< EOS_ProductUserId > m_LocalUserId = nullptr;

	// This function is asynchronous; the EOS SDK guarantees that your callback will run when the operation completes, regardless of whether it succeeds or fails.
	static void LoginCompleteCallbackFn(const EOS_Auth_LoginCallbackInfo* Data) {
		auto self = reinterpret_cast<Auth *>( Data ->ClientData );
		if ( false ) {
		} else if ( EOS_EResult::EOS_Success == Data ->ResultCode ) {
			LOG( "[LoginCompleteCallbackFn] EOS_Success" );
			self ->m_Account = Data ->LocalUserId;
			self ->m_bSuccess = true;
		} else {
			LOG( "[LoginCompleteCallbackFn] %d", (int)Data ->ResultCode );
			self ->m_bError = true;
		}
	}

	static void ConnectLoginCompleteCb(const EOS_Connect_LoginCallbackInfo* Data) {
		auto self = reinterpret_cast<Auth *>( Data ->ClientData );
		if (Data->ResultCode == EOS_EResult::EOS_Success) {
			LOG( "[EOS SDK] Connect Login Complete - ProductUserId: %s"
				, HumanReadable::ProductUserIDToString_( Data->LocalUserId ) );
			//OnConnectLoginComplete( Data ->ResultCode, self ->m_Account, Data ->LocalUserId );
			self ->m_LocalUserId = Data ->LocalUserId;
		} else {
			LOG( "[EOS SDK] Connect Login Complete failed with %s", EOS_EResult_ToString( Data->ResultCode ) );
		}
		return;
	}

public:
	Auth(EOS_HPlatform platformHandle) :
		m_PlatformHandle( platformHandle )
	{}

	bool connectAndLogin(const std::string &Token) {

		EOS_HAuth Handle = ::EOS_Platform_GetAuthInterface( m_PlatformHandle );
		{
			EOS_Auth_Credentials Credentials = { };
			Credentials.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;

			EOS_Auth_LoginOptions Options = { };
			Options.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
			EOS_EAuthScopeFlags DefaultLoginScope = EOS_EAuthScopeFlags::EOS_AS_BasicProfile | EOS_EAuthScopeFlags::EOS_AS_FriendsList | EOS_EAuthScopeFlags::EOS_AS_Presence;
			Options.ScopeFlags = DefaultLoginScope;

			Options.Credentials = &Credentials;
			Credentials.Id = "localhost:10000";
			Credentials.Token = Token.c_str( );
			//Credentials.Token = "credX";
			Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_Developer;
			LOG( "[Auth/Synchronously] Logging In with Host: %s", Credentials.Id );

			::SetLastError( 0 );
			m_bSuccess = false;
			m_bError = false;
			::EOS_Auth_Login( Handle, &Options, this, LoginCompleteCallbackFn );
		}
		// Notification is approximately 10 minutes prior to expiration. ::EOS_Connect_AddNotifyAuthExpiration()
		EOS_ELoginStatus status = (EOS_ELoginStatus)-1;
		do {
//			::EOS_Platform_Tick( m_PlatformHandle );
			std::this_thread::sleep_for( std::chrono::milliseconds{ 1 } );
			EOS_EpicAccountId acc = ::EOS_Auth_GetLoggedInAccountByIndex( Handle, 0 );
			if ( !acc )
				continue;
			status = ::EOS_Auth_GetLoginStatus( Handle, acc ); //} while ( EOS_ELoginStatus::EOS_LS_LoggedIn != status && !m_bError);
		} while ( !m_bSuccess && !m_bError );
		if ( m_bError ) 
			return false;

		LOG( "[Auth/Synchronously] Connect Login - User ID: %s", HumanReadable::EpicAccountIDToString_( m_Account ) );
		EOS_Auth_Token* UserAuthToken = nullptr;
		EOS_Auth_CopyUserAuthTokenOptions CopyTokenOptions = { };
		CopyTokenOptions.ApiVersion = EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST;
		if ( EOS_EResult::EOS_Success != ::EOS_Auth_CopyUserAuthToken( Handle, &CopyTokenOptions, m_Account, &UserAuthToken ) )
			return false;
		EOS_Connect_Credentials Credentials = { };
		Credentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
		Credentials.Token = UserAuthToken ->AccessToken;
		Credentials.Type = EOS_EExternalCredentialType::EOS_ECT_EPIC;

		EOS_Connect_LoginOptions Options = { };
		Options.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
		Options.Credentials = &Credentials;
		Options.UserLoginInfo = nullptr;

		auto ConnectHandle = ::EOS_Platform_GetConnectInterface( m_PlatformHandle );
		m_LocalUserId = nullptr;
		::EOS_Connect_Login( ConnectHandle, &Options, this, ConnectLoginCompleteCb );
		::EOS_Auth_Token_Release( UserAuthToken );
		do {
//			::EOS_Platform_Tick( m_PlatformHandle );
			std::this_thread::sleep_for( std::chrono::milliseconds{ 1 } );
		} while ( !m_LocalUserId );
		// TODO(alex): void FAuthentication::PrintAuthToken(EOS_Auth_Token* InAuthToken)

		return true;
	}
	EOS_EpicAccountId getAccount() const {
		return m_Account;
	}
	EOS_ProductUserId getLocalUserId() const {
		return m_LocalUserId;
	}
};
} // namespace syscross::HelloEOS::Synchronously
