#include "DataPacket.h"

#include <chrono>

DataPacket::DataPacket( const std::string zBuffer ) :
	m_zBuffer( zBuffer ),
	m_aCBuffer( zBuffer.begin(), zBuffer.end() )
{
	// TODO: Is there a way around this size_t to uint32_t conversion...
	m_QuicBuffer = { static_cast<uint32_t>( GetBufferSize() ), GetCBuffer() };
}

DataPacket::~DataPacket()
{}

void
DataPacket::SetTimeSentNow()
{
	m_iTimeSentMs = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	printf( "Setting time sent: %I64d\n", m_iTimeSentMs );
}
