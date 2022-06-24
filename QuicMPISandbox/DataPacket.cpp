#include "DataPacket.h"

#include <vector>

DataPacket::DataPacket( const std::string zBuffer ) :
	m_zBuffer( zBuffer ),
	m_aCBuffer( zBuffer.begin(), zBuffer.end() )
{
	m_QuicBuffer = { static_cast<uint32_t>( GetBufferSize() ), GetCBuffer() };
}

DataPacket::~DataPacket()
{}
