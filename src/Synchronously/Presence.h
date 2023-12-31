// src\Synchronously\Presence.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously {
class Presence {
	std::atomic_bool m_bError = false;
	std::atomic_bool m_bPresence = false;

protected: 
	EOS_HPlatform m_platformHandle;
	EOS_EpicAccountId m_epicAccountId;

private: 
	static void EOS_CALL SetPresenceCallbackFn(const EOS_Presence_SetPresenceCallbackInfo* Data) {
		if ( !Data )
			return;
		// Operation is retrying so it is not complete yet
		if ( EOS_FALSE == EOS_EResult_IsOperationComplete( Data->ResultCode ) )
			return;
		Presence *self = reinterpret_cast<Presence *>( Data ->ClientData );
		if ( EOS_EResult::EOS_Success != Data->ResultCode ) {
			self ->m_bError = true;
			self ->m_bPresence = true;
			LOG( "[SetPresenceCallbackFn] Set presence failed: %s.", EOS_EResult_ToString( Data->ResultCode ) );
			return;
		}
		self ->m_bError = false;
		self ->m_bPresence = true;
		LOG( "[SetPresenceCallbackFn] Set presence success");
		return;
	}

public:
	Presence(EOS_HPlatform platformHandle, EOS_EpicAccountId epicAccountId) :
		m_platformHandle( platformHandle )
		, m_epicAccountId( epicAccountId )
	{}

	bool setOnlineAndTitle(const std::string &RichText = "Using P2P NAT", bool doTick = true) {
		m_bPresence = false;
		m_bError = false;
		EOS_EResult Result;
		EOS_Presence_CreatePresenceModificationOptions CreateModOpt = { };
		CreateModOpt.ApiVersion = EOS_PRESENCE_CREATEPRESENCEMODIFICATION_API_LATEST;
		CreateModOpt.LocalUserId = m_epicAccountId;

		EOS_HPresence PresenceHandle = ::EOS_Platform_GetPresenceInterface( m_platformHandle );

		EOS_HPresenceModification PresenceModification;
		Result = ::EOS_Presence_CreatePresenceModification(
			PresenceHandle, &CreateModOpt, &PresenceModification );
		if ( EOS_EResult::EOS_Success != Result ) {
			LOG( "[EOS SDK] Create presence modification failed: %s.", EOS_EResult_ToString( Result ) );
			return false;
		}

		EOS_PresenceModification_SetRawRichTextOptions SetTextOpt = {};
		SetTextOpt.ApiVersion = EOS_PRESENCE_SETRAWRICHTEXT_API_LATEST;
		SetTextOpt.RichText = RichText.c_str();
		Result = ::EOS_PresenceModification_SetRawRichText( PresenceModification, &SetTextOpt );
		if (Result != EOS_EResult::EOS_Success) {
			::EOS_PresenceModification_Release(PresenceModification);
			LOG( "[EOS SDK] Set presence rich text failed: %s.", EOS_EResult_ToString( Result ) );
			return false;
		}

		EOS_Presence_SetPresenceOptions SetOpt = { };
		SetOpt.ApiVersion = EOS_PRESENCE_SETPRESENCE_API_LATEST;
		SetOpt.LocalUserId = m_epicAccountId;
		SetOpt.PresenceModificationHandle = PresenceModification;
		::EOS_Presence_SetPresence( PresenceHandle, &SetOpt, this, SetPresenceCallbackFn );
		::EOS_PresenceModification_Release( PresenceModification );

		do {
			if ( doTick ) ::EOS_Platform_Tick( m_platformHandle );
			std::this_thread::sleep_for( std::chrono::milliseconds{ 1 } );
		} while( !m_bPresence );
		// TODO(alex): full status
//		FDebugLog::Log(L"[EOS SDK] User presence Info retrieved - UserId: %ls", UserId.ToString().c_str());
//		FDebugLog::Log(L"  Status: %ls", FFriends::FriendPresenceToString(Info.Status).c_str());
//		FDebugLog::Log(L"  Application: %ls", Info.Application.c_str());
//		FDebugLog::Log(L"  Rich Text: %ls", Info.RichText.c_str());
//		FDebugLog::Log(L"  Platform: %ls", Info.Platform.c_str());
		return !m_bError;
	}
};
} // namespace syscross::HelloEOS::Synchronously
