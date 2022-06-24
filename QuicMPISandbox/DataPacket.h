#pragma once

#include <string>
#include <vector>

#include "msquic.h"

class DataPacket
{
public:
	DataPacket( const std::string zBuffer );
	virtual ~DataPacket();

	const QUIC_BUFFER* GetQuicBuffer() { return &m_QuicBuffer; }
	int GetQuicBufferCount() { return 1; }

	const char* GetBuffer() { return m_zBuffer.c_str(); }
	uint8_t* GetCBuffer() { return &m_aCBuffer[0]; }
	const size_t GetBufferSize() { return m_zBuffer.size(); }

private:
	std::string m_zBuffer;
	std::vector<uint8_t> m_aCBuffer;

	QUIC_BUFFER m_QuicBuffer;
};
