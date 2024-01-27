// src\InitializeEOS.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS {
class InitializeEOS {
	EOS_HPlatform m_PlatformHandle = nullptr;

	static void EOS_CALL EOSSDKLoggingCallback_(const EOS_LogMessage* InMsg) {
		LOG( "[%s] %s", InMsg ->Category, InMsg ->Message );
		::OutputDebugStringA( InMsg ->Category );
		::OutputDebugStringA( InMsg ->Message );
		::OutputDebugStringA( "\n" );
	}
	
public:
	~InitializeEOS() {
		if ( m_PlatformHandle )
			::EOS_Platform_Release( m_PlatformHandle );
		::EOS_Shutdown( );
	}
	EOS_HPlatform initialize() {
		EOS_InitializeOptions SDKOptions = { };
		SDKOptions.ApiVersion = EOS_INITIALIZE_API_LATEST;
		SDKOptions.AllocateMemoryFunction = nullptr;
		SDKOptions.ReallocateMemoryFunction = nullptr;
		SDKOptions.ReleaseMemoryFunction = nullptr;
		SDKOptions.ProductName = SampleConstants::GameName;
		SDKOptions.ProductVersion = "1.0";
		SDKOptions.SystemInitializeOptions = nullptr;
		SDKOptions.OverrideThreadAffinity = nullptr;
		EOS_EResult InitResult = ::EOS_Initialize(&SDKOptions);
		if ( InitResult != EOS_EResult::EOS_Success ) {
			LOG( "[InitializeEOS] EOS_Initialize" );
			throw std::runtime_error( "error EOS_Initialize" );
		}

		EOS_EResult SetLogCallbackResult = ::EOS_Logging_SetCallback( &EOSSDKLoggingCallback_ );
		if ( EOS_EResult::EOS_Success != SetLogCallbackResult ) {
			LOG( "[InitializeEOS] EOS_Logging_SetCallback" );
			throw std::runtime_error( "error EOS_Logging_SetCallback" );
		}
		//::EOS_Logging_SetLogLevel( EOS_ELogCategory::EOS_LC_ALL_CATEGORIES, EOS_ELogLevel::EOS_LOG_VeryVerbose );
		::EOS_Logging_SetLogLevel( EOS_ELogCategory::EOS_LC_ALL_CATEGORIES, EOS_ELogLevel::EOS_LOG_Info );

		EOS_Platform_Options PlatformOptions = { };
		PlatformOptions.ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
		PlatformOptions.bIsServer = false;
		PlatformOptions.EncryptionKey = SampleConstants::EncryptionKey;
		PlatformOptions.OverrideCountryCode = nullptr;
		PlatformOptions.OverrideLocaleCode = nullptr;
		// Disable overlay support
		PlatformOptions.Flags = 
			EOS_PF_DISABLE_OVERLAY | EOS_PF_DISABLE_SOCIAL_OVERLAY;
		PlatformOptions.CacheDirectory = "C:\\Temp\\";

		PlatformOptions.ProductId = SampleConstants::ProductId;
		PlatformOptions.SandboxId = SampleConstants::SandboxId;
		PlatformOptions.DeploymentId = SampleConstants::DeploymentId;
		PlatformOptions.ClientCredentials.ClientId = SampleConstants::ClientCredentialsId;
		PlatformOptions.ClientCredentials.ClientSecret = SampleConstants::ClientCredentialsSecret;

		EOS_Platform_RTCOptions RtcOptions = { };
		RtcOptions.ApiVersion = EOS_PLATFORM_RTCOPTIONS_API_LATEST;
		RtcOptions.BackgroundMode = EOS_ERTCBackgroundMode::EOS_RTCBM_LeaveRooms;
		// Windows spec
		EOS_Windows_RTCOptions WindowsRtcOptions = { };
		WindowsRtcOptions.ApiVersion = EOS_WINDOWS_RTCOPTIONS_API_LATEST;
		// TODO(alex): removeme
		WindowsRtcOptions.XAudio29DllPath = R"(C:\Prj\_L\3d\EOS_SDK\SDK\Bin\x64\xaudio2_9redist.dll)";
		RtcOptions.PlatformSpecificOptions = &WindowsRtcOptions;
		PlatformOptions.RTCOptions = &RtcOptions;

		// Zero is interpreted as "perform all available work".
		PlatformOptions.TickBudgetInMilliseconds = 0;

		// FPlatform::GetPlatformHandle( )
		m_PlatformHandle = ::EOS_Platform_Create( &PlatformOptions );
		if (PlatformOptions.IntegratedPlatformOptionsContainerHandle)
			::EOS_IntegratedPlatformOptionsContainer_Release( PlatformOptions.IntegratedPlatformOptionsContainerHandle );
		if ( !m_PlatformHandle ) {
			LOG( "[InitializeEOS] EOS_Platform_Create" );
			throw std::runtime_error( "error EOS_Platform_Create" );
		}
		return m_PlatformHandle;
	}

	EOS_HPlatform getPlatformHandle() const {
		return m_PlatformHandle;
	}

};
} // namespace syscross::HelloEOS
