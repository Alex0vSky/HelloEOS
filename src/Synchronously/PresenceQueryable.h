// src\Synchronously\PresenceQueryable.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously {
class PresenceQueryable : public Presence {
	std::atomic_bool m_bError = false;
	std::atomic_bool m_bQueried = false;

	static void EOS_CALL onQueryPresenceCompleteCallback_(const EOS_Presence_QueryPresenceCallbackInfo* Data) {
		if ( !Data )
			return;
		// Operation is retrying so it is not complete yet
		if ( EOS_FALSE == EOS_EResult_IsOperationComplete( Data ->ResultCode ) )
			return;
		PresenceQueryable *self = reinterpret_cast<PresenceQueryable *>( Data ->ClientData );
		if ( EOS_EResult::EOS_Success != Data ->ResultCode ) {
			self ->m_bError = true;
			self ->m_bQueried = true;
			LOG( "[onQueryPresenceCompleteCallback_] Query presence failed: %s.", EOS_EResult_ToString( Data->ResultCode ) );
			return;
		}
		self ->m_bError = false;
		self ->m_bQueried = true;
		//LOG( "[onQueryPresenceCompleteCallback_] Query presence success");
		return;
	}

public:
	PresenceQueryable(EOS_HPlatform platformHandle, EOS_EpicAccountId epicAccountId) :
		Presence( platformHandle, epicAccountId )
	{}

	bool query(EOS_EpicAccountId targetUserId, EOS_Presence_EStatus *status) {
		EOS_EResult Result;
		EOS_HPresence hPresence = ::EOS_Platform_GetPresenceInterface( m_platformHandle );
		{
			m_bError = false;
			m_bQueried = false;
			::EOS_Presence_QueryPresenceOptions options = { EOS_PRESENCE_QUERYPRESENCE_API_LATEST };
			options.LocalUserId = m_epicAccountId;
			options.TargetUserId = targetUserId;
			::EOS_Presence_QueryPresence( hPresence, &options, this, onQueryPresenceCompleteCallback_ );
			do {
				::EOS_Platform_Tick( m_platformHandle );
				std::this_thread::sleep_for( std::chrono::milliseconds{ 1 } );
			} while( !m_bQueried );
		}
		if ( m_bError )
			return false;

		{
			::EOS_Presence_HasPresenceOptions options = { EOS_PRESENCE_HASPRESENCE_API_LATEST };
			options.LocalUserId = m_epicAccountId;
			options.TargetUserId = targetUserId;
			if ( EOS_FALSE == ::EOS_Presence_HasPresence( hPresence, &options ) ) {
				LOG( "[query] fail EOS_Presence_HasPresence");
				return false;
			}
		}
		{
			::EOS_Presence_CopyPresenceOptions options = { EOS_PRESENCE_COPYPRESENCE_API_LATEST };
			options.LocalUserId = m_epicAccountId;
			options.TargetUserId = targetUserId;
			EOS_Presence_Info *outPresence;
			Result = ::EOS_Presence_CopyPresence( hPresence, &options, &outPresence );
			if ( EOS_EResult::EOS_Success != Result ) {
				LOG( "[query] fail EOS_Presence_CopyPresence");
				return false;
			}
			*status = outPresence ->Status;
			//LOG( "[query] get status for: '%s'", HumanReadable::EpicAccountIDToString_( targetUserId ) );
			EOS_Presence_Info_Release( outPresence );
		}
		return true;
	}
};
} // namespace syscross::HelloEOS::Synchronously
