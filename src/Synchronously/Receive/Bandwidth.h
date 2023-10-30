// src\Synchronously\Receive\Bandwidth.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously::Receive {
class Bandwidth : public BaseReceive {
	using BaseReceive::BaseReceive;

	bool checkPacket_(const Networking::messageData_t &messageData, size_t *pj) {
		auto max = std::max( Networking::c_MaxDataSizeBytes, messageData.size( ) );
		if ( max != Networking::c_MaxDataSizeBytes )
			LOG( "[recvAndCheck] cutted packet" );
		size_t i = 0;
		for ( ; i < max; ++i, ++(*pj) ) {
			auto value = static_cast< Networking::messageData_t::value_type >( (*pj) );
			// Simple sequence checking
			if ( messageData[ i ] != value ) {
				LOG( "[recvAndCheck] wrong j: %zd", (*pj) );
				break;
			}
		}
		return i >= max;
	}

public:
	bool recvAndCheck() {
		size_t j = 0;
		Networking::messageData_t messageData;

		LOG( "[~] c_countPackets: %zd", Networking::c_countPackets );
		for ( size_t packet = 0; packet < Networking::c_countPackets; ++packet ) {
			messageData = receive_( ).value( );
			if ( !checkPacket_( messageData, &j ) )
				return false;
		}

		return true;
	}
};
} // namespace syscross::HelloEOS::Synchronously::Receive
