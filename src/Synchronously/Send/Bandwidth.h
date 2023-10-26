// src\Synchronously\Send\Bandwidth.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Synchronously::Send {
class Bandwidth : public BaseSend {
	using BaseSend::BaseSend;

public:
	bool measure(size_t *pBandwith) {
		const auto max = Networking::c_MaxDataSizeBytes;
		Networking::messageData_t messageData;
		messageData.resize( max );
		size_t j = 0;

		LOG( "[~] c_countPackets: %zd", Networking::c_countPackets );

		for ( size_t packet = 0; packet < Networking::c_countPackets; ++packet ) {
			for ( size_t i = 0; i < max; ++i, ++j ) {
				auto value = static_cast< Networking::messageData_t::value_type >( j );
				messageData[ i ] = value;
			}
			if ( !sendPacket_( messageData ) )
				return false;
		}

		// TODO(alex): no way to known deliverance now, and UDP too
		LOG( "[~] press [Ctrl+C] to exit" );
		ticks_( );
		return true;
	}
};
} // namespace syscross::HelloEOS::Synchronously::Send
