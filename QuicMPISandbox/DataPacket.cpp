#include "DataPacket.h"

DataPacket::DataPacket( const std::string zBuffer ) :
	m_zBuffer( zBuffer ),
	m_aCBuffer( zBuffer.begin(), zBuffer.end() )
{
	// TODO: Is there a way around this size_t to uint32_t conversion...
	m_QuicBuffer = { static_cast<uint32_t>( GetBufferSize() ), GetCBuffer() };
}

DataPacket::~DataPacket()
{}
