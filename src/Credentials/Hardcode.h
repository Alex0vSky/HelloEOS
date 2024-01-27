// src\Credentials\Hardcode.h - empty creds
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS {
struct SampleConstants {
	// The product id for the running application, found on the dev portal
	static char ProductId[33];

    // The application id for the running application, found on the dev portal
//	static constexpr char ApplicationId[33];
	
	// The sandbox id for the running application, found on the dev portal
    static char SandboxId[33];

	// The deployment id for the running application, found on the dev portal
    static char DeploymentId[33];

	// Client id of the service permissions entry, found on the dev portal
    static char ClientCredentialsId[33];

	// Client secret for accessing the set of permissions, found on the dev portal
    static char ClientCredentialsSecret[44];

	// Game name
	static constexpr char GameName[] = "GameName";

	// Encryption key. Not used by this sample.
	static constexpr char EncryptionKey[] = "1111111111111111111111111111111111111111111111111111111111111111";
};
} // namespace syscross::HelloEOS
